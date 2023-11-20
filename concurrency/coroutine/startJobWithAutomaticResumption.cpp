
#include <coroutine>
#include <functional>
#include <iostream>
#include <random>

using namespace std;

std::random_device seed;
auto gen = bind_front(uniform_int_distribution<>(0, 1), default_random_engine(seed()));

struct MySuspendAlways{
    bool await_ready()const noexcept{
        cout << "\tMySuspendAlways::await_ready\n";
        return !!gen(); // 随机 true/false
    }
    bool await_suspend(coroutine_handle<> handle)const noexcept{
        cout << "\tMySuspendAlways::await_suspend\n";
        handle.resume();    // 自动恢复
        return true;
    }
    void await_resume()const noexcept{
        cout << "\tMySuspendAlways::await_resume\n";
    }
};

struct MySuspendNever{
    bool await_ready()const noexcept{
        cout << "\tMySuspendNever::await_ready\n";
        return true;
    }
    void await_suspend(coroutine_handle<>)const noexcept{
        // 这里是不可能调用的
        cout << "\tMySuspendNever::await_suspend\n";
    }
    void await_resume()const noexcept{
        cout << "\tMySuspendNever::await_resume\n";
    }
};

struct Job{
    struct promise_type;
    using handle_type = coroutine_handle<promise_type>;
    handle_type coro;

    Job(handle_type h): coro(h){}
    ~Job(){
        cout << boolalpha << coro.done();
        if(coro)
            coro.destroy();
    }

    struct promise_type{
        Job get_return_object(){
            return handle_type::from_promise(*this);
        }

        // 要自己恢复
        MySuspendAlways initial_suspend(){
            cout << "\tJob Prepared  " << "\n";
            return {};
        }

        suspend_always final_suspend()noexcept{
            cout << "\tJob finished " << "\n";
            return {};
        }

        void unhandled_exception(){

        }

        void return_void(){}
    };

    void start(){
        // coro.resume();
    }
};

Job prepareJob(){
    co_await MySuspendNever();
}

int main(){
    {
        cout << "Before job " << "\n";
        auto job = prepareJob();
        prepareJob();
        prepareJob();
        prepareJob();
        cout << "After job " << endl;
    }
    {
        _CrtDumpMemoryLeaks();
    }
    return 0;
}
