#pragma once

#include <asio.hpp>
#include <array>

class udp_server{
public:
    udp_server(asio::io_context& ioContext);

private:
    void startReceive();
    void startSend(std::size_t bytesTransferred);

    asio::io_context& ioContext_;
    asio::ip::udp::socket socket_;
    asio::ip::udp::endpoint remoteEndpoint_;
    std::array<char, 1> recvBuffer_;
};
