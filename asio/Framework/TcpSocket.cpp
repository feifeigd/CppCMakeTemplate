#include "internal/TcpSocketData.h"
#include "internal/IoContextData.h"
#include <iostream>

TcpSocket::TcpSocket(IoContext& ioContext)
    : data_(new data{asio::ip::tcp::socket{ioContext.data_->context_}})
{
    
}

TcpSocket::TcpSocket(TcpSocket&& rhs)noexcept
    : data_(rhs.data_)
{
    rhs.data_ = nullptr;
}

void TcpSocket::dispatch(std::function<void()>&& task)
{
    asio::dispatch(data_->socket_.get_executor(), std::move(task));
}


TcpSocket::~TcpSocket()
{
    std::cout << __FUNCTION__ << ": " << data_ << std::endl;
    if (data_)
	{
		delete data_;
		data_ = nullptr;
    }
}
