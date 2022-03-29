#include "CTrapManager.h"
#include "CTaskManager.h"

#define MODEL_TRAP "Resource\\trap.obj", "Resource\\trap.mtl" //㩃��f��
#define TRAP_QUANTITY 2			//㩂̏�����

CTrapManager* CTrapManager::mInstance;

CModel CTrapManager::sTrap;

CTrapManager::CTrapManager()
	:mMapTrap(false)
	, mTrapQuantity(TRAP_QUANTITY)
{
	sTrap.Load(MODEL_TRAP);
}

CTrapManager::~CTrapManager()
{
	for (size_t i = 0; i < mTrapList.size(); i++) {
		delete mTrapList[i];
	}
}

void CTrapManager::Generate()
{
	mInstance = new CTrapManager;
}

void CTrapManager::Release()
{
	if (mInstance) {
		delete mInstance;
		mInstance = NULL;
	}
}

CTrapManager* CTrapManager::GetInstance()
{
	return mInstance;
}

void CTrapManager::Update()
{
	for (size_t i = 0; i < mTrapList.size(); i++) {
		mTrapList[i]->Update();
	}
}

void CTrapManager::TrapGenerate(CVector pos, CVector rot)
{
	//�}�b�v���㩂��Ȃ��Ƃ�&&㩂̏�������0��葽���Ƃ�
	if (mMapTrap == false && mTrapQuantity > 0) {
		//㩐���
		CTrap* trap = new CTrap;
		trap->SetPos(pos);
		trap->SetRot(rot);
		trap->Update();
		mMapTrap = true;
		mTrapQuantity--;	//㩂̏����������炷
		mTrapList.push_back(trap);
	}
}