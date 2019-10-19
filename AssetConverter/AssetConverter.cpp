#include "Include.h"
#include "SkeletalConverter.h"
#include "SkeletalMeshConverter.h"
#include "StaticMeshConverter.h"

int main(int argc, char* argv[])
{ 
	if (argc != 4)
	{
		cout << "입력한 인자가 적어 프로그램을 실행할 수 없습니다." << endl;
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
		cout << "입력한 인자가 맞지 않습니다." << endl;
	}
	return 0;
}