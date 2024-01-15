#pragma once

#include "../../../Functor.hpp"
#include "../../../TcpSession.h"

#include <asio.hpp>	// TODO 去掉

struct SocketConnectionOption final {
	asio::ip::tcp::endpoint endpoint;	// TODO 去掉
	std::chrono::nanoseconds timeout = std::chrono::seconds(10);
	SocketFailedConnectHandler failedHandler;
	std::vector<SocketProcessingHandler> socketProcessingHandler;
};

struct TcpSessionOption final {
	std::vector<TcpSessionEstablishHandler> establishHandlers;	// socket 连接上，调用recv之前的回调
	TcpSession::DataHandler dataHandler;
	TcpSession::ClosedHandler closeHandler;
	TcpSession::EofHandler eofHandler;
};
