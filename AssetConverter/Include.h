#pragma once
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

#define Direct3D
#include "Math.h"
#include <cstdint>

using namespace std;

#if defined(_DEBUG) || defined(DEBUG)
#pragma comment(lib, "assimp-vc141-mtd.lib")
#else
#pragma comment(lib, "assimp-vc141-mt.lib")
#endif

enum class TextureType
{
	Ambient,
	Diffuse,
	Specular,
};

struct Material
{
	aiColor4D ambient;
	aiColor4D diffuse;
	aiColor4D specular;

	unordered_map<TextureType, string> textureMaps;
};

// StaticMesh
struct StaticMeshVertex
{
	aiVector3D position;
	aiVector3D normal;
	aiVector2D uv;
};

struct StaticMesh
{
	struct Subset
	{
		UINT vertexStart;
		UINT vertexCount;
		UINT faceStart;
		UINT faceCount;
	};

	vector<Subset> subsets;
	vector<Material> materials;
	vector<StaticMeshVertex> vertices;
	vector<UINT> indices;
};

// Skeletal
template <typename T>
struct KeyFrame
{
	float time;
	T data;
};

struct SkinnedVertex
{
	Math::Vector3 position;
	Math::Vector3 normal;
	Math::Vector2 uv;
	uint8_t jointIndices[4];
	float jointWeights[4];
};

struct Joint
{
	string name;
	int parent;
};

struct Skeleton
{
	vector<Joint> joints;
	Math::Matrix4x4 Root;
	aiMatrix4x4 t;
};

// SkeletalMesh
struct SkeletalMeshVertex
{
	aiVector3D position;
	aiVector3D normal;
	aiVector2D uv;
	aiColor4D color;
	uint8_t jointIndices[4] = { 0,0,0,0 };
	float jointWeights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
};

struct SkeletalMesh
{
	struct Subset
	{
		UINT vertexStart;
		UINT vertexCount;
		UINT faceStart;
		UINT faceCount;
	};

	vector<Subset> subsets;
	vector<Material> materials;
	vector<SkeletalMeshVertex> vertices;
	vector<UINT> indices;
};
using JointHashMap = unordered_map<string, int>;