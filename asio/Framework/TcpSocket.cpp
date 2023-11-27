#include "internal/TcpSocketData.h"
#include "internal/IoContextData.h"

TcpSocket::TcpSocket(IoContext& ioContext)
    : data_(new data{asio::ip::tcp::socket{ioContext.data_->context_}})
{
    
}

TcpSocket::TcpSocket(TcpSocket&& rhs)
    : data_(rhs.data_)
{
    rhs.data_ = nullptr;
}

TcpSocket::~TcpSocket()
{
    delete data_;
    data_ = nullptr;
}
