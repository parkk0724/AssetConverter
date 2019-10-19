#pragma once
class SkeletalMeshConverter
{
public:
	SkeletalMeshConverter();
	~SkeletalMeshConverter();

private:
	vector<string> _texture_path;
	vector<string> _deduplication;
	struct _finddata_t fd;

private:
	void CopyTextures(IN string inFile_path, OUT string outFile_path);
	void CopyFileSearch(IN string inFile_path, IN string searchFile, OUT string outFile_path);
	void ReadSkeleton(const aiNode* node, int index, unordered_map<string, int>& skeleton);
	void ReadMesh(const aiScene* scene, const aiNode* node, const aiMatrix4x4& parent, const JointHashMap& joints, SkeletalMesh& skeletalMesh);
	void WriteSkeletalMesh(const wstring& filename, const SkeletalMesh& mesh);
	void FileSearch(IN string inFile_path, OUT string outFile_path);
	void DuplicationFileSearch(string File_path);
	int isFileOrDir();

public:
	void Run(char* in, char* out);
};
