#include "udp_server.h"
#include <iostream>
#include <memory>

using namespace std;

static string make_daytime_string()
{
    time_t now = time(nullptr);
    return ctime(&now);
}

udp_server::udp_server(asio::io_context& ioContext)
    : ioContext_{ioContext}
    , socket_{ioContext, asio::ip::udp::endpoint(asio::ip::udp::v4(), 13)}
{
    startReceive();
}

void udp_server::startReceive()
{
    socket_.async_receive_from(asio::buffer(recvBuffer_), remoteEndpoint_,
        [=](std::error_code const& ec, std::size_t bytesTransferred)
        {
            if(!ec){
                startSend(bytesTransferred);
            }
            startReceive();
        });
}

void udp_server::startSend(std::size_t bytesTransferred)
{
    auto sendBuffer = make_shared<string>(make_daytime_string());
    socket_.async_send_to(asio::buffer(*sendBuffer, bytesTransferred), remoteEndpoint_,
        [=](std::error_code const& ec, std::size_t /*bytesTransferred*/)
        {
            if(!ec){
                cout << "send: " << *sendBuffer << endl;
            }
        });
}
