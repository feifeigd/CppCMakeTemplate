#include "tcp_connection.h"

#include <iostream>
#include <ctime>

using namespace std;

tcp_connection::~tcp_connection()
{
    cout << "tcp_connection::~tcp_connection()" << endl;
}

std::string make_daytime_string()
{
    time_t now = time(nullptr);
    return ctime(&now);
}

tcp_connection::tcp_connection(asio::io_context &ioContext)
    : socket_(ioContext)
{
}

tcp_connection::pointer tcp_connection::create(asio::io_context &ioContext)
{
    struct make_shared_enabler : public tcp_connection
    {
        make_shared_enabler(asio::io_context& ioContext)
            : tcp_connection(ioContext)
        {
        }
    };

    // 因为现在 make_shared 不能调用 tcp_connection 构造函数，又不想直接用 new，所以搞了这个子类 make_shared_enabler
    return make_shared<make_shared_enabler>(ioContext);
}

void tcp_connection::start()
{
    message_ = make_daytime_string();
    asio::async_write(socket_, asio::buffer(message_),
        [this, self = shared_from_this()](error_code const& ec, size_t /*length*/)
        {
            if(!ec){
                cout << "thread_id: " << this_thread::get_id() << "\n";
                cout << "send: " << message_ << endl;
            }
        });
}
