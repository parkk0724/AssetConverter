#include "pch.h"
#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "d3dx9.h"


#pragma comment(lib, "assimp-vc140-mt.lib")
#pragma comment(lib, "d3dx9d.lib")


using namespace std;
using namespace std::filesystem;

//anim
template <typename T>
struct KeyFrame
{
	float time;
	T data;
};

struct Sample
{

};

struct AnimationClip
{

};

//pmg
struct SkinnedVertex
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 uv;
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
	D3DXMATRIXA16 Root;
	aiMatrix4x4 t;
};


Skeleton sk;

void ReadSkeleton(const aiNode* node, int index, int parent)
{
	//aiMatrix4x4 t = node->mTransformation;
	////t.Transpose();
	//// t.Transpose()를 하게 된다면, sk.Root(0,0) = t.a1; sk.Root(0,1) = t.a2; ... 

	//sk.Root(0, 0) = t.a1;
	//sk.Root(1, 0) = t.a2; // ...

	Joint joint;
	joint.name = node->mName.C_Str();
	joint.parent = parent;


	sk.joints.push_back(joint);

	//cout << joint.name << endl;

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
		ReadSkeleton(node->mChildren[i], sk.joints.size(), index);
}

//void ReadMesh(const aiNode* node)
//{
//   // position
//
//   // normal
//
//   // uv
//
//   // joint index
//
//   // joint weight
//
//
//   for (unsigned int i = 0; i < node->mNumChildren; ++i)
//      ReadMesh(node->mChildren[i]);
//}

// -skt
// argv[0] : 프로그램명
// argv[1] : 옵션
// argv[2] : 입력 경로
// argv[3] : 출력 경로
int main(int argc, char* argv[])
{
	if (argc != 4)
	{
		return 0;
	}

	if (strcmp(argv[1], "-skt") == 0)
	{

	}

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile("male_skeleton.x", aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded);



	for (unsigned int i = 0; i < scene->mRootNode->mNumChildren; ++i)
		ReadSkeleton(scene->mRootNode->mChildren[i], 0, -1);


	// ReadMesh(scene->mRootNode);

	// 바이너리

	// 1. 뼈의 개수 (1바이트) : uint8_t

	// 2. 이름의 바이트 수(4바이트) : int
	// 3. 이름 데이터 : char[]
	// 4. 부모 인덱스(4바이트) : int

	uint8_t joint_size;
	int skel_name_length;
	int skel_parent_index;

	// 뼈의 개수 저장
	joint_size = (uint8_t)sk.joints.size();

	cout << "전체 뼈의 개수: " << sk.joints.size() << endl << endl;

	// 내용을 출력할 파일 생성 및 바이너리 모드로 진행
	string filename = "\\male_skeleton.skt";
	ofstream skt_out((argv[3] + filename), ios::binary);

	if (skt_out.is_open())
	{
		// 뼈의 개수 파일에 출력
		skt_out.write((char*)&joint_size, sizeof(uint8_t));

		// 뼈의 개수에 따라 모든 뼈들의 정보 저장
		for (int i = 0; i < joint_size; i++)
		{
			// 뼈의 이름 길이 가져옴
			skel_name_length = sk.joints.at(i).name.length();

			cout << i << "번 뼈의 이름 길이: " << skel_name_length << endl;

			// 뼈의 이름의 길이를 4바이트(int) 만큼의 자리로 저장
			skt_out.write((char*)&skel_name_length, sizeof(int));

			cout << i << "번 뼈의 이름: " << sk.joints.at(i).name << endl;

			// 뼈의 이름을 이름 길이만큼 저장
			skt_out.write((char*)sk.joints.at(i).name.c_str(), skel_name_length);

			// 부모 인덱스 가져옴
			skel_parent_index = sk.joints.at(i).parent;

			cout << i << "번 뼈의 부모 인덱스: " << skel_parent_index << endl;

			// 부모 인덱스를 4바이트(int) 만큼의 자리로 저장
			skt_out.write((char*)&skel_parent_index, sizeof(int));

			cout << endl;
		}
	}

	skt_out.close();

	//cout << endl;

	return 0;
}