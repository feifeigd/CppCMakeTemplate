#include <iostream>
#include <thread>
#include <vector>

using namespace std;

void func(stop_token stoken){
    atomic<int> counter{};
    auto thread_id = this_thread::get_id();
    stop_callback(stoken, [&, thread_id]{
        cout << "Thread id: " << thread_id 
            << "; counter: " << counter << "\n";
    });
    while(counter < 1000){
        this_thread::sleep_for(0.2s);
        ++counter;
    }
}

int main(){
    {
        vector<jthread> vecThread(10);
        for(auto& thr: vecThread)thr = jthread(func);
        this_thread::sleep_for(1s);
        for(auto& thr: vecThread)thr.request_stop();
        cout << endl;

    }
    {
        _CrtDumpMemoryLeaks();
    }

    return 0;
}
