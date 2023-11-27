#pragma once

#include "noncopyable.hpp"

class IoContext : public noncopyable
{
public:
    IoContext();
    ~IoContext();

    void run();
    void stop();

    void post(std::function<void()>&& task);
    
private:
    friend class Application;
    friend class TcpAcceptor;

    struct data;
    data* data_;    
};
