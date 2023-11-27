#pragma once

#include "IoContext.h"
#include "noncopyable.hpp"
#include <thread>

class IoContextThread: private noncopyable{
public:
    IoContextThread();
    ~IoContextThread();

    void start();
    void stop();

    IoContext& getIoContext();
private:
    std::jthread thread_;
    IoContext ioContext_;
    bool started_ = false;
};
