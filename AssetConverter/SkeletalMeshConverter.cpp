#include "Include.h"
#include "SkeletalMeshConverter.h"

SkeletalMeshConverter::SkeletalMeshConverter()
{
}


SkeletalMeshConverter::~SkeletalMeshConverter()
{
}

void SkeletalMeshConverter::CopyTextures(IN string inFile_path, OUT string outFile_path)
{
	size_t size = fd.size;
	ifstream fin(inFile_path, ifstream::binary);

	vector<char> buffer(size);
	fin.read(buffer.data(), size);

	ofstream fout(outFile_path, ofstream::binary);
	fout.write(buffer.data(), size);
}

void SkeletalMeshConverter::CopyFileSearch(IN string inFile_path, IN string searchFile, OUT string outFile_path)
{
	intptr_t handle;
	int check = 0;
	string file_path2;

	inFile_path += "\\";
	file_path2 = inFile_path;
	inFile_path += "*";

	if ((handle = _findfirst(inFile_path.c_str(), &fd)) == -1)
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
			CopyFileSearch(file_pt, searchFile, outFile_path);
		}
		else if (check == 1 && fd.size != 0 && fd.name[0] != '.' && file_pt.find(searchFile) != string::npos)
		{
			cout << "파일명 : " << file_pt.c_str() << ", 크기 : " << fd.size << endl;
			_deduplication.push_back(fd.name);
			CopyTextures(file_pt, outFile_path + "\\" + fd.name);
		}
	}
	_findclose(handle);
}

void SkeletalMeshConverter::ReadSkeleton(const aiNode * node, int index, unordered_map<string, int>& skeleton)
{
	if (node->mNumMeshes == 0)
		skeleton[node->mName.C_Str()] = index;

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
		ReadSkeleton(node->mChildren[i], skeleton.size(), skeleton);
}

void SkeletalMeshConverter::ReadMesh(const aiScene * scene, const aiNode * node, const aiMatrix4x4 & parent, const JointHashMap & joints, SkeletalMesh & skeletalMesh)
{
	aiMatrix4x4 transform = parent * node->mTransformation;

	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		// Subset
		SkeletalMesh::Subset subset;
		subset.vertexStart = (skeletalMesh.subsets.size() == 0 ? 0 : skeletalMesh.subsets.back().vertexStart + skeletalMesh.subsets.back().vertexCount);
		subset.vertexCount = mesh->mNumVertices;
		subset.faceStart = (skeletalMesh.subsets.size() == 0 ? 0 : skeletalMesh.subsets.back().faceStart + skeletalMesh.subsets.back().faceCount);
		subset.faceCount = mesh->mNumFaces;
		skeletalMesh.subsets.push_back(subset);

		//Meterial
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		Material mat;

		aiColor3D ambientColor;
		material->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor);
		mat.ambient.r = ambientColor.r;
		mat.ambient.g = ambientColor.g;
		mat.ambient.b = ambientColor.b;
		mat.ambient.a = 1.0f;

		aiColor3D diffuseColor;
		material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
		mat.diffuse.r = diffuseColor.r;
		mat.diffuse.g = diffuseColor.g;
		mat.diffuse.b = diffuseColor.b;
		material->Get(AI_MATKEY_OPACITY, mat.diffuse.a);

		aiColor3D specularColor;
		material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
		mat.specular.r = specularColor.r;
		mat.specular.g = specularColor.g;
		mat.specular.b = specularColor.b;
		material->Get(AI_MATKEY_SHININESS, mat.specular.a);

		aiString name;
		material->GetTexture(aiTextureType_DIFFUSE, 0, &name);
		mat.textureMaps[TextureType::Diffuse] = name.C_Str();

		skeletalMesh.materials.push_back(mat);

		// Vertex
		for (unsigned int vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex)
		{
			SkeletalMeshVertex vertex;

			vertex.position = mesh->mVertices[vertexIndex];
			vertex.normal = mesh->mNormals[vertexIndex];

			vertex.position = transform * vertex.position;
			vertex.normal = aiMatrix3x3(transform) * vertex.normal;


			if (mesh->HasTextureCoords(0))
			{
				vertex.uv.x = mesh->mTextureCoords[0][vertexIndex].x;
				vertex.uv.y = mesh->mTextureCoords[0][vertexIndex].y;
			}

			if (mesh->HasVertexColors(0))
			{
				vertex.color = mesh->mColors[0][vertexIndex];
			}
			else
			{
				vertex.color = aiColor4D(0, 0, 0, 0);
			}

			skeletalMesh.vertices.push_back(vertex);
		}

		// Index
		for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
		{
			const aiFace& face = mesh->mFaces[faceIndex];
			for (unsigned int j = 0; j < face.mNumIndices; ++j)
				skeletalMesh.indices.push_back(face.mIndices[j]);
		}

		// Joint
		for (unsigned int jointIndex = 0; jointIndex < mesh->mNumBones; ++jointIndex)
		{
			const aiBone* bone = mesh->mBones[jointIndex];

			for (unsigned int j = 0; j < bone->mNumWeights; ++j)
			{
				const aiVertexWeight& weight = bone->mWeights[j];

				const SkeletalMesh::Subset& subset = skeletalMesh.subsets[i];
				SkeletalMeshVertex& vertex = skeletalMesh.vertices[subset.vertexStart + weight.mVertexId];

				for (int k = 0; k < 4; ++k)
				{
					if (weight.mWeight > vertex.jointWeights[k])
					{
						for (int m = 3; m > k; --m)
						{
							vertex.jointIndices[m] = vertex.jointIndices[m - 1];
							vertex.jointWeights[m] = vertex.jointWeights[m - 1];
						}

						if (auto it = joints.find(bone->mName.C_Str()); it != joints.cend())
							vertex.jointIndices[k] = it->second;
						vertex.jointWeights[k] = weight.mWeight;

						break;
					}
				}
			}
		}
	}


	for (unsigned int i = 0; i < node->mNumChildren; ++i)
		ReadMesh(scene, node->mChildren[i], transform, joints, skeletalMesh);
}

