#pragma once

#include "IoContextThreadPool.h"
#include "Functor.hpp"

#include <string>

class TcpAcceptor final
	: private noncopyable
{
public:
	TcpAcceptor(IoContext& ioContext, IoContextThreadPool& ioContextProvider,  std::string const& ip, unsigned short port);
	~TcpAcceptor();

	void startAccept(SocketEstablishHandler&& handler);
	void close();

private:
	void doAccept();

	struct data;
	data* data_;

	IoContext& ioContext_;
	IoContextThreadPool& ioContextProvider_;	// 分配给新连接的ioContext

	std::string ip_;
	unsigned short port_; 
	SocketEstablishHandler handler_;
};
