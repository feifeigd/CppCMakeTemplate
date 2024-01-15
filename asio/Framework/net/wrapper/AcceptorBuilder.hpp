#pragma once

#include "internal/TcpSessionBuilder.hpp"

template<typename Derived>
class BaseTcpSessionAcceptorBuilder {
public:
	using SessionOptionBuilderCallback = std::function<void(SessionOptionBuilder&)>;

	BaseTcpSessionAcceptorBuilder(TcpAcceptor& acceptor)
		: acceptor_(acceptor)
	{
		return;
	}

	~BaseTcpSessionAcceptorBuilder() {
		return;
	}

	Derived& setSessionOptionBuilderCallback(SessionOptionBuilderCallback callback) {
		sessionOptionBuilderCallback_ = std::move(callback);
		return static_cast<Derived&>(*this);
	}

	Derived& setReceiveBufferSize(size_t size) {
		receiveBufferSize_ = size;
		return static_cast<Derived&>(*this);
	}

	void start() {
		auto establishHandler = [builderCallback = sessionOptionBuilderCallback_, receiveBufferSize = receiveBufferSize_](TcpSocket socket) {
			SessionOptionBuilder builder;
			if (builderCallback) {
				builderCallback(builder);
			}
			auto& option = builder.option();

			auto session = TcpSession::Make(std::move(socket), receiveBufferSize,
				std::move(option.dataHandler), std::move(option.closeHandler), std::move(option.eofHandler));

			for (const auto& handler : option.establishHandlers) {
				handler(session);
			}

			session->start();
			return;
		};

		acceptor_.startAccept([handlers = std::move(socketProcessingHandlers_), establishHandler = std::move(establishHandler)](TcpSocket socket) {
			for (const auto& handler: handlers)
			{
				handler(socket);
			}
			establishHandler(std::move(socket));
			return;
		});
	}

private:
	TcpAcceptor& acceptor_;
	std::vector<SocketProcessingHandler> socketProcessingHandlers_;
	SessionOptionBuilderCallback sessionOptionBuilderCallback_;
	size_t receiveBufferSize_ = 1024;
};

class TcpSessionAcceptorBuilder
	: public BaseTcpSessionAcceptorBuilder<TcpSessionAcceptorBuilder>
	, private noncopyable
{
public:
	using BaseTcpSessionAcceptorBuilder::BaseTcpSessionAcceptorBuilder;
};
