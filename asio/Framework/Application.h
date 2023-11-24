#pragma once

#include <asio.hpp>
#include <functional>

// 包装了信号处理的类
// 一个进程只有一个Application对象
class Application{
public:
	using Callback = std::function<void()>;

	Application(asio::io_context& ioContext);
	~Application();

	void run();
	void stop();

	void post(Callback&& callback){
		asio::post(threadPool_, callback);
	}
	asio::thread_pool& pool() { return threadPool_; }

	void setStopCallback(Callback&& callback);
	void setRunCallback(Callback&& callback);

private:
	asio::thread_pool threadPool_;
	asio::io_context& ioContext_;
	asio::signal_set signals_;

	Callback runCallback;
	Callback stopCallback;

	void signalHandler(std::error_code const& error, int signal);
};
