#pragma warning (disable : 4996)

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <Windows.h>
#include <iostream>
using namespace std;

struct _finddata_t fd;

#if defined(_DEBUG) || defined(DEBUG)
	#pragma comment(lib, "assimp-vc141-mtd.lib")
#else
	#pragma comment(lib, "assimp-vc141-mt.lib")
#endif

using namespace std;

struct SkeletalMeshVertex
{
	aiVector3D position;
	aiVector3D normal;
	aiVector2D uv;
	string jointIndices[4];
	float jointWeights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
};

struct SkeletalMesh
{
	struct Subset
	{
		vector<SkeletalMeshVertex> vertices;
		vector<uint32_t> indices;
		size_t vertexStart;
		size_t faceStart;
	};

	vector<Subset> subsets;
};

enum class TextureType
{
	Ambient,
	Diffuse,
	Specular,
};

struct Material
{
	unordered_map<TextureType, string> textures;
};

namespace
{
	size_t totalVertexCount = 0;
	size_t totalFaceCount = 0;
}

void ReadMesh(const aiMesh* mesh, SkeletalMesh::Subset& subset)
{
	// Vertex
	for (unsigned int vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex)
	{
		SkeletalMeshVertex vertex;

		vertex.position = mesh->mVertices[vertexIndex];
		vertex.normal = mesh->mNormals[vertexIndex];
		
		if (mesh->HasTextureCoords(0))
		{
			vertex.uv.x = mesh->mTextureCoords[0][vertexIndex].x;
			vertex.uv.y = mesh->mTextureCoords[0][vertexIndex].y;
		}

		subset.vertices.push_back(vertex);
	}

	// Index
	for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
	{
		const aiFace& face = mesh->mFaces[faceIndex];
		subset.indices.push_back(face.mIndices[0]);
		subset.indices.push_back(face.mIndices[1]);
		subset.indices.push_back(face.mIndices[2]);
	}

	subset.vertexStart = totalVertexCount;
	subset.faceStart = totalFaceCount;

	totalVertexCount += mesh->mNumVertices;
	totalFaceCount += mesh->mNumFaces;


	// Joint
	for (unsigned int jointIndex = 0; jointIndex < mesh->mNumBones; ++jointIndex)
	{
		const aiBone* bone = mesh->mBones[jointIndex];

		for (unsigned int j = 0; j < bone->mNumWeights; ++j)
		{
			const aiVertexWeight& weight = bone->mWeights[j];

			SkeletalMeshVertex& vertex = subset.vertices[weight.mVertexId];

			for (int k = 0; k < 4; ++k)
			{
				if (weight.mWeight > vertex.jointWeights[k])
				{
					for (int m = 3; m > k; --m)
					{
						vertex.jointIndices[m] = vertex.jointIndices[m - 1];
						vertex.jointWeights[m] = vertex.jointWeights[m - 1];
					}

					vertex.jointIndices[k] = bone->mName.C_Str();
					vertex.jointWeights[k] = weight.mWeight;

					break;
				}
			}
		}
	}
}

void ProcessNode(const aiScene* scene, const aiNode* node, SkeletalMesh& skeletalMesh)
{
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		
		SkeletalMesh::Subset subset;
		ReadMesh(mesh, subset);

		skeletalMesh.subsets.push_back(subset);
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
		ProcessNode(scene, node->mChildren[i], skeletalMesh);
}

void ReadMaterial(const aiScene* scene, vector<Material>& materials)
{
	for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
	{
		const aiMaterial* material = scene->mMaterials[i];
		Material mat;

		aiString name;
		for (unsigned int j = 0; j < material->GetTextureCount(aiTextureType_DIFFUSE); ++j)
		{
			material->GetTexture(aiTextureType_DIFFUSE, j, &name);
			mat.textures[TextureType::Diffuse] = name.C_Str();
		}

		materials.push_back(mat);
	}
}

