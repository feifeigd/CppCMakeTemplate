
#include "Application.h"
#include "internal/IoContextData.h"

#include <writer.h>

#include <asio.hpp>

#include <csignal>  // SIGINT, SIGTERM
#include <iostream>

using namespace std;

struct Application::data{
    data(Application* app)
        :app_{app}
        , work_(asio::make_work_guard(app->ioContext_.data_->context_ ))
        , signals_(app->ioContext_.data_->context_, SIGINT, SIGTERM)
    {
        signals_.async_wait(std::bind(&data::signalHandler, this, std::placeholders::_1, std::placeholders::_2));
    }
    
    Application* app_;

	asio::signal_set signals_;
    asio::executor_work_guard<asio::io_context::executor_type> work_;
    

	void signalHandler(std::error_code const& error, int signal);
};

Application::Application(IoContext& ioContext)
    : ioContext_{ioContext}
{
    data_ = new data(this);
}

Application::~Application(){
    Writer{} << "Application::~Application()\n";
    delete data_;
    data_ = nullptr;
}


void Application::run(){
    if(runCallback)runCallback();
    Writer{} << "Application running thread_id: " << this_thread::get_id() << "\n";

    threadPool_.start(thread::hardware_concurrency());
    ioContext_.run();
    
    Writer{} << "Application stopped thread_id: " << this_thread::get_id() << "\n";
}


void Application::stop(){
    if(stopCallback)
        stopCallback();

    threadPool_.stop();
    ioContext_.stop();
}

void Application::setRunCallback(Callback&& callback){
    runCallback = callback;
}

void Application::setStopCallback(Callback&& callback){
    stopCallback = callback;
}

void Application::data::signalHandler(std::error_code const& error, int signal){
    if(error){
        app_->stop();
        return;
    }

    switch (signal)
    {
    case SIGINT:
    case SIGTERM:
        app_->stop();
        break;
    
    default:
        break;
    }
}
