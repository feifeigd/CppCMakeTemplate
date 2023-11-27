#pragma once

#include "IoContextThread.h"
#include <vector>

class IoContextThreadPool: private noncopyable
{
public:
    IoContextThreadPool();

    void start(std::size_t threadNum = 1);
    void stop();

    IoContext& pickIoContext();

private:
    std::vector<std::unique_ptr< IoContextThread>> threads_;
};
