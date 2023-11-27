#pragma once

#include "noncopyable.hpp"
#include <functional>

class IoContext : public noncopyable
{
public:
    IoContext();
    ~IoContext();

    void run();
    void stop();
    bool stopped()const;

    void post(std::function<void()>&& task);

private:
    friend class Application;
    friend class TcpAcceptor;
    friend class TcpSocket;

    struct data;
    data* data_;    
};
