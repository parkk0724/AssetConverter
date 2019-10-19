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
	// 경로 내의 파일 정보를 입력 받을 구조체 및 핸들
	intptr_t handle;

	// CMD 상에서 입력받은 인자 중 세 번쨰 인자(입력 경로)를 string 형식으로 저장
	string input_path(in);

	// 입력 경로에서 *.x 확장자인 파일을 모두 찾기 위해 입력 경로에 /*.x를 붙여서 사용
	string input_path_x;
	input_path_x.assign(input_path);
	input_path_x.append("/*.x");

	// 만약 입력 경로가 잘못되었다면(폴더가 없는 경우 등) 프로그램 종료
	if ((handle = _findfirst(input_path_x.c_str(), &fd)) == -1L)
	{
		cout << "해당 경로가 잘못되었거나 존재하지 않습니다." << endl;
		return;
	}

	//  *.x 확장자 파일의 이름들을 string형으로 저장할 vector형 변수
	vector<string> x_fnames;

	// 입력 경로 내에 모든 파일들을 불러옴
	do
	{
		// 만약 파일이면 vector에 저장
		if (isFileOrDir())
		{
			x_fnames.push_back(fd.name);
		}

	} while (_findnext(handle, &fd) == 0);

	// 경로를 넣어놨던 핸들 메모리 해제
	_findclose(handle);

	Assimp::Importer importer;
	const aiScene* scene;

	// 파일 이름을 저장한 vector 내의 모든 파일들을 가져와서 변환
	for (int i = 0; i < x_fnames.size(); i++)
	{
		// 현재 *.x 파일 이름 저장
		string curr_x_fname = x_fnames.at(i);

		// 현재 *.x 파일 이름에 경로까지 추가한 string을 저장
		string input_x_fname;
		input_x_fname.assign(input_path);
		input_x_fname.append("/");
		input_x_fname.append(curr_x_fname);

		// .x 파일 load
		scene = importer.ReadFile(input_x_fname.c_str(), aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded);

		cout << input_x_fname << " 뼈 정보를 불러옵니다." << endl;

		// 파일 이름(경로)을 확인
		if (strcmp(importer.GetErrorString(), ""))
		{
			cout << "잘못된 파일 이름를 입력하셨습니다." << endl;
			importer.FreeScene();

			return;
		}

		// 파일이 정상적으로 불러와졌을 경우, 모든 뼈 정보 읽어옴
		for (unsigned int i = 0; i < scene->mRootNode->mNumChildren; ++i)
			ReadSkeleton(scene->mRootNode->mChildren[i], 0, -1);

		// binary 형식 파일로 저장
		// 0. 4x4 행렬
		// 1. 뼈의 개수 (1바이트) : uint8_t
		// 2. 이름의 바이트 수(4바이트) : int
		// 3. 이름 데이터 : char[]
		// 4. 부모 인덱스(4바이트) : int

		// 총 뼈의 개수, 뼈 이름의 길이, 부모 뼈의 index를 저장할 변수
		uint8_t joint_count;
		int skel_name_length;
		int skel_parent_index;

		// 총 뼈의 개수 저장
		joint_count = (uint8_t)sk.joints.size();

		// Root node의 transformation 행렬의 전치 행렬 저장
		aiMatrix4x4 mTrans_t = scene->mRootNode->mTransformation.Transpose();

		// root node의 transformation 행렬을 sk에 저장
		Math::Matrix4x4 mat
		(
			mTrans_t.a1, mTrans_t.b1, mTrans_t.c1, mTrans_t.d1,
			mTrans_t.a2, mTrans_t.b2, mTrans_t.c2, mTrans_t.d2,
			mTrans_t.a3, mTrans_t.b3, mTrans_t.c3, mTrans_t.d3,
			mTrans_t.a4, mTrans_t.b4, mTrans_t.c4, mTrans_t.d4
		);
		sk.Root = mat;

		// 저장할 *.skt 파일의 이름 저장, CMD 상에서 입력받은 출력 경로까지 모두 합침
		string output_x_fname;
		output_x_fname.assign(out);
		output_x_fname.append("/");
		output_x_fname.append(curr_x_fname.substr(0, curr_x_fname.length() - 2));
		output_x_fname.append(".skt");

		// 내용을 출력할 파일 생성 및 바이너리 모드로 진행
		ofstream skt_out(output_x_fname.c_str(), ios::binary);

		// 출력할 파일이 정상적으로 열린 경우 실행
		if (skt_out.is_open())
		{
			cout << output_x_fname << "에 뼈 정보를 저장합니다." << endl;

			// root node의 transformation 행렬 저장
			//skt_out << sk.Root;
			skt_out.write((char*)&sk.Root, sizeof(Math::Matrix4x4));

			// 뼈의 개수 파일에 출력
			skt_out.write((char*)&joint_count, sizeof(uint8_t));

			// 뼈의 개수에 따라 모든 뼈들의 정보 저장
			for (int i = 0; i < joint_count; i++)
			{
				// 뼈의 이름 길이 가져옴
				skel_name_length = sk.joints.at(i).name.length();

				// 뼈의 이름의 길이를 4바이트(int) 만큼의 자리로 저장
				skt_out.write((char*)&skel_name_length, sizeof(int));

				// 뼈의 이름을 이름 길이만큼 저장
				skt_out.write((char*)sk.joints.at(i).name.c_str(), skel_name_length);

				// 부모 인덱스 가져옴
				skel_parent_index = sk.joints.at(i).parent;

				// 부모 인덱스를 4바이트(int) 만큼의 자리로 저장
				skt_out.write((char*)&skel_parent_index, sizeof(int));
			}

			// 메오리 해제
			skt_out.close();
		}
		// 출력 경로가 잘못된 경우(폴더가 없는 경우 등) 프로그램 종료
		else
		{
			cout << "잘못된 출력 경로를 입력하셨습니다." << endl;
			return;
		}
	}
	return;
}
