#pragma once
#include "CTrap.h"

//㩊Ǘ��N���X
class CTrapManager {
private:
	static CTrapManager* mInstance; //㩊Ǘ��̃C���X�^���X

	std::vector<CTrap*> mTrapList; //㩊i�[

	bool mMapTrap;			//�}�b�v���㩂������true
public:
	CTrapManager();		//�f�t�H���g�R���X�g���N�^
	~CTrapManager();	//�f�X�g���N�^

	static void Generate();	//����
	static void Release();	//���
	static CTrapManager* GetInstance();	//�C���X�^���X�擾

	void Update();	//�X�V

	void TrapGenerate(CVector pos, CVector rot); //㩐���

	void SetMapTrapFlag(bool flag); //�}�b�v���㩂����邩���f����t���O��ݒ�

	bool TrapAvailable(); //㩂��g�p�\�Ȏ�true��Ԃ�
};