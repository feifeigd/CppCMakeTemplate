#pragma once

#include "../TcpSocket.h"
#include <asio.hpp>

struct TcpSocket::data
{
    asio::ip::tcp::socket socket_;
};
