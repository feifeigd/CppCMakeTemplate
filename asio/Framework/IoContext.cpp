#include "internal/IoContextData.h"
#include <iostream>

IoContext::IoContext()
    : data_(new data)
{

}

IoContext::~IoContext(){
    stop();

    delete data_;
    data_ = nullptr;
}

void IoContext::run(){
    while (true)
    {
        try{
            data_->context_.run();
            break;  // 正常退出; 如果发生异常, 则重启循环
        }catch(std::exception const& e){
            std::cout << "IoContext::run() exception: " << e.what() << "\n";
        }catch(...){
            std::cout << "IoContext::run() unknown exception\n";
        }
    }    
}

void IoContext::stop(){
    data_->context_.stop();
}

void IoContext::post(std::function<void()>&& task){
    data_->context_.post(std::move(task));
}

bool IoContext::stopped()const{
    return data_->context_.stopped();
}
