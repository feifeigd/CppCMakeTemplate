#pragma once

#include "../../../noncopyable.hpp"
#include "Option.hpp"
#include "../../../TcpSession.h"

template<typename Derived>
class BaseSessionOptionBuilder {
public:
	// 新建一个网络链接
	Derived& AddEstablishHandler(TcpSessionEstablishHandler handler)noexcept {
		option_.establishHandlers.push_back( std::move(handler));
		return static_cast<Derived&>(*this);
	}

	Derived& SetDataHandler(TcpSession::DataHandler handler)noexcept {
		option_.dataHandler = std::move(handler);
		return static_cast<Derived&>(*this);
	}

	Derived& SetCloseHandler(TcpSession::ClosedHandler handler)noexcept {
		option_.closeHandler = std::move(handler);
		return static_cast<Derived&>(*this);
	}

	Derived& SetEofHandler(TcpSession::EofHandler handler)noexcept {
		option_.eofHandler = std::move(handler);
		return static_cast<Derived&>(*this);
	}

	[[nodiscard]]
	TcpSessionOption& option()noexcept {
		return option_;
	}
protected:
	void clear() {
		option_.establishHandlers.clear();
	}

private:
	TcpSessionOption option_;
};

class SessionOptionBuilder 
	: public BaseSessionOptionBuilder<SessionOptionBuilder> 
	, private noncopyable
{

};
