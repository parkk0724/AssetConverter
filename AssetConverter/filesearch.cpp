#pragma warning (disable : 4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <Windows.h>
#include <iostream>
using namespace std;

struct _finddata_t fd;

int isFileOrDir()
{
	if (fd.attrib & _A_SUBDIR) //Dir
		return 0;
	else // File
		return 1;
}

void FileSearch(string file_path, string searchFile)
{
	intptr_t handle;
	int check = 0;
	string file_path2;

	file_path += "\\";
	file_path2 = file_path;
	file_path += "*";

	if ((handle = _findfirst(file_path.c_str(), &fd)) == -1)
	{
		cout << "No such file or directory" << endl;
		return;
	}

	while (_findnext(handle, &fd) == 0)
	{
		string file_pt;
		file_pt = file_path2;
		file_pt += fd.name;

		check = isFileOrDir();

		if (check == 0 && fd.name[0] != '.')
		{
			FileSearch(file_pt, searchFile);
		}
		else if (check == 1 && fd.size != 0 && fd.name[0] != '.')
		{
			cout << "파일명 : " << file_pt.c_str() << ", 크기 : " << fd.size << endl;
		}
	}
	_findclose(handle);
}

int main()
{
	string file_path = "C:";
	string searchfile = "a.txt";
	FileSearch(file_path, searchfile);

	return 0;
}