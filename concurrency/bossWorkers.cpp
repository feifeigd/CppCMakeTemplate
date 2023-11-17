

#include <iostream>
#include <latch>
#include <mutex>
#include <string>
using namespace std;

latch workDone(6);
latch goHome(1);

void synchronizedOut(string const& s){
    static mutex m;
    lock_guard<mutex> lock(m);
    cout << s << endl;
}

class Worker{
    string name;
public:
    Worker(string const& n ): name(n){}

    void operator()(){
        // notify the boss when work is done.
        synchronizedOut(name + ": " + "Work done!\n");
        workDone.count_down();
        // waiting before going home
        goHome.wait();
        synchronizedOut(name + ": " + "Good bye!\n");
    }
};

int main(){
    cout << "\nBOSS: START WORKIGN!\n";

    Worker herb("\tHerb");
    thread herbWork(herb);

    Worker scott("\tScott");
    thread scottWork(scott);

    Worker bjarne("\tBjarne");
    thread bjarneWork(bjarne);

    Worker andrei("\tAndrei");
    thread andreiWork(andrei);

    Worker andrew("\tAndrew");
    thread andrewWork(andrew);

    Worker david("\tDavid");
    thread davidWork(david);


    workDone.wait();
    cout << "\n";
    goHome.count_down();
    cout << "BOSS: GO HOME!\n";

    herbWork.join();
    scottWork.join();
    bjarneWork.join();
    andreiWork.join();
    davidWork.join();
    andrewWork.join();

    return 0;
}
