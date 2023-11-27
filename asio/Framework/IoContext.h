#pragma once

#include "noncopyable.hpp"

class IoContext : public noncopyable
{
public:
    IoContext();
    ~IoContext();

    void run();
    void stop();

private:
    struct data;
    data* data_;    
};
