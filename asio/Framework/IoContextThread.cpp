#include "IoContextThread.h"
#include "writer.h"

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
        Writer{} << "IoContextThread start running thread id=" << std::this_thread::get_id(); 
        ioContext_.run();
        Writer{} << "IoContextThread stopped thread id=" << std::this_thread::get_id(); 
    });
    started_ = true;
}

void IoContextThread::stop()
{
    if(ioContext_.stopped() || !started_)
        return;
    ioContext_.stop();
    Writer{} << "IoContextThread::stop thread id=" << std::this_thread::get_id(); 
    thread_.join();

    started_ = false;
}

IoContext& IoContextThread::getIoContext()
{
    return ioContext_;
}
