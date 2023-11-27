#include "internal/IoContextData.h"


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

void IoContext::post(std::function<void()>&& task){
    data_->context_.post(std::move(task));
}

bool IoContext::stopped()const{
    return data_->context_.stopped();
}
