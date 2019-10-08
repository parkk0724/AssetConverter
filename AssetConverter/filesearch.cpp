#pragma warning (disable : 4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <Windows.h>

struct _finddata_t fd;

int isFileOrDir()
{
	if (fd.attrib & _A_SUBDIR)
		return 0;
	else
		return 1;
}

void FileSearch(char file_path[], char searchFile[])
{
	intptr_t handle;
	int check = 0;
	char file_path2[_MAX_PATH];

	strcat(file_path, "\\");
	strcpy(file_path2, file_path);
	strcat(file_path, "*");

	if ((handle = _findfirst(file_path, &fd)) == -1)
	{
		printf("No such file or directory\n");
		return;
	}

	while (_findnext(handle, &fd) == 0)
	{
		char file_pt[_MAX_PATH];
		strcpy(file_pt, file_path2);
		strcat(file_pt, fd.name);

		check = isFileOrDir();

		if (check == 0 && fd.name[0] != '.')
		{
			FileSearch(file_pt, searchFile);
		}
		else if (check == 1 && fd.size != 0 && fd.name[0] != '.')
		{
			printf("파일명 : %s, 크기:%d\n", file_pt, fd.size);
		}
	}
	_findclose(handle);
}

int main()
{
	char file_path[_MAX_PATH] = "C:";
	char searchfile[_MAX_PATH] = "a.txt";
	FileSearch(file_path, searchfile);

	return 0;
}