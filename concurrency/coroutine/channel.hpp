#pragma once

#include "lazy.hpp"

#include <deque>
#include <stdexcept>
#include <optional>
#include <tuple>

template<typename Drived>
struct IntrusiveNode{
    Drived* next{};
};

template<typename T>
class FiFoList{
public:

    void push(T* newNode){
        if(!tail){
            head = tail = newNode;
        }else{
            tail->next = newNode;
            tail = newNode;
        }
    }

    T* pop(){
        if(!head)
            return nullptr;
        auto elem = head;
        head = (head->next);
        if(!head){
            tail = nullptr; // 队列变空
        }
        return elem;
    }

    [[nodiscard]]
    bool empty()const{
        return ! head;
    }
private:
    T *head{}, *tail{};
};

template<typename Type>
class channel{
public:
    channel(size_t buffer_size = 0): buffer_size_{buffer_size}{}

    // awaiter
    struct async_recv: IntrusiveNode<async_recv> {
        async_recv(channel<Type>& channel): channel_{channel} {}
        channel<Type>& channel_;
        std::coroutine_handle<> handle_;

        [[nodiscard]]
        bool await_ready()const{
            return !channel_.fifo_.empty() || !channel_.senders_.empty();
        }

        std::coroutine_handle<> await_suspend(std::coroutine_handle<> handle){
            handle_ = handle;
            channel_.receivers_.push(this);

            if(!channel_.consumeds_.empty()){
                auto send = channel_.consumeds_.pop();
                return send->handle_;
            }

            if(!channel_.senders_.empty()){
                auto send = channel_.senders_.pop();
                return send->handle_;
            }

            return std::noop_coroutine();
        }

        std::tuple<Type, bool> await_resume(){

            if(!channel_.fifo_.empty()){
                auto data = std::move(channel_.fifo_.front());
                channel_.fifo_.pop_front();

                return {std::move(data), true};
            }

            if(!channel_.senders_.empty()){
                auto send = channel_.senders_.pop();
                auto data = std::move(send->data_.value());
                send->data_.reset();
                channel_.consumeds_.push(send);

                return {std::move(data), true};
            }

            if(!channel_.closed_ )
                throw std::runtime_error("unexpected await resume");
            return {};            
        }
    };

    async_recv recv(){
        return {*this};
    }

    // awaiter
    struct async_send: IntrusiveNode<async_send> {
        async_send(channel<Type>& channel, Type&& data): channel_{channel}, data_{data}
        {}

        channel<Type>& channel_;
        std::coroutine_handle<> handle_;
        std::optional<Type> data_;

        [[nodiscard]]
        bool await_ready(){
            if(channel_.full())
                return false;
            
            channel_.fifo_.push_back(std::move(data_.value()));
            data_.reset();
            return true;
        }

        std::coroutine_handle<> await_suspend(std::coroutine_handle<> handle){
            handle_ = handle;
            channel_.senders_.push(this);
            if(!channel_.receivers_.empty()){
                auto recv = channel_.receivers_.pop();
                return recv->handle_;   // 切换到接收者协程
            }
            return std::noop_coroutine();
        }

        void await_resume(){}
    };

    async_send send(Type const& type){
        return {*this, type};
    }
    async_send send(Type && type){
        return {*this, std::move(type)};
    }
    void close(){
        closed_ = true;
    }
    bool closed()const{
        return closed_;
    }

    bool empty()const{
        return closed_ 
            && receivers_.empty() && senders_.empty() && consumeds_.empty();
    }

    // 同步执行
    void sync_await(){
        while ((closed_ || !fifo_.empty()) && !receivers_.empty())
        {
            auto recv = receivers_.pop();
            recv->handle_.resume();
        }

        while (!consumeds_.empty())
        {
            auto send = consumeds_.pop();
            send->handle_.resume();
        }

        while ((closed_ || !full()) && !senders_.empty())
        {
            auto send = senders_.pop();
            send->handle_.resume();
        }
    }

private:
    bool full(){
        return fifo_.size() >= buffer_size_;
    }

    size_t buffer_size_{};
    FiFoList<async_recv> receivers_;
    FiFoList<async_send> senders_;
    FiFoList<async_send> consumeds_;
    std::deque<Type> fifo_;
    bool closed_{};
};
