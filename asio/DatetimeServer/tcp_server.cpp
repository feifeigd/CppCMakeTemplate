#include "tcp_server.h"
#include "tcp_connection.h"

tcp_server::tcp_server(asio::io_context& ioContext)
    : ioContext_{ioContext}
    , acceptor_(ioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 13))
    , socket_(ioContext)
{
    startAccept();
}

void tcp_server::startAccept()
{
    auto newConnection = tcp_connection::create(ioContext_);
    acceptor_.async_accept(newConnection->socket(),
        [=](std::error_code const& ec)
        {
            if(!ec){
                newConnection->start();
            }
            startAccept();
        });
}
