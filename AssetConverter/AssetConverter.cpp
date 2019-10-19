#include "Include.h"
#include "SkeletalConverter.h"
#include "SkeletalMeshConverter.h"
#include "StaticMeshConverter.h"

int main(int argc, char* argv[])
{ 
	if (argc != 4)
	{
		cout << "�Է��� ���ڰ� ���� ���α׷��� ������ �� �����ϴ�." << endl;
		return 0;
	}

	if (strcmp(argv[1], "-skt") == 0)
	{
		SkeletalConverter skt;
		skt.Run(argv[2], argv[3]);
	}
	else if ( strcmp(argv[1],"-skm") == 0)
	{
		SkeletalMeshConverter skm;
		skm.Run(argv[2], argv[3]);
	}
	else if (strcmp(argv[1], "-sm") == 0)
	{
		StaticMeshConverter sm;
		sm.Run(argv[2], argv[3]);
	}
	else
	{
		cout << "�Է��� ���ڰ� ���� �ʽ��ϴ�." << endl;
	}
	return 0;
}