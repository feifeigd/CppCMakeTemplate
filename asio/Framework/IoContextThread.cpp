#include "IoContextThread.h"

IoContextThread::IoContextThread()
{

}

IoContextThread::~IoContextThread()
{
    stop();    
}

void IoContextThread::start()
{
    if(started_)
        return;
    
    thread_ = std::jthread([this](){
        ioContext_.run();
    });
    started_ = true;
}

void IoContextThread::stop()
{
    ioContext_.stop();
    thread_.join();

    started_ = false;
}

IoContext& IoContextThread::getIoContext()
{
    return ioContext_;
}
