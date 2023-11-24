#pragma once

#include <asio.hpp>

class tcp_server{
public:
    tcp_server(asio::io_context& ioContext);

private:
    void startAccept();

    asio::io_context& ioContext_;
    asio::ip::tcp::acceptor acceptor_;
    asio::ip::tcp::socket socket_;
};
