#pragma once

#include "IoContextThreadPool.h"
#include "base/WaitGroup.h"

#include <functional>

// 包装了信号处理的类
// 一个进程只有一个Application对象
class Application{
public:
	using Callback = std::function<void()>;

	Application();
	Application(IoContext& ioContext);
	~Application();

	void run();
	void stop();

	void setStopCallback(Callback&& callback);
	void setRunCallback(Callback&& callback);

	auto& getIoContext(){
		return ioContext_;
	}
	auto& getThreadPool(){
		return threadPool_;
	}
private:
	struct data;
	data* data_;

	std::unique_ptr<IoContext> ioContextPtr_;	// 主线程, 放在 ioContext_ 之前
	IoContext& ioContext_;	// 主线程

	Callback runCallback;
	Callback stopCallback;
	IoContextThreadPool threadPool_;
	WaitGroup waitGroup_;
};
