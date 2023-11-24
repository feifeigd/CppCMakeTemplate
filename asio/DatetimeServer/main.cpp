
#include "tcp_server.h"
#include "udp_server.h"
#include <Application.h>
#include <iostream>
#include <writer.h>

using namespace std;

void Print(std::error_code ec, asio::steady_timer* timer, int* count, asio::io_context::strand& strand)
{
	if (*count < 3)
	{
		for (size_t i = 0; i < 10; i++)
		{
			strand.post([c=*count,j=i](){
				Writer{} << c << ",j=" << j << " strand thread_id: " << this_thread::get_id();
			});
		}

		++(*count);
		timer->expires_at(timer->expires_at() + std::chrono::seconds(1));
		timer->async_wait(std::bind(&Print, std::placeholders::_1, timer, count, strand));
	}
}

int main(){

	try{

		asio::io_context ioContext;

		asio::io_context::strand strand(ioContext);

		ioContext.post([](){
			Writer{} << "post thread_id: " << this_thread::get_id();
		});

		Application app(ioContext);
		app.setRunCallback([]{
			Writer{} << "run";
		});
		app.setStopCallback([]{
			Writer{} << "stop";
		});

		app.post([]{
			Writer{} << "app post thread_id: " << this_thread::get_id();
		});

		tcp_server server1(ioContext);
		udp_server server2(ioContext);

		asio::steady_timer timer(ioContext, std::chrono::seconds(3));
		int count = 0;
		timer.async_wait(std::bind(&Print, std::placeholders::_1, &timer, &count, strand));

		app.run();
		
	}catch(std::exception const& e){
		cerr << e.what() << endl;
	}catch(...){
		cerr << "unknown exception" << endl;
	}

	_CrtDumpMemoryLeaks();
	
	return 0;
}