void WriteSkeletalMesh(const wstring& filename, const SkeletalMesh& mesh)
{
	//ofstream fout(filename, ofstream::binary);
	ofstream fout(filename);

	// 1. 서브메시 갯수
	size_t subsetCount = mesh.subsets.size();
	//fout.write((char*)&subsetCount, sizeof(size_t));

	fout << subsetCount << "\n";

	// 2. 서브메시 정보
	// 2.1 정점
	// 2.2 인덱스

	for (const auto& subset : mesh.subsets)
	{
		// 버텍스 개수
		size_t vertexCount = subset.vertices.size();
		//fout.write((char*)&vertexCount, sizeof(size_t));
		
		fout << vertexCount << "\n";
		for (const auto& vertex : subset.vertices)
		{
			//fout.write((char*)&vertex.position, sizeof(aiVector3D));
			fout << vertex.position.x << " " << vertex.position.y << " " << vertex.position.z << "\n";
			//fout.write((char*)&vertex.normal, sizeof(aiVector3D));
			fout << vertex.normal.x << " " << vertex.normal.y << " " << vertex.normal.z << "\n";
			//fout.write((char*)&vertex.uv, sizeof(aiVector2D));
			fout << vertex.uv.x << "" << vertex.uv.y << "\n";

			for (size_t i = 0; i < 4; ++i)
			{
				size_t length = vertex.jointIndices[i].length();
				//fout.write((char*)&length, sizeof(size_t));
				fout << length << "\n";

				if (length > 0)
				{
					//fout.write((char*)vertex.jointIndices[i].data(), sizeof(char) * length);
					fout << vertex.jointIndices[i] << "\n";
					//fout.write((char*)&vertex.jointWeights[i], sizeof(float));
					fout << vertex.jointWeights[i] << "\n";
				}
			}
		}

		// 인덱스 개수
		size_t indexCount = subset.indices.size();
		//fout.write((char*)&indexCount, sizeof(size_t));
		fout << indexCount << "\n";

		for (auto index : subset.indices)
			//fout.write((char*)&index, sizeof(uint32_t));
			fout << index << "\n";

		
		// 버텍스 시작 위치
		//fout.write((char*)&subset.vertexStart, sizeof(size_t));
		fout << subset.vertexStart << " ";

		// 인덱스 시작 위치
		//fout.write((char*)&subset.faceStart, sizeof(size_t));
		fout << subset.faceStart << " ";
	}
}

int isFileOrDir()
{
	if (fd.attrib & _A_SUBDIR) //Dir
		return 0;
	else // File
		return 1;
}

void FileSearch(string file_path)
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
		string str = fd.name;
		string strName = str.substr(0, str.length() - 2);
		string strEXT = str.substr(str.length() - 2, str.length());
		

		check = isFileOrDir();

		if (check == 0 && fd.name[0] != '.')
		{
			FileSearch(file_pt);
		}
		else if (check == 1 && fd.size != 0 && fd.name[0] != '.' && strEXT == ".x")
		{
			cout << "파일명 : " << file_pt.c_str() << ", 크기 : " << fd.size << endl;

			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(file_pt,
				aiProcess_JoinIdenticalVertices | aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded);

			if (scene != nullptr)
			{
				const aiNode* root = scene->mRootNode;

				SkeletalMesh skm;

				for (unsigned int i = 0; i < root->mNumChildren; ++i)
					ProcessNode(scene, root->mChildren[i], skm);

				vector<Material> materials;
				ReadMaterial(scene, materials);

				// 파일 쓰기
				string strTemp = file_path2 + strName + ".skm";
				wstring wsfout_pt;
				wsfout_pt.assign(strTemp.begin(), strTemp.end());
				WriteSkeletalMesh(wsfout_pt, skm);
			}
		}
	}
	_findclose(handle);
}

int main(int argc, char* argv[])
{
	string file_path = "C:\\x";
	FileSearch(file_path);

	return 0;
}