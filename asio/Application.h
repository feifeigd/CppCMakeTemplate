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

    void setStopCallback(Callback&& callback);
    void setRunCallback(Callback&& callback);

private:
    asio::io_context& ioContext_;
    asio::signal_set signals_;

    Callback runCallback;
    Callback stopCallback;

    void signalHandler(asio::error_code const& error, int signal);
};
