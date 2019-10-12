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
	//// t.Transpose()�� �ϰ� �ȴٸ�, sk.Root(0,0) = t.a1; sk.Root(0,1) = t.a2; ... 

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
// argv[0] : ���α׷���
// argv[1] : �ɼ�
// argv[2] : �Է� ���
// argv[3] : ��� ���
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

	// ���̳ʸ�

	// 1. ���� ���� (1����Ʈ) : uint8_t

	// 2. �̸��� ����Ʈ ��(4����Ʈ) : int
	// 3. �̸� ������ : char[]
	// 4. �θ� �ε���(4����Ʈ) : int

	uint8_t joint_size;
	int skel_name_length;
	int skel_parent_index;

	// ���� ���� ����
	joint_size = (uint8_t)sk.joints.size();

	cout << "��ü ���� ����: " << sk.joints.size() << endl << endl;

	// ������ ����� ���� ���� �� ���̳ʸ� ���� ����
	string filename = "\\male_skeleton.skt";
	ofstream skt_out((argv[3] + filename), ios::binary);

	if (skt_out.is_open())
	{
		// ���� ���� ���Ͽ� ���
		skt_out.write((char*)&joint_size, sizeof(uint8_t));

		// ���� ������ ���� ��� ������ ���� ����
		for (int i = 0; i < joint_size; i++)
		{
			// ���� �̸� ���� ������
			skel_name_length = sk.joints.at(i).name.length();

			cout << i << "�� ���� �̸� ����: " << skel_name_length << endl;

			// ���� �̸��� ���̸� 4����Ʈ(int) ��ŭ�� �ڸ��� ����
			skt_out.write((char*)&skel_name_length, sizeof(int));

			cout << i << "�� ���� �̸�: " << sk.joints.at(i).name << endl;

			// ���� �̸��� �̸� ���̸�ŭ ����
			skt_out.write((char*)sk.joints.at(i).name.c_str(), skel_name_length);

			// �θ� �ε��� ������
			skel_parent_index = sk.joints.at(i).parent;

			cout << i << "�� ���� �θ� �ε���: " << skel_parent_index << endl;

			// �θ� �ε����� 4����Ʈ(int) ��ŭ�� �ڸ��� ����
			skt_out.write((char*)&skel_parent_index, sizeof(int));

			cout << endl;
		}
	}

	skt_out.close();

	//cout << endl;

	return 0;
}