#include "Include.h"
#include "SkeletalConverter.h"

SkeletalConverter::SkeletalConverter()
{
}


SkeletalConverter::~SkeletalConverter()
{
}

void SkeletalConverter::ReadSkeleton(const aiNode * node, int index, int parent)
{
	aiMatrix4x4 t = node->mTransformation;

	Joint joint;
	joint.name = node->mName.C_Str();
	joint.parent = parent;

	sk.joints.push_back(joint);

	Math::Matrix4x4 mat
	(
		t.a1, t.b1, t.c1, t.d1,
		t.a2, t.b2, t.c2, t.d2,
		t.a3, t.b3, t.c3, t.d3,
		t.a4, t.b4, t.c4, t.d4
	);
	sk.Root = mat;

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
		ReadSkeleton(node->mChildren[i], sk.joints.size(), index);
}

int SkeletalConverter::isFileOrDir()
{
	if (fd.attrib & _A_SUBDIR) //Dir
		return 0;
	else // File
		return 1;
}

void SkeletalConverter::Run(char* in, char* out)
{
	// ��� ���� ���� ������ �Է� ���� ����ü �� �ڵ�
	intptr_t handle;

	// CMD �󿡼� �Է¹��� ���� �� �� ���� ����(�Է� ���)�� string �������� ����
	string input_path(in);

	// �Է� ��ο��� *.x Ȯ������ ������ ��� ã�� ���� �Է� ��ο� /*.x�� �ٿ��� ���
	string input_path_x;
	input_path_x.assign(input_path);
	input_path_x.append("/*.x");

	// ���� �Է� ��ΰ� �߸��Ǿ��ٸ�(������ ���� ��� ��) ���α׷� ����
	if ((handle = _findfirst(input_path_x.c_str(), &fd)) == -1L)
	{
		cout << "�ش� ��ΰ� �߸��Ǿ��ų� �������� �ʽ��ϴ�." << endl;
		return;
	}

	//  *.x Ȯ���� ������ �̸����� string������ ������ vector�� ����
	vector<string> x_fnames;

	// �Է� ��� ���� ��� ���ϵ��� �ҷ���
	do
	{
		// ���� �����̸� vector�� ����
		if (isFileOrDir())
		{
			x_fnames.push_back(fd.name);
		}

	} while (_findnext(handle, &fd) == 0);

	// ��θ� �־���� �ڵ� �޸� ����
	_findclose(handle);

	Assimp::Importer importer;
	const aiScene* scene;

	// ���� �̸��� ������ vector ���� ��� ���ϵ��� �����ͼ� ��ȯ
	for (int i = 0; i < x_fnames.size(); i++)
	{
		// ���� *.x ���� �̸� ����
		string curr_x_fname = x_fnames.at(i);

		// ���� *.x ���� �̸��� ��α��� �߰��� string�� ����
		string input_x_fname;
		input_x_fname.assign(input_path);
		input_x_fname.append("/");
		input_x_fname.append(curr_x_fname);

		// .x ���� load
		scene = importer.ReadFile(input_x_fname.c_str(), aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded);

		cout << input_x_fname << " �� ������ �ҷ��ɴϴ�." << endl;

		// ���� �̸�(���)�� Ȯ��
		if (strcmp(importer.GetErrorString(), ""))
		{
			cout << "�߸��� ���� �̸��� �Է��ϼ̽��ϴ�." << endl;
			importer.FreeScene();

			return;
		}

		// ������ ���������� �ҷ������� ���, ��� �� ���� �о��
		for (unsigned int i = 0; i < scene->mRootNode->mNumChildren; ++i)
			ReadSkeleton(scene->mRootNode->mChildren[i], 0, -1);

		// binary ���� ���Ϸ� ����
		// 0. 4x4 ���
		// 1. ���� ���� (1����Ʈ) : uint8_t
		// 2. �̸��� ����Ʈ ��(4����Ʈ) : int
		// 3. �̸� ������ : char[]
		// 4. �θ� �ε���(4����Ʈ) : int

		// �� ���� ����, �� �̸��� ����, �θ� ���� index�� ������ ����
		uint8_t joint_count;
		int skel_name_length;
		int skel_parent_index;

		// �� ���� ���� ����
		joint_count = (uint8_t)sk.joints.size();

		// Root node�� transformation ����� ��ġ ��� ����
		aiMatrix4x4 mTrans_t = scene->mRootNode->mTransformation.Transpose();

		// root node�� transformation ����� sk�� ����
		Math::Matrix4x4 mat
		(
			mTrans_t.a1, mTrans_t.b1, mTrans_t.c1, mTrans_t.d1,
			mTrans_t.a2, mTrans_t.b2, mTrans_t.c2, mTrans_t.d2,
			mTrans_t.a3, mTrans_t.b3, mTrans_t.c3, mTrans_t.d3,
			mTrans_t.a4, mTrans_t.b4, mTrans_t.c4, mTrans_t.d4
		);
		sk.Root = mat;

		// ������ *.skt ������ �̸� ����, CMD �󿡼� �Է¹��� ��� ��α��� ��� ��ħ
		string output_x_fname;
		output_x_fname.assign(out);
		output_x_fname.append("/");
		output_x_fname.append(curr_x_fname.substr(0, curr_x_fname.length() - 2));
		output_x_fname.append(".skt");

		// ������ ����� ���� ���� �� ���̳ʸ� ���� ����
		ofstream skt_out(output_x_fname.c_str(), ios::binary);

		// ����� ������ ���������� ���� ��� ����
		if (skt_out.is_open())
		{
			cout << output_x_fname << "�� �� ������ �����մϴ�." << endl;

			// root node�� transformation ��� ����
			//skt_out << sk.Root;
			skt_out.write((char*)&sk.Root, sizeof(Math::Matrix4x4));

			// ���� ���� ���Ͽ� ���
			skt_out.write((char*)&joint_count, sizeof(uint8_t));

			// ���� ������ ���� ��� ������ ���� ����
			for (int i = 0; i < joint_count; i++)
			{
				// ���� �̸� ���� ������
				skel_name_length = sk.joints.at(i).name.length();

				// ���� �̸��� ���̸� 4����Ʈ(int) ��ŭ�� �ڸ��� ����
				skt_out.write((char*)&skel_name_length, sizeof(int));

				// ���� �̸��� �̸� ���̸�ŭ ����
				skt_out.write((char*)sk.joints.at(i).name.c_str(), skel_name_length);

				// �θ� �ε��� ������
				skel_parent_index = sk.joints.at(i).parent;

				// �θ� �ε����� 4����Ʈ(int) ��ŭ�� �ڸ��� ����
				skt_out.write((char*)&skel_parent_index, sizeof(int));
			}

			// �޿��� ����
			skt_out.close();
		}
		// ��� ��ΰ� �߸��� ���(������ ���� ��� ��) ���α׷� ����
		else
		{
			cout << "�߸��� ��� ��θ� �Է��ϼ̽��ϴ�." << endl;
			return;
		}
	}
	return;
}
