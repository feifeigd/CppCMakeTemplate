#pragma once

#include "../IoContext.h"

#include <asio.hpp>

struct IoContext::data{
    data()
        : work_(asio::make_work_guard(context_))
    {
    }
    
    asio::io_context context_;
    asio::executor_work_guard<asio::io_context::executor_type> work_;   // 没工作，也不退出 run()
};
