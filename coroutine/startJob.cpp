
#include <coroutine>

#include <iostream>

using namespace std;

struct Job{
    struct promise_type;
    using handle_type = coroutine_handle<promise_type>;
    handle_type coro;

    Job(handle_type h): coro(h){}
    ~Job(){
        if(coro && !coro.done())
            coro.destroy();
    }

    struct promise_type{
        Job get_return_object(){
            return handle_type::from_promise(*this);
        }

        // 要自己恢复
        suspend_always initial_suspend(){
            cout << "\tPreparing job " << "\n";
            return {};
        }

        suspend_never final_suspend()noexcept{
            cout << "\tPerforming job " << "\n";
            return {};
        }

        void unhandled_exception(){

        }

        void return_void(){}
    };

    void start(){
        coro.resume();
    }
};

Job prepareJob(){
    co_await suspend_never();
}

int main(){
    {
        cout << "Before job " << "\n";
        auto job = prepareJob();
        job.start();
        cout << "After job " << endl;
    }
    {
        _CrtDumpMemoryLeaks();
    }
    return 0;
}
