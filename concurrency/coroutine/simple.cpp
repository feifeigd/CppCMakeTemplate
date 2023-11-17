
#include "channel.hpp"
#include <iostream>
using namespace std;

lazy<void> recv1(channel<int>& chan){
    cout << "recv1: begin\n";
    while(true){
        auto&& [a, ok] = co_await chan.recv();
        if(!ok)
            break;
        cout << "recv1: " << a << "\n";
    }
    cout << "recv1: end\n";
    co_return;
}

lazy<void> recv2(channel<int>& chan){
    cout << "recv2: begin\n";
    while(true){
        auto&& [a, ok] = co_await chan.recv();
        if(!ok)
            break;
        cout << "recv2: " << a << "\n";
    }
    cout << "recv2: end\n";
    co_return;
}

lazy<void> send(channel<int>& chan){
    cout << "send: begin\n";

    cout << "send: 0\n";
    co_await chan.send(0);

    cout << "send: 1\n";
    co_await chan.send(1);

    cout << "send: 2\n";
    co_await chan.send(2);

    cout << "send: 3\n";
    co_await chan.send(3);

    cout << "send: close\n";
    chan.close();

    cout << "send: end\n";
}

void single_chan(){
    auto chan = channel<int>{};
    auto rc1 = recv1(chan);
    auto rc2 = recv2(chan);
    auto sc = send(chan);

    cout << __LINE__ << ": rc1::sync_await\n";
    rc1.sync_await();

    cout << __LINE__ << ": rc2::sync_await\n";
    rc2.sync_await();
    
    cout << __LINE__ << ": sc::sync_await\n";
    sc.sync_await();

    cout << __LINE__ << ": sync_await\n";
    chan.sync_await();
    
    cout << "auto coro handle destroy with lazy promise_type\n";

    cout << endl;
}

int main(){
    {
        _Lazy_promise_base<void> a;
        // _Lazy_promise_base<int> b;

        single_chan();
        cout << "=====================\n";
    }
    {
        _CrtDumpMemoryLeaks();
    }
    return 0;
}
