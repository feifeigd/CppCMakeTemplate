
#include <coroutine>
#include <iostream>
#include <memory>   // shared_ptr

using namespace std;

// resumable 
template<typename T>
struct MyFuture{
    shared_ptr<T> value;
    MyFuture(shared_ptr<T> p): value(p) {}

    T get(){
        return *value;
    }

    struct promise_type
    {
        shared_ptr<T> ptr = make_shared<T>();

        MyFuture<T> get_return_object(){
            return ptr;
        }

        suspend_never initial_suspend(){
            return {};
        }

        suspend_never final_suspend()noexcept{
            return {};
        }
        void unhandled_exception(){
            exit(1);
        }

        // 由 co_return v 调用
        void return_value(T v){
            *ptr = v;
        }
    };
    
};

MyFuture<int> createFuture(){
    co_return 2023;
}

int main(){
    cout << "\n";
    auto fut = createFuture();
    cout << "fut.get(): " << fut.get() << "\n";
    cout << endl;
    return 0;
}

