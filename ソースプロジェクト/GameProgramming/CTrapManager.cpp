#include "CTrapManager.h"
#include "CTaskManager.h"
#include "CXPlayer.h"

CTrapManager* CTrapManager::mInstance;

CTrapManager::CTrapManager()
	:mMapTrap(false)
{
}

CTrapManager::~CTrapManager()
{
	for (size_t i = 0; i < mTrapList.size(); i++) {
		delete mTrapList[i];
	}
	//マップ上に罠が残っていた時
	if (mMapTrap == true) {
		//罠の所持数を加算
		CXPlayer::GetInstance()->SetTrapQuantity(1, true);
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
	/*
	for (size_t i = 0; i < mTrapList.size(); i++) {
		mTrapList[i]->Update();
	}
	*/
}

void CTrapManager::TrapGenerate(CVector pos, CVector rot)
{
	//罠生成
	CTrap* trap = new CTrap;
	trap->SetPos(pos);
	trap->SetRot(rot);
	trap->Update();
	mMapTrap = true;
	mTrapList.push_back(trap);
}

void CTrapManager::SetMapTrapFlag(bool flag)
{
	mMapTrap = flag;
}

bool CTrapManager::TrapAvailable()
{
	//罠の所持数が0を上回っているとき、マップ上にトラップが置かれていないときtrueを返す
	return (CXPlayer::GetInstance()->GetTrapQuantity() > 0 && mMapTrap == false);
}
