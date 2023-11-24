
#include "Application.h"
#include <writer.h>
#include <csignal>  // SIGINT, SIGTERM
#include <iostream>

using namespace std;

Application::Application(asio::io_context& ioContext)
    :ioContext_(ioContext), signals_(ioContext_, SIGINT, SIGTERM)
{
    signals_.async_wait(std::bind(&Application::signalHandler, this, std::placeholders::_1, std::placeholders::_2));
}

Application::~Application(){
    Writer{} << "Application::~Application()\n";
}

void Application::run(){
    if(runCallback)runCallback();
    Writer{} << "Application thread_id: " << this_thread::get_id() << "\n";
    for (size_t i = 0; i < thread::hardware_concurrency(); i++)
    {
        post([this]{ioContext_.run(); });
    }
    ioContext_.run();
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

void Application::signalHandler(std::error_code const& error, int signal){
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
