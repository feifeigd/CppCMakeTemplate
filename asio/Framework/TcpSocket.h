#pragma once

#include "IoContext.h"

class TcpSocket: private noncopyable
{
public:
    TcpSocket(IoContext& ioContext);
    ~TcpSocket();

    TcpSocket(TcpSocket&& rhs);
private:
    friend class TcpAcceptor;

    struct data;
    data* data_;
};
