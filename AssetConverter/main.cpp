#include <cstdint>
#include <iostream>
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
	float jointWeights[4];
	
};

struct SkeletalMeshSubset
{
	string materialName;
	vector<SkinnedVertex> vertices;
	vector<uint32_t> indices;
	std::unordered_map<std::string, Math::Matrix4x4> offsetTransforms;
	int material;
};

struct Material
{
	Vector4 diffuse;
	Vector4 ambient;
	Vector4 specular;
	string name;
};


struct SkeletalMesh
{
	vector<Material> materials;
	vector<SkeletalMeshSubset> subsets;
};

int main(int argc, char* argv[])
{
	//if (strcmp(argv[1], "-sm") == 0)
	//{
	//	// 일반 물체 (static mesh)
	//}
	//else if (strcmp(argv[1], "-sk") == 0)
	//{
	//	// 사람 피부 (skeleltal mesh or skinned mesh)
	//}

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile("sword1.x", aiProcess_ConvertToLeftHanded);

	if (scene != nullptr)
	{
		Material material;
		for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
		{
			aiString path;
			aiMaterial * m = scene->mMaterials[i];
			m->GetTexture(aiTextureType_DIFFUSE, 0, &path);
			material.name = path.C_Str();
			aiColor3D color;
			m->Get(AI_MATKEY_COLOR_DIFFUSE, color);
			int a = 0;
		}

		SkeletalMesh skeletalmesh;
		skeletalmesh.subsets.resize(scene->mNumMeshes);
		for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
		{
			// 위치
			const aiMesh* mesh = scene->mMeshes[i];
			//skeletalmesh.subsets[i].materialName = mesh->ma
			SkinnedVertex skVertex;

			for (unsigned int j = 0; j < mesh->mNumVertices; ++j)
			{
				const aiVector3D& position = mesh->mVertices[j];
				Vector3 pos(position.x, position.y, position.z);
				skVertex.position = pos;

				const aiVector3D& normal = mesh->mNormals[j];
				Vector3 nom(normal.x, normal.y, normal.z);
				skVertex.normal = nom;

				const aiVector3D& texture = mesh->mTextureCoords[0][j];
				Vector2 tex(texture.x, texture.y);
				skVertex.uv = tex;

				//const aiColor4D * color = mesh->mColors[0];
				//Vector4 col(color->a, color->r, color->g, color->b);
				//skVertex.color = col;

				skeletalmesh.subsets[i].vertices.push_back(skVertex);
			}

			// 면
			
			for (int i = 0; i > mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[0];
				uint32_t * a = face.mIndices;
				uint32_t b = face.mNumIndices;
			}

			// 사람 피부인 경우에만

			// 뼈
			for (unsigned int i = 0; i < mesh->mNumBones; ++i)
			{
				const aiBone* bone = mesh->mBones[i];

				Math::Matrix4x4 offset;
				offset[0][0] = bone->mOffsetMatrix.a1;

				skeletalmesh.subsets[0].offsetTransforms[bone->mName.C_Str()] = offset;

				for (unsigned int j = 0; j < bone->mNumWeights; ++j)
				{
					const aiVertexWeight& weight = bone->mWeights[j];
					skeletalmesh.subsets[0].vertices[weight.mVertexId].joints[5] = bone->mName.C_Str();
					skeletalmesh.subsets[0].vertices[weight.mVertexId].jointWeights[5] = weight.mWeight;
				}
			}

			//mesh->
		}
	}

	return 0;
}