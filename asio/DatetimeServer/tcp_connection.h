#pragma once

#include <asio.hpp>
#include <memory>   // enable_shared_from_this

class tcp_connection
    : public std::enable_shared_from_this<tcp_connection>
{
public:
    using pointer = std::shared_ptr<tcp_connection>;
    ~tcp_connection();
    
    static pointer create(asio::io_context& ioContext);

    asio::ip::tcp::socket& socket(){ return socket_; }

    void start();
private:
    tcp_connection(asio::io_context& ioContext);
    asio::ip::tcp::socket socket_;
    std::string message_;
};
