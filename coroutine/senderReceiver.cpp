
#include <chrono>
#include <coroutine>
#include <iostream>
#include <thread>

using namespace std;

class Event{
public:
    Event() = default;

    Event(Event const&) = delete;
    Event(Event &&) = delete;
    Event& operator=(Event const&) = delete;
    Event& operator=(Event &&) = delete;

    class Awaiter;
    Awaiter operator co_await() const noexcept;
    void notify()noexcept;

private:
    friend class Awaiter;
    mutable atomic<Awaiter*> suspendedWaiter;
    mutable atomic<bool> notified;
};

class Event::Awaiter{
public:
    Awaiter(Event const& e): event(e){}

    bool await_ready()const;
    bool await_suspend(coroutine_handle<> handle)noexcept;
    void await_resume(){}
private:
    friend class Event;

    Event const& event;
    coroutine_handle<> coroutineHandle;
};

Event::Awaiter Event::operator co_await() const noexcept{
    return *this;
}

void Event::notify()noexcept{
    notified = true;
    // try to load the waiter
    auto waiter = suspendedWaiter.load();
    if(waiter){
        // resume the coroutine => awiat_resume
        waiter->coroutineHandle.resume();
    }
}

bool Event::Awaiter::await_ready()const{
    if(event.suspendedWaiter.load()){
        throw runtime_error("More than one waiter is not valid");
    }
    // false: suspends the coroutine
    // true: the coroutine is executed like a normal function
    return event.notified;
}

bool Event::Awaiter::await_suspend(coroutine_handle<> handle)noexcept{
    coroutineHandle = handle;
    if(event.notified)
        return false; // 不用挂起
    // store the waiter for later notification
    event.suspendedWaiter.store(this);
    return true;
}

struct Task{
    struct promise_type{
        suspend_never initial_suspend(){return {};}
        suspend_never final_suspend()noexcept{return {};}
        void unhandled_exception(){}
        Task get_return_object(){return {};}
        void return_void(){}
    };
};

Task receiver(Event& event){
    auto start = chrono::high_resolution_clock::now();
    co_await event;
    cout << "Got the notification! \n";
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elaspsed = end - start;
    cout << "Waited " << elaspsed.count() << " seconds." << endl;
}

int main(){
    cout << "\nNotification before waiting\n";
    Event event1;
    auto senderThread1 = thread([&event1]{event1.notify();});
    auto receiverThread1 = thread(receiver, ref(event1));

    senderThread1.join();
    receiverThread1.join();

    cout << "\n";
    
    cout << "\nNotification after 2 seconds waiting\n";
    Event event2;
    auto receiverThread2 = thread(receiver, ref(event2));
    auto senderThread2 = thread([&event2]{
        this_thread::sleep_for(2s);
        event2.notify();
    });

    senderThread2.join();
    receiverThread2.join();

    cout << "\n";

    return 0;
}
