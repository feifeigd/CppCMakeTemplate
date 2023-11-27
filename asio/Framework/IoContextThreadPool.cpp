#include "IoContextThreadPool.h"
#include <stdexcept>    // std::runtime_error

IoContextThreadPool::IoContextThreadPool()
{

}

void IoContextThreadPool::start(std::size_t threadNum)
{
    if(threadNum < 1)
        threadNum = 1;
    if(threads_.size() > 0)
        return;
    
    for(std::size_t i = 0; i < threadNum; ++i){
        threads_.emplace_back(std::make_unique<IoContextThread>())->start();
    }
}

void IoContextThreadPool::stop()
{
    for(auto& thread : threads_){
        thread->stop();
    }
    threads_.clear();
}

IoContext& IoContextThreadPool::pickIoContext()
{
    if(threads_.empty())
        throw std::runtime_error("IoContextThreadPool::pickIoContext() no ioContext");
    
    static std::atomic<std::size_t> nextIndex{0};
    auto index = nextIndex.fetch_add(1) % threads_.size();
    return threads_[index]->getIoContext();
}
