#include "CTrapManager.h"
#include "CTaskManager.h"

#define TRAP_QUANTITY 2			//ã©‚ÌŠ”

CTrapManager* CTrapManager::mInstance;

CModel CTrapManager::sTrap;

CTrapManager::CTrapManager()
	:mMapTrap(false)
	, mTrapQuantity(TRAP_QUANTITY)
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
	//ƒ}ƒbƒvã‚Éã©‚ª‚È‚¢‚Æ‚«&&ã©‚ÌŠ”‚ª0‚æ‚è‘½‚¢‚Æ‚«
	if (mMapTrap == false && mTrapQuantity > 0) {
		//ã©¶¬
		CTrap* trap = new CTrap;
		trap->SetPos(pos);
		trap->SetRot(rot);
		trap->Update();
		mMapTrap = true;
		mTrapQuantity--;	//ã©‚ÌŠ”‚ğŒ¸‚ç‚·
		mTrapList.push_back(trap);
	}
}
