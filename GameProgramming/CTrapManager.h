#pragma once
#include "CTrap.h"
#include "CModel.h"

class CTrapManager {
private:
	static CTrapManager* mInstance;

	std::vector<CTrap*> mTrapList;
public:
	CTrapManager();
	~CTrapManager();

	static void Generate();
	static void Release();
	static CTrapManager* GetInstance();

	void Update();

	static CModel sTrap;

	void TrapGenerate(CVector pos, CVector rot); //㩐���

	int mTrapQuantity;		//㩂̏�����

	bool mMapTrap;			//�}�b�v���㩂������true

	bool TrapAvailable(); //㩂��g�p�\�Ȏ�true��Ԃ�
};