
#include <Application.h>
#include <iostream>

using namespace std;

int main(){

    try{
        Application app;
        app.setRunCallback([](){
            cout << "run" << endl;
        });
        app.setStopCallback([](){
            cout << "stop" << endl;
        });
        app.run();

    }catch(std::exception const& e){
        cerr << e.what() << endl;
    }catch(...){
        cerr << "unknown exception" << endl;
    }

    _CrtDumpMemoryLeaks();

    return 0;
}
