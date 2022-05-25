#pragma once
#include "CTrap.h"

//㩊Ǘ��N���X
class CTrapManager {
private:
	static CTrapManager* mInstance; //㩊Ǘ��̃C���X�^���X

	std::vector<CTrap*> mTrapList; //㩊i�[
public:
	CTrapManager();
	~CTrapManager();

	static void Generate();	//����
	static void Release();	//���
	static CTrapManager* GetInstance();	//�C���X�^���X�擾

	void Update();

	void TrapGenerate(CVector pos, CVector rot); //㩐���

	int mTrapQuantity;		//㩂̏�����

	bool mMapTrap;			//�}�b�v���㩂������true

	bool TrapAvailable(); //㩂��g�p�\�Ȏ�true��Ԃ�
};