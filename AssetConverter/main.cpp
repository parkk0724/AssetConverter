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
	float jointWeights[4] = {0,};
	
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
};

void ReadNode(const aiScene* scene, const aiNode* node, const Math::Matrix4x4& parent, SkeletalMesh& skeletalMesh)
{
	Math::Matrix4x4 global = parent;

	if (node->mNumMeshes > 0)
	{
		aiMatrix4x4 t = node->mTransformation;
		t.Transpose();

		Math::Matrix4x4 local(
			t.a1, t.a2, t.a3, t.a4,
			t.b1, t.b2, t.b3, t.b4,
			t.c1, t.c2, t.c3, t.c4,
			t.d1, t.d2, t.d3, t.d4
		);

		global = local * parent;
	}

	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		
		SkeletalMeshSubset subset;
		
		// 
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

			subset.vertices.push_back(skVertex);
		}

		// 면

		for (unsigned int j = 0; j < mesh->mNumFaces; ++j)
		{
			aiFace face = mesh->mFaces[j];
			uint32_t * faceIndices = face.mIndices;
			uint32_t faceNum = face.mNumIndices;

			for (int k = face.mNumIndices - 1; k >= 0; --k)
			{
				subset.indices.push_back(faceIndices[k]);
			}
		}

		// 사람 피부인 경우에만

		// 뼈
		for (unsigned int j = 0; j < mesh->mNumBones; ++j)
		{
			const aiBone* bone = mesh->mBones[j];

			Math::Matrix4x4 offset;
			offset[0][0] = bone->mOffsetMatrix.a1;
			offset[0][1] = bone->mOffsetMatrix.b1;
			offset[0][2] = bone->mOffsetMatrix.c1;
			offset[0][3] = bone->mOffsetMatrix.d1;

			offset[1][0] = bone->mOffsetMatrix.a2;
			offset[1][1] = bone->mOffsetMatrix.b2;
			offset[1][2] = bone->mOffsetMatrix.c2;
			offset[1][3] = bone->mOffsetMatrix.d2;

			offset[2][0] = bone->mOffsetMatrix.a3;
			offset[2][1] = bone->mOffsetMatrix.b3;
			offset[2][2] = bone->mOffsetMatrix.c3;
			offset[2][3] = bone->mOffsetMatrix.d3;

			offset[3][0] = bone->mOffsetMatrix.a4;
			offset[3][1] = bone->mOffsetMatrix.b4;
			offset[3][2] = bone->mOffsetMatrix.c4;
			offset[3][3] = bone->mOffsetMatrix.d4;

			subset.offsetTransforms[bone->mName.C_Str()] = offset;

			//
			for (unsigned int k = 0; k < bone->mNumWeights; ++k)
			{
				const aiVertexWeight& weight = bone->mWeights[k];

				for (int l = 0; l < 4; ++l)
				{
					if (weight.mWeight > subset.vertices[weight.mVertexId].jointWeights[l])
					{
						subset.vertices[weight.mVertexId].joints[l] = bone->mName.C_Str();
						subset.vertices[weight.mVertexId].jointWeights[l] = weight.mWeight;
						break;
					}
				}
			}


			//>>
		}

		Material material;
		aiString path;
		aiMaterial * aimaterial = scene->mMaterials[i];
		aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path);
		material.texName = path.C_Str();
		aiColor3D color;
		aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		material.diffuse.x = color.r;
		material.diffuse.y = color.g;
		material.diffuse.z = color.b;
		material.diffuse.w = 1.0f;

		subset.material = material;

		subset.globalTransform = global;
		skeletalMesh.subsets.push_back(subset);
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
		ReadNode(scene, node->mChildren[i], global, skeletalMesh);
}

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
	const aiScene* scene = importer.ReadFile("mantle1.x", aiProcess_ConvertToLeftHanded);

	if (scene != nullptr)
	{
		SkeletalMesh skeletalmesh;
		
		Math::Matrix4x4 transform(
			scene->mRootNode->mTransformation.a1, scene->mRootNode->mTransformation.b1, scene->mRootNode->mTransformation.c1, scene->mRootNode->mTransformation.d1,
			scene->mRootNode->mTransformation.a2, scene->mRootNode->mTransformation.b2, scene->mRootNode->mTransformation.c2, scene->mRootNode->mTransformation.d2,
			scene->mRootNode->mTransformation.a3, scene->mRootNode->mTransformation.b3, scene->mRootNode->mTransformation.c3, scene->mRootNode->mTransformation.d3,
			scene->mRootNode->mTransformation.a4, scene->mRootNode->mTransformation.b4, scene->mRootNode->mTransformation.c4, scene->mRootNode->mTransformation.d4
		);
		ReadNode(scene, scene->mRootNode, transform, skeletalmesh);


		//skeletalmesh.subsets.resize(scene->mNumMeshes);
		//for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
		//{
		//	// 위치
		//	const aiMesh* mesh = scene->mMeshes[i];
		//	SkinnedVertex skVertex;

		//	for (unsigned int j = 0; j < mesh->mNumVertices; ++j)
		//	{
		//		const aiVector3D& position = mesh->mVertices[j];
		//		Vector3 pos(position.x, position.y, position.z);
		//		skVertex.position = pos;

		//		const aiVector3D& normal = mesh->mNormals[j];
		//		Vector3 nom(normal.x, normal.y, normal.z);
		//		skVertex.normal = nom;

		//		const aiVector3D& texture = mesh->mTextureCoords[0][j];
		//		Vector2 tex(texture.x, texture.y);
		//		skVertex.uv = tex;

		//		//const aiColor4D * color = mesh->mColors[0];
		//		//Vector4 col(color->a, color->r, color->g, color->b);
		//		//skVertex.color = col;

		//		skeletalmesh.subsets[i].vertices.push_back(skVertex);
		//	}

		//	// 면

		//	for (unsigned int j = 0; j < mesh->mNumFaces; ++j)
		//	{
		//		aiFace face = mesh->mFaces[j];
		//		uint32_t * faceIndices = face.mIndices;
		//		uint32_t faceNum = face.mNumIndices;

		//		for (int k = face.mNumIndices - 1; k >= 0; --k)
		//		{
		//			skeletalmesh.subsets[i].indices.push_back(faceIndices[k]);
		//		}
		//	}

		//	// 사람 피부인 경우에만

		//	// 뼈
		//	for (unsigned int j = 0; j < mesh->mNumBones; ++j)
		//	{
		//		const aiBone* bone = mesh->mBones[j];

		//		Math::Matrix4x4 offset;
		//		offset[0][0] = bone->mOffsetMatrix.a1;
		//		offset[0][1] = bone->mOffsetMatrix.b1;
		//		offset[0][2] = bone->mOffsetMatrix.c1;
		//		offset[0][3] = bone->mOffsetMatrix.d1;

		//		offset[1][0] = bone->mOffsetMatrix.a2;
		//		offset[1][1] = bone->mOffsetMatrix.b2;
		//		offset[1][2] = bone->mOffsetMatrix.c2;
		//		offset[1][3] = bone->mOffsetMatrix.d2;

		//		offset[2][0] = bone->mOffsetMatrix.a3;
		//		offset[2][1] = bone->mOffsetMatrix.b3;
		//		offset[2][2] = bone->mOffsetMatrix.c3;
		//		offset[2][3] = bone->mOffsetMatrix.d3;

		//		offset[3][0] = bone->mOffsetMatrix.a4;
		//		offset[3][1] = bone->mOffsetMatrix.b4;
		//		offset[3][2] = bone->mOffsetMatrix.c4;
		//		offset[3][3] = bone->mOffsetMatrix.d4;

		//		skeletalmesh.subsets[i].offsetTransforms[bone->mName.C_Str()] = offset;

		//		//
		//		for (unsigned int k = 0; k < bone->mNumWeights; ++k)
		//		{
		//			const aiVertexWeight& weight = bone->mWeights[k];

		//			for (int l = 0; l < 4; ++l)
		//			{
		//				if (weight.mWeight > skeletalmesh.subsets[i].vertices[weight.mVertexId].jointWeights[l])
		//				{
		//					skeletalmesh.subsets[i].vertices[weight.mVertexId].joints[l] = bone->mName.C_Str();
		//					skeletalmesh.subsets[i].vertices[weight.mVertexId].jointWeights[l] = weight.mWeight;
		//					break;
		//				}
		//			}
		//		}


		//		//>>
		//	}

		//	Material material;
		//	aiString path;
		//	aiMaterial * aimaterial = scene->mMaterials[i];
		//	aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path);
		//	material.texName = path.C_Str();
		//	aiColor3D color;
		//	aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		//	material.diffuse.x = color.r;
		//	material.diffuse.y = color.g;
		//	material.diffuse.z = color.b;
		//	material.diffuse.w = 1.0f;

		//	skeletalmesh.subsets[i].material = material;
		//}

		// << Write 
		ofstream skm_out("abc.sm", ios::binary);
		if (skm_out.is_open())
		{
			int skmMeshSize = skeletalmesh.subsets.size();
			skm_out.write((char*)&skmMeshSize, sizeof(int));

			for (int i = 0; i < skmMeshSize; i++)
			{
				int skmSubsetsVerticesSize = skeletalmesh.subsets[i].vertices.size();
				skm_out.write((char*)&skmSubsetsVerticesSize, sizeof(int));
				for (int j = 0; j < skmSubsetsVerticesSize; j++)
				{
					skm_out.write((char*)&skeletalmesh.subsets[i].vertices[j].position, sizeof(Vector3));

					skm_out.write((char*)&skeletalmesh.subsets[i].vertices[j].normal, sizeof(Vector3));

					skm_out.write((char*)&skeletalmesh.subsets[i].vertices[j].uv, sizeof(Vector2));

					skm_out.write((char*)&skeletalmesh.subsets[i].vertices[j].color, sizeof(Vector4));

					for (int k = 0; k < 4; k++)
					{
						int len = skeletalmesh.subsets[i].vertices[j].joints[k].length();
						skm_out.write((char*)&len, sizeof(int));
						skm_out.write(skeletalmesh.subsets[i].vertices[j].joints[k].data(), sizeof(char)*len);
						skm_out.write((char*)&skeletalmesh.subsets[i].vertices[j].jointWeights[k], sizeof(float));
					}
				}

				int skmSubsetsIndicesSize = skeletalmesh.subsets[i].indices.size();
				skm_out.write((char*)&skmSubsetsIndicesSize, sizeof(int));
				for (int j = 0; j < skmSubsetsIndicesSize; j++)
				{
					skm_out.write((char*)&skeletalmesh.subsets[i].indices[j], sizeof(uint32_t));
				}

				int skmSubsetBones = skeletalmesh.subsets[i].offsetTransforms.size();
				skm_out.write((char*)&skmSubsetBones, sizeof(int));
				
				unordered_map<string, Matrix4x4>::iterator it = skeletalmesh.subsets[i].offsetTransforms.begin();
				for (it; it != skeletalmesh.subsets[i].offsetTransforms.end(); it++)
				{
					int len = it->first.length();
					skm_out.write((char*)&len, sizeof(int));
					skm_out.write(it->first.data(), sizeof(char)*len);
					skm_out.write((char*)&it->second, sizeof(Matrix4x4));
				}

				skm_out.write((char*)&skeletalmesh.subsets[i].material.diffuse, sizeof(Vector4));
				skm_out.write((char*)&skeletalmesh.subsets[i].material.ambient, sizeof(Vector4));
				skm_out.write((char*)&skeletalmesh.subsets[i].material.specular, sizeof(Vector4));
				int len = skeletalmesh.subsets[i].material.texName.length();
				skm_out.write((char*)&len, sizeof(int));
				skm_out.write(skeletalmesh.subsets[i].material.texName.data(), sizeof(char)*len);

				skm_out.write((char*)&skeletalmesh.subsets[i].globalTransform, sizeof(Matrix4x4));
			}

			cout << "OK" << endl;
		}
		skm_out.close();
	}

	return 0;
}
