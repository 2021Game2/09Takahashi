#include "CTrapManager.h"
#include "CTaskManager.h"
#include "CXPlayer.h"

#define MAP_TRAP_MAX 3 //マップ上に設置できる罠の上限

CTrapManager* CTrapManager::mInstance;

CTrapManager::CTrapManager()
	:mMapTrapNum(0)
{
}

CTrapManager::~CTrapManager()
{
	for (size_t i = 0; i < mTrapList.size(); i++) {
		delete mTrapList[i];
	}

	//プレイヤーのインスタンスがあるとき
	if (CXPlayer::GetInstance()) {
		//マップ上に有効な罠があるとき
		if (mMapTrapNum > 0) {
			//罠の所持数を加算
			CXPlayer::GetInstance()->SetTrapQuantity(mMapTrapNum, true);
		}
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
	//リセット
	mMapTrapNum = 0;

	for (size_t i = 0; i < mTrapList.size(); i++) {
		//敵と衝突したか判断するフラグがfalse
		if (mTrapList[i]->GetIsEnemyCol() == false) {
			//マップ上の有効な罠の数をカウントアップ
			mMapTrapNum++;
		}
	}
}

void CTrapManager::TrapGenerate(CVector pos, CVector rot)
{
	//罠生成
	CTrap* trap = new CTrap;
	trap->SetPos(pos);
	trap->SetRot(rot);
	trap->Update();
	mTrapList.push_back(trap);
}

bool CTrapManager::TrapAvailable()
{
	//罠の所持数が0を上回っているとき、マップ上の有効な罠の数が設置できる上限を超えていないとき
	return (CXPlayer::GetInstance()->GetTrapQuantity() > 0 && mMapTrapNum < MAP_TRAP_MAX);
}
