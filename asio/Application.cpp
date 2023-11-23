
#include "Application.h"
#include <csignal>

Application::Application(asio::io_context& ioContext)
    :ioContext_(ioContext), signals_(ioContext_, SIGINT, SIGTERM)
{
    signals_.async_wait(std::bind(&Application::signalHandler, this, std::placeholders::_1, std::placeholders::_2));
}

Application::~Application(){
}

void Application::run(){
    if(runCallback)runCallback();
}


void Application::stop(){
    if(stopCallback)stopCallback();
    ioContext_.stop();
}

void Application::setRunCallback(Callback&& callback){
    runCallback = callback;
}

void Application::setStopCallback(Callback&& callback){
    stopCallback = callback;
}

void Application::signalHandler(asio::error_code const& error, int signal){
    if(error){
        stop();
        return;
    }

    switch (signal)
    {
    case SIGINT:
    case SIGTERM:
        stop();
        break;
    
    default:
        break;
    }
}
