#include <iostream>

#include <io.h>

#include <string>

using namespace std;



int main() {

	string path = "C:\\a\\b\\a.txt";
	struct _finddata_t fd;
	intptr_t handle;
	if ((handle = _findfirst(path.c_str(), &fd)) == -1L)
		cout << "No file in directory!" << endl;
	do
	{
		cout << fd.name << endl;
	} while (_findnext(handle, &fd) == 0);
	_findclose(handle);

}