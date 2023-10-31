
#ifdef WIN32
#include <direct.h>
#define _getcwd getcwd
#else
#include <unistd.h>
#endif // WIN32

#include <iostream>
using namespace std;

int main(int argc, char const* argv[]) {
	char buffer[256];
	
    cout << "Hello World! cwd=" << getcwd(buffer, sizeof buffer - 1) << endl;
	for (int i = 0; i < argc; i++)
	{
		cout << "i=" << i << ", arg=" << argv[i] << endl;
	}
    return 0;
}
