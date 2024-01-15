#pragma once

#include "IoContext.h"

class TcpSocket: private noncopyable
{
public:
    TcpSocket(IoContext& ioContext);
    ~TcpSocket();

    TcpSocket(TcpSocket&& rhs)noexcept;

    void dispatch(std::function<void()>&& task);

private:
    friend class TcpAcceptor;
    friend class TcpSession;

    struct data;
    data* data_;
};
