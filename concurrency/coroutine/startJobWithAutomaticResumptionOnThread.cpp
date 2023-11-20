
#include <coroutine>
#include <functional>
#include <iostream>
#include <random>
#include <thread>

using namespace std;

std::random_device seed;
auto gen = bind_front(uniform_int_distribution<>(0, 1), default_random_engine(seed()));

struct MyAwaitable{
    jthread& outerThread;
    
    bool await_ready()const noexcept{
            cout << "\tMyAwaitable::await_ready\n";
        auto res = !!gen();
        if(res)       
            cout << "\t (executed)\n";     
        else
            cout << "\t (suspended)\n"; 

        return res; // 随机 true/false
    }
    void await_suspend(coroutine_handle<> handle)const noexcept{
        cout << "\tMySuspendAlways::await_suspend\n";
        outerThread = jthread([handle]{handle.resume(); }); // 创建线程
    }
    void await_resume()const noexcept{
        cout << "\tMySuspendAlways::await_resume\n";
    }
};

struct Job{
    static inline int JobCounter{1};

    struct promise_type;
    using handle_type = coroutine_handle<promise_type>;
    handle_type coro;

    Job(handle_type h): coro(h){
        ++JobCounter;
    }
    ~Job(){
        cout << boolalpha << coro.done();
        // if(coro)
        //     coro.destroy();
    }

    struct promise_type{
        int jobNumber{JobCounter};
        Job get_return_object(){
            return handle_type::from_promise(*this);
        }

        // 要自己恢复
        suspend_never initial_suspend(){
            cout << "\tJob " << jobNumber << " Prepared on thread " <<  this_thread::get_id() << "\n";
            return {};
        }

        suspend_never final_suspend()noexcept{
            cout << "\tJob " << jobNumber << " finished on thread " <<  this_thread::get_id() << "\n";
            return {};
        }

        void unhandled_exception(){

        }

        void return_void(){}
    };
};

Job performJob(jthread& out){
    co_await MyAwaitable{out};
}

int main(){
    {
        vector<jthread> threads(8);
        for(auto& thr: threads)
            performJob(thr);
    }
    {
        _CrtDumpMemoryLeaks();
    }
    return 0;
}
