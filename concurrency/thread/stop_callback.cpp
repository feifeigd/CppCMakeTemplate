
#include "writer.h"
#include <condition_variable>
#include <mutex>
#include <thread>

using namespace std;


int main(){
    {
        // A worker thread.
        // It will wait until it is requested to stop.
        jthread worker([](stop_token stoken){
            {Writer{} << "Worker thread's id: " << this_thread::get_id() << "\n";}
            mutex m;
            unique_lock lock{m};
            condition_variable_any().wait(lock, stoken, [&stoken]{return stoken.stop_requested();});
            Writer{} << "Worker wakeup thread's id: " << this_thread::get_id() << "\n";
        });

        // Register a stop callback on the worker thread.
        stop_callback callback(worker.get_stop_token(), []{
            Writer{} << "Stop callback executed by thread: " << this_thread::get_id() << "\n";  // 这个并不在主线程执行
        });

        // stop_callback objects can be destroyed prematurely to prevent execution.
        {
            stop_callback scoped_callback(worker.get_stop_token(), []{
                Writer{} << "Scoped stop callback executed by thread: " << this_thread::get_id() << "\n";
            }); // request_stop 还没调用，scoped_callback 就析构了，所以回调不会执行
        }

        // Demonstrate which thread executes the stop_callback and when.
        // Define a stopper function.
        auto stopper_func = [&worker]{
            this_thread::sleep_for(2s);
            if(worker.request_stop())
                Writer{} << "Stop request executed by thread: " << this_thread::get_id() << "\n";
            else
                Writer{} << "Stop request not executed by thread: " << this_thread::get_id() << "\n";
        };

        // Let multiple threads complete for stopping the worker thread.
        jthread stopper1(stopper_func);
        // jthread stopper2(stopper_func);
        stopper1.join();
        // stopper2.join();

        // After a stop has already been requested, a new stop_callack executes immediately.
        {Writer{} << "Main thread: " << this_thread::get_id() << "\n";}
        stop_callback callback_after_stop(worker.get_stop_token(), []{
            Writer{} << "Stop callback executed by thread: " << this_thread::get_id() << "\n";
        });
        worker.join();
    }

    {
        _CrtDumpMemoryLeaks();
    }

    return 0;
}
