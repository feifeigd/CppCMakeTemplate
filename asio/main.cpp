
#include <Application.h>
#include <iostream>

using namespace std;

int main(){

    try{
        asio::io_context ioContext;
        Application app(ioContext);
        app.setRunCallback([](){
            cout << "run" << endl;
        });
        app.setStopCallback([](){
            cout << "stop" << endl;
        });
        app.run();
        ioContext.run();

    }catch(std::exception const& e){
        cerr << e.what() << endl;
    }catch(...){
        cerr << "unknown exception" << endl;
    }

    _CrtDumpMemoryLeaks();

    return 0;
}
