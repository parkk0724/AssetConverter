#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"


#define Direct3D
#include "Math.h"

#if defined(_DEBUG) || defined(DEBUG)
#pragma comment(lib, "assimp-vc141-mtd.lib")
#else
#pragma comment(lib, "assimp-vc141-mt.lib")
#endif

using namespace std;
using namespace Math;

struct StaticMesh
{
	Math::Vector3 position;
	Math::Vector3 normal;
	Math::Vector2 uv;
};

struct SkinnedVertex
{
	Math::Vector3 position;
	Math::Vector3 normal;
	Math::Vector2 uv;
	Math::Vector4 color;
	string joints[4];
	float jointWeights[4] = { 0, };

};

struct Material
{
	Vector4 diffuse;
	Vector4 ambient;
	Vector4 specular;
	string texName;
};

struct SkeletalMeshSubset
{
	Material material;
	vector<SkinnedVertex> vertices;
	vector<uint32_t> indices;
	Math::Matrix4x4 globalTransform;
	std::unordered_map<std::string, Math::Matrix4x4> offsetTransforms;
};

struct SkeletalMesh
{
	vector<SkeletalMeshSubset> subsets;
	Math::Matrix4x4 finalTransform;
};

// 메시 개수 : 32비트

int main()
{
	SkeletalMesh skeletalmesh;

	ifstream skm_read("abc.sm", ios::binary);

	if (skm_read.is_open())
	{
		skm_read.seekg(0, ios::end);
		long size = skm_read.tellg();

		skm_read.seekg(0, ios::beg);

		int32_t skmMeshSize;
		skm_read.read((char*)&skmMeshSize, sizeof(int32_t));
		skeletalmesh.subsets.resize(skmMeshSize);
		for (int i = 0; i < skmMeshSize; i++)
		{
			int32_t skmSubsetsVerticesSize;
			skm_read.read((char*)&skmSubsetsVerticesSize, sizeof(int32_t));
			for (int j = 0; j < skmSubsetsVerticesSize; j++)
			{
				SkinnedVertex sV;
				skm_read.read((char*)&sV.position, sizeof(Vector3));

				skm_read.read((char*)&sV.normal, sizeof(Vector3));

				skm_read.read((char*)&sV.uv, sizeof(Vector2));

				skm_read.read((char*)&sV.color, sizeof(Vector4));

				for (int k = 0; k < 4; k++)
				{
					int32_t len;
					skm_read.read((char*)&len, sizeof(int32_t));
					sV.joints[k].resize(len);
					skm_read.read(&sV.joints[k][0], sizeof(char)*len);
					skm_read.read((char*)&sV.jointWeights[k], sizeof(float));
				}
				skeletalmesh.subsets[i].vertices.push_back(sV);
			}

			int32_t skmSubsetsIndicesSize;
			skm_read.read((char*)&skmSubsetsIndicesSize, sizeof(int32_t));

			for (int j = 0; j < skmSubsetsIndicesSize; j++)
			{
				int32_t index;
				skm_read.read((char*)&index, sizeof(int32_t));
				skeletalmesh.subsets[i].indices.push_back(index);
			}

			int32_t skmSubsetBonesSize;
			skm_read.read((char*)&skmSubsetBonesSize, sizeof(int32_t));

			for (int j = 0; j < skmSubsetBonesSize; j++)
			{
				int32_t len;
				skm_read.read((char*)&len, sizeof(int32_t));
				string str;
				str.resize(len);
				skm_read.read(&str[0], sizeof(char)*len);
				Matrix4x4 m4;
				skm_read.read((char*)&m4, sizeof(Matrix4x4));
				skeletalmesh.subsets[i].offsetTransforms[str] = m4;
			}

			skm_read.read((char*)&skeletalmesh.subsets[i].material.diffuse, sizeof(Vector4));
			skm_read.read((char*)&skeletalmesh.subsets[i].material.ambient, sizeof(Vector4));
			skm_read.read((char*)&skeletalmesh.subsets[i].material.specular, sizeof(Vector4));
			int32_t len;
			skm_read.read((char*)&len, sizeof(int32_t));
			skeletalmesh.subsets[i].material.texName.resize(len);
			skm_read.read(&skeletalmesh.subsets[i].material.texName[0], sizeof(char)*len);
		}
	}
	skm_read.close();

	cout << "OK" << endl;

	return 0;
}