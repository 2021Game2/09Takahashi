#pragma once
#include "CTrap.h"

//㩊Ǘ��N���X
class CTrapManager {
private:
	static CTrapManager* mInstance; //㩊Ǘ��̃C���X�^���X

	std::vector<CTrap*> mTrapList;	//㩊i�[

	int mMapTrapNum;	//�}�b�v��ɂ���L����㩂̐�
public:
	CTrapManager();		//�f�t�H���g�R���X�g���N�^
	~CTrapManager();	//�f�X�g���N�^

	static void Generate();	//����
	static void Release();	//���
	static CTrapManager* GetInstance();	//�C���X�^���X�擾

	void Update();	//�X�V

	void TrapGenerate(CVector pos, CVector rot); //㩐���

	bool TrapAvailable(); //㩂��g�p�\�Ȏ�true��Ԃ�
};