#pragma once

#include "IoContextThreadPool.h"

#include <functional>

// 包装了信号处理的类
// 一个进程只有一个Application对象
class Application{
public:
	using Callback = std::function<void()>;

	Application(IoContext& ioContext);
	~Application();

	void run();
	void stop();

	void setStopCallback(Callback&& callback);
	void setRunCallback(Callback&& callback);

private:
	struct data;
	data* data_;

	IoContext& ioContext_;	// 主线程

	Callback runCallback;
	Callback stopCallback;
	IoContextThreadPool threadPool_;
};
