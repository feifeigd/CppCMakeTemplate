
#include <chrono>
#include <iostream>
#include <cppcoro/sync_wait.hpp>    // sync_wait
#include <cppcoro/task.hpp>     // task
#include <vector>
#include <list>

using namespace std;

auto getTimeSince(chrono::time_point<chrono::high_resolution_clock> const& start){
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    return elapsed.count();
}

cppcoro::task<> third(chrono::time_point<chrono::high_resolution_clock> const& start){
    this_thread::sleep_for(1s);
    cout << "Third awaited " << getTimeSince(start) << " seconds.\n";
    co_return;
}

cppcoro::task<> second(chrono::time_point<chrono::high_resolution_clock> const& start){
    auto sec = third(start);
    this_thread::sleep_for(1s);
    co_await sec;
    cout << "Second awaited " << getTimeSince(start) << " seconds.\n";
}

cppcoro::task<> first(chrono::time_point<chrono::high_resolution_clock> const& start){
    auto sec = second(start);
    this_thread::sleep_for(1s);
    co_await sec;
    cout << "First awaited " << getTimeSince(start) << " seconds.\n";
}

struct Point2D{
    int x, y;
};

int main(){
    {
        Point2D point{.x=1, .y=2};
        auto foo = []<typename T>(vector<T> const& vec){

        };
        list<int> a;


        cout << "\n";
        auto start = chrono::high_resolution_clock::now();
        cppcoro::sync_wait(first(start));
        cout << "Main waited " << getTimeSince(start) << "seconds.\n";
        cout << endl;
    }

    {
        _CrtDumpMemoryLeaks();
    }

    return 0;
}
