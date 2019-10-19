#include "Include.h"
#include "StaticMeshConverter.h"

StaticMeshConverter::StaticMeshConverter()
{
}


StaticMeshConverter::~StaticMeshConverter()
{
}

void StaticMeshConverter::ReadMesh(const aiScene * scene, StaticMesh & staticMesh)
{
	staticMesh.subsets.resize(scene->mNumMeshes);
	staticMesh.materials.resize(scene->mNumMaterials);

	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		const aiMesh* mesh = scene->mMeshes[i];
		aiVector3D zeroVector(0.0f, 0.0f, 0.0f);


		// 버덱스
		for (unsigned int vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex)
		{
			StaticMeshVertex vertex;

			vertex.position = mesh->mVertices[vertexIndex];
			vertex.normal = mesh->mNormals[vertexIndex];

			if (mesh->HasTextureCoords(0))
			{
				vertex.uv.x = mesh->mTextureCoords[0][vertexIndex].x;
				vertex.uv.y = mesh->mTextureCoords[0][vertexIndex].y;
			}

			staticMesh.vertices.push_back(vertex);
		}


		// 인덱스

		for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
		{
			const aiFace& face = mesh->mFaces[faceIndex];
			for (unsigned int j = 0; j < face.mNumIndices; ++j)
				staticMesh.indices.push_back(face.mIndices[j]);
		}

		// 메테리얼
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		Material mat;

		aiColor3D ambientColor;
		material->Get(AI_MATKEY_COLOR_DIFFUSE, ambientColor);
		mat.ambient.r = ambientColor.r;
		mat.ambient.g = ambientColor.g;
		mat.ambient.b = ambientColor.b;
		material->Get(AI_MATKEY_OPACITY, mat.ambient.a);

		aiColor3D diffuseColor;
		material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
		mat.diffuse.r = diffuseColor.r;
		mat.diffuse.g = diffuseColor.g;
		mat.diffuse.b = diffuseColor.b;
		material->Get(AI_MATKEY_OPACITY, mat.diffuse.a);

		aiColor3D specularColor;
		material->Get(AI_MATKEY_COLOR_DIFFUSE, specularColor);
		mat.specular.r = specularColor.r;
		mat.specular.g = specularColor.g;
		mat.specular.b = specularColor.b;
		material->Get(AI_MATKEY_OPACITY, mat.specular.a);

		aiString name;
		material->GetTexture(aiTextureType_DIFFUSE, 0, &name);
		mat.textureMaps[TextureType::Diffuse] = name.C_Str();

		staticMesh.materials[i] = mat;

		//버덱스
		staticMesh.subsets[i].vertexStart = 0;
		staticMesh.subsets[i].vertexCount = mesh->mNumVertices;

		if (i > 0)
			staticMesh.subsets[i].vertexStart = staticMesh.subsets[i - 1].vertexStart + staticMesh.subsets[i - 1].vertexCount;
		//인덱스	
		staticMesh.subsets[i].faceStart = 0;
		staticMesh.subsets[i].faceCount = mesh->mNumFaces;

		if (i > 0)
			staticMesh.subsets[i].faceStart = staticMesh.subsets[i - 1].faceStart + staticMesh.subsets[i - 1].faceCount;
	}
}