void SkeletalMeshConverter::WriteSkeletalMesh(const wstring & filename, const SkeletalMesh & mesh)
{
	// 1. 서브셋 개수 (4바이트)
// 2. 서브셋 정보 쓰기
// 3. 전체 정점 개수 (4바이트)
// 4. 정점 전부 쓰기
// 5. 전체 인덱스 개수 (4바이트)
// 6. 전체 인덱스 쓰기


	ofstream fout(filename, ofstream::binary);
	//ofstream fout(filename);

	// Subset
	UINT subsetCount = mesh.subsets.size();
	fout.write((char*)&subsetCount, sizeof(UINT));
	fout.write((char*)mesh.subsets.data(), sizeof(SkeletalMesh::Subset) * subsetCount);

	// Material
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

	// Vertex
	UINT vertexCount = mesh.vertices.size();
	fout.write((char*)&vertexCount, sizeof(UINT));
	fout.write((char*)mesh.vertices.data(), sizeof(SkeletalMeshVertex) * vertexCount);

	// Index
	UINT indexCount = mesh.indices.size();
	fout.write((char*)&indexCount, sizeof(UINT));
	fout.write((char*)mesh.indices.data(), sizeof(UINT) * indexCount);

	fout.close();
}

void SkeletalMeshConverter::FileSearch(IN string inFile_path, OUT string outFile_path)
{
	intptr_t handle;
	int check = 0;
	string file_path2;

	inFile_path += "\\";
	file_path2 = inFile_path;
	inFile_path += "*";

	if ((handle = _findfirst(inFile_path.c_str(), &fd)) == -1)
	{
		cout << "No such file or directory" << endl;
		return;
	}

	while (_findnext(handle, &fd) == 0)
	{
		string file_pt;
		file_pt = file_path2;
		file_pt += fd.name;
		string str = fd.name;
		string strName = str.substr(0, str.length() - 2);
		string strEXT = str.substr(str.length() - 2, str.length());


		check = isFileOrDir();

		if (check == 0 && fd.name[0] != '.')
		{
			FileSearch(file_pt, outFile_path);
		}
		else if (check == 1 && fd.size != 0 && fd.name[0] != '.' && strEXT == ".x")
		{
			cout << "파일명 : " << file_pt.c_str() << ", 크기 : " << fd.size << endl;

			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(file_pt,
				aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded);

			if (scene != nullptr)
			{
				const aiNode* root = scene->mRootNode;

				JointHashMap joints;
				SkeletalMesh skm;

				ReadSkeleton(root, 0, joints);
				ReadMesh(scene, root, aiMatrix4x4(), joints, skm);

				for (int i = 0; i < skm.materials.size(); i++)
				{
					for (auto & mtl : skm.materials[i].textureMaps)
					{
						// 중복 파일 검사
						bool check = false;
						for (auto & dup : _deduplication)
						{
							if (dup == mtl.second)
							{
								check = true;
								break;
							}
						}
						if (check)
						{
							//cout << "중복파일명 : " << mtl.second << endl;
							continue;
						}

						for (auto & t_path : _texture_path)
							CopyFileSearch(t_path, mtl.second, outFile_path);
					}
				}

				// 파일 쓰기
				string strTemp = outFile_path + "\\" + strName + ".skm";
				wstring wsfout_pt;
				wsfout_pt.assign(strTemp.begin(), strTemp.end());
				WriteSkeletalMesh(wsfout_pt, skm);
				cout << "파일경로 : " << strTemp << "\n";
			}
		}
	}
	_findclose(handle);
}

void SkeletalMeshConverter::DuplicationFileSearch(string File_path)
{
	intptr_t handle;
	int check = 0;
	string file_path2;

	File_path += "\\";
	file_path2 = File_path;
	File_path += "*";

	if ((handle = _findfirst(File_path.c_str(), &fd)) == -1)
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
			DuplicationFileSearch(file_pt);
		}
		else if (check == 1 && fd.size != 0 && fd.name[0] != '.')
		{
			_deduplication.push_back(fd.name);
		}
	}
	_findclose(handle);
}

int SkeletalMeshConverter::isFileOrDir()
{
	if (fd.attrib & _A_SUBDIR) //Dir
		return 0;
	else // File
		return 1;
}

void SkeletalMeshConverter::Run(char* in, char* out)
{
	string inFile_path = in;
	string outFile_path = out;

	WCHAR root[MAX_PATH];
	GetModuleFileNameW(NULL, root, MAX_PATH);

	wstring fullPath = root;
	fullPath = fullPath.substr(0, fullPath.rfind('\\'));
	fullPath += L"\\config.txt";

	ifstream fin(fullPath);
	if (fin.fail())
		return;


	string line;
	while (getline(fin, line))
		_texture_path.push_back(line.substr(line.find(":") + 2));

	DuplicationFileSearch(outFile_path);

	FileSearch(inFile_path, outFile_path);
}