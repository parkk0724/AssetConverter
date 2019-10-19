#pragma once
class StaticMeshConverter
{
public:
	StaticMeshConverter();
	~StaticMeshConverter();
	
private:
	wstring fullPath;
	string _path;
	struct _finddata_t fd;

private:
	void ReadMesh(const aiScene* scene, StaticMesh& staticMesh);
	void WriteStaticMesh(const wstring& filename, const StaticMesh& mesh, string out_path);
	void FileSearch(string file_path, string searchFile);
	void CopyTextures(const vector<Material>& materials, string out_path);
	int isFileOrDir();

public:
	void Run(char* in, char* out);
};

