#include "TcpAcceptor.h"

#include "internal/IoContextData.h"
#include "internal/TcpSocketData.h"

#include <asio.hpp>
#include <iostream>

struct TcpAcceptor::data{
    data(IoContext& ioContext, std::string const& ip, unsigned short port)
        : acceptor_{ioContext.data_->context_, asio::ip::tcp::endpoint{asio::ip::address::from_string(ip), port}}
    {
        acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    }

    asio::ip::tcp::acceptor acceptor_;
};

TcpAcceptor::TcpAcceptor(IoContext& ioContext, IoContextThreadPool& ioContextProvider, std::string const& ip, unsigned short port)
    : data_(new data(ioContext, ip, port))
    , ioContext_(ioContext)
    , ioContextProvider_(ioContextProvider)
    , ip_(ip)
    , port_(port)
{

}

TcpAcceptor::~TcpAcceptor(){
    close();

    delete data_;
    data_ = nullptr;
}

void TcpAcceptor::close(){
    data_->acceptor_.close();
}

void TcpAcceptor::startAccept(SocketEstablishHandler&& handler){
    handler_ = std::move(handler);

    doAccept();
}

void TcpAcceptor::doAccept(){
    if(!data_->acceptor_.is_open())
        return;

    auto socket = std::make_shared<TcpSocket>(ioContextProvider_.pickIoContext());
    data_->acceptor_.async_accept(socket->data_->socket_, [this, socket](std::error_code const& error){
        if(error){
            std::cout << "TcpAcceptor::startAccept() error: " << error.message() << "\n";
            doAccept();
            return;
        }

        ioContext_.post([this, socket]{
            handler_(std::move(*socket));
        });

        std::cout << "TcpAcceptor::startAccept() accepted\n";
        doAccept();
    });
}