void StaticMeshConverter::WriteStaticMesh(const wstring & filename, const StaticMesh & mesh, string out_path)
{
	// 1. 서브메쉬 개수 (4바이트) 
	// 2. 전체 정점 개수 
	// 3. 전체 정점 기록
	//
	wstring wStr;
	wStr.assign(out_path.begin(), out_path.end());

	ofstream fout(wStr + L"\\" + filename, ofstream::binary);

	// 1. 서브메시 갯수
	int subsetCount = mesh.subsets.size();
	fout.write((char*)&subsetCount, sizeof(int));
	fout.write((char*)mesh.subsets.data(), sizeof(StaticMesh::Subset) * subsetCount);

	//메테리얼
	UINT mtlCount = mesh.materials.size();
	fout.write((char*)&mtlCount, sizeof(UINT));
	for (auto & mtl : mesh.materials)
	{
		fout.write((char*)&mtl.ambient, sizeof(aiColor4D));
		fout.write((char*)&mtl.diffuse, sizeof(aiColor4D));
		fout.write((char*)&mtl.specular, sizeof(aiColor4D));

		UINT texCount = mtl.textureMaps.size();
		fout.write((char*)&texCount, sizeof(UINT));
		for (auto & tex : mtl.textureMaps)
		{
			fout.write((char*)&tex.first, sizeof(TextureType));
			UINT length = tex.second.length();
			fout.write((char*)&length, sizeof(UINT));
			fout.write((char*)tex.second.data(), sizeof(char)*length);
		}
	}

	//버덱스 갯수
	int  vertexCount = mesh.vertices.size();
	fout.write((char*)&vertexCount, sizeof(int));
	fout.write((char*)mesh.vertices.data(), sizeof(StaticMeshVertex) * vertexCount);


	// 인덱스 갯수
	int indexCount = mesh.indices.size();
	fout.write((char*)&indexCount, sizeof(int));
	fout.write((char*)mesh.indices.data(), sizeof(UINT) * indexCount);
}

void StaticMeshConverter::FileSearch(string file_path, string searchFile)
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
		else if (check == 1 && fd.size != 0 && fd.name[0] != '.' && file_pt.find(searchFile) != string::npos)
		{
			cout << "파일명 : " << file_pt.c_str() << ", 크기 : " << fd.size << endl;
			_path = file_pt;
		}
	}
	_findclose(handle);

}

void StaticMeshConverter::CopyTextures(const vector<Material>& materials, string out_path)
{
	for (const auto& material : materials)
	{
		auto found = material.textureMaps.find(TextureType::Diffuse);
		if (found != material.textureMaps.cend())
		{
			ifstream in(fullPath + L"\\config2.txt", ifstream::binary);
			if (in.fail())
				exit(0);

			string line;
			getline(in, line);

			string path = line.substr(line.find(":") + 2);

			string searchfile = found->second;
			FileSearch(path, searchfile);

			size_t size = fd.size;
			ifstream fin(_path, ifstream::binary);

			vector<char> buffer(size);
			fin.read(buffer.data(), size);

			string outFile;
			outFile.assign(fullPath.begin(), fullPath.end());

			ofstream fout(out_path + "\\" + found->second, ofstream::binary);
			fout.write(buffer.data(), size);
		}
	}
}

int StaticMeshConverter::isFileOrDir()
{
	if (fd.attrib & _A_SUBDIR) //Dir
		return 0;
	else // File
		return 1;
}

void StaticMeshConverter::Run(char * in, char * out)
{
	string inFile_path = in;
	string outFile_path = out;

	WCHAR root[MAX_PATH];
	GetModuleFileNameW(NULL, root, MAX_PATH);

	fullPath = root;
	fullPath = fullPath.substr(0, fullPath.rfind('\\'));

	string xFile;
	xFile.assign(fullPath.begin(), fullPath.end());

	vector<string> files;
	string path = inFile_path + "\\*.x*";

	intptr_t handle;
	if ((handle = _findfirst(path.c_str(), &fd)) == -1L)
	{
		cout << "No file in directory!" << endl;
	}
	do
	{
		files.push_back(fd.name);

	} while (_findnext(handle, &fd) == 0);

	_findclose(handle);

	for (int i = 0; i < files.size(); i++)
	{
		cout << files[i] << endl;
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(inFile_path + "\\" + files[i],
			aiProcess_JoinIdenticalVertices | aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded);

		if (scene != nullptr)
		{
			StaticMesh stm;
			ReadMesh(scene, stm);

			// 폴더 뒤지면서 텍스처 파일 복사해오기

			CopyTextures(stm.materials, outFile_path);
			string stmfile_a = files[i].substr(0, files[i].size() - 2);
			wstring stmfile_w;
			stmfile_w.assign(stmfile_a.begin(), stmfile_a.end());
			WriteStaticMesh(stmfile_w + L".sm", stm, outFile_path);
		}
	}

	return;
}
