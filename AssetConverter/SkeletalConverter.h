#pragma once
class SkeletalConverter
{
public:
	SkeletalConverter();
	~SkeletalConverter();
	
private:
	Skeleton sk;
	struct _finddata_t fd;

private:
	void ReadSkeleton(const aiNode* node, int index, int parent);
	int isFileOrDir();

public:
	void Run(char* in, char* out);
};

