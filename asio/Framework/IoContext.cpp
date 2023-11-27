#include "IoContext.h"
#include <asio.hpp>

struct IoContext::data{
    data()
        : work_(asio::make_work_guard(context_))
    {
    }
    
    asio::io_context context_;
    asio::executor_work_guard<asio::io_context::executor_type> work_;   // 没工作，也不退出 run()
};

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
    data_->context_.run();
}

void IoContext::stop(){
    data_->context_.stop();
}
