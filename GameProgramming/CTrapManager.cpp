#include "CTrapManager.h"
#include "CTaskManager.h"

CTrapManager* CTrapManager::mInstance;

CModel CTrapManager::sTrap;

CTrapManager::CTrapManager()
{
	sTrap.Load("trap.obj", "trap.mtl");
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
	//ã©¶¬
	CTrap* trap = new CTrap;
	trap->SetPos(pos);
	trap->SetRot(rot);
	trap->Update();
	mTrapList.push_back(trap);
}
