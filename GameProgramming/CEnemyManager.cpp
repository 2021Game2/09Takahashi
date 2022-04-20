#include "CEnemyManager.h"
#include "CRes.h"
#include "CXPlayer.h"
#include <time.h>

#define MODEL_ENEMY "Resource\\knight\\knight_low.x" //敵モデル

CEnemyManager* CEnemyManager::mInstance;

CEnemyManager::CEnemyManager()
	:mNearTarget(nullptr)
	,mEnemyDeathNum(0)
{
	CRes::sKnight.Load(MODEL_ENEMY);
	CRes::sKnight.SeparateAnimationSet(0, 10, 80, "walk");//1:移動
	CRes::sKnight.SeparateAnimationSet(0, 1530, 1830, "idle1");//2:待機
	CRes::sKnight.SeparateAnimationSet(0, 10, 80, "walk");//3:ダミー
	CRes::sKnight.SeparateAnimationSet(0, 10, 80, "walk");//4:ダミー
	CRes::sKnight.SeparateAnimationSet(0, 10, 80, "walk");//5:ダミー
	CRes::sKnight.SeparateAnimationSet(0, 10, 80, "walk");//6:ダミー
	CRes::sKnight.SeparateAnimationSet(0, 440, 520, "attack1_1");//7:Attack1
	CRes::sKnight.SeparateAnimationSet(0, 520, 615, "attack2");//8:Attack2
	CRes::sKnight.SeparateAnimationSet(0, 10, 80, "walk");//9:ダミー
	CRes::sKnight.SeparateAnimationSet(0, 10, 80, "walk");//10:ダミー
	CRes::sKnight.SeparateAnimationSet(0, 1160, 1260, "death1");//11:ダウン
	CRes::sKnight.SeparateAnimationSet(0, 90, 160, "knockback");//12:ノックバック
	CRes::sKnight.SeparateAnimationSet(0, 1120, 1160, "stun");//13:スタン
	CRes::sKnight.SeparateAnimationSet(0, 170, 220, "Dash");//14:ダッシュ
	CRes::sKnight.SeparateAnimationSet(0, 380, 430, "Jump");//15:ジャンプ

	srand((unsigned)time(NULL)); //乱数用
}

CEnemyManager::~CEnemyManager()
{
	for (size_t i = 0; i < mEnemyList.size(); i++) {
		delete mEnemyList[i];
	}
}

void CEnemyManager::Generate()
{
	mInstance = new CEnemyManager;
}

void CEnemyManager::Release()
{
	if (mInstance) {
		delete mInstance;
		mInstance = nullptr;
	}
}

CEnemyManager* CEnemyManager::GetInstance()
{
	return mInstance;
}

void CEnemyManager::EnemyGenerate(int num)
{
	for (int i = 0; i < num; i++) {
		CVector tPos;
		tPos.Set(0, 0, 0);
		//		tPos.mY += -3.0f + (float)(rand() % 6);
		tPos.mX += -30.0f + (float)(rand() % 60);
		tPos.mZ += -30.0f + (float)(rand() % 60);

		CXEnemy* tmp = new CXEnemy;
		tmp->SetPos(tPos);
		tmp->Update();
		mEnemyList.push_back(tmp);
	}
}

CXEnemy* CEnemyManager::GetNearEnemy()
{
	return mNearTarget;
}

void CEnemyManager::Update()
{
	AIUpdate();

	/*
	for (size_t i = 0; i < mEnemyList.size(); i++) {
		mEnemyList[i]->Update();
	}
	*/
}

void CEnemyManager::Render()
{
	/*
	for (size_t i = 0; i < mEnemyList.size(); i++) {
		mEnemyList[i]->Render();
	}
	*/
}

void CEnemyManager::AIUpdate()
{
	int DeathCount = 0;
	float len1 = 10000.0f;

	for (size_t i = 0; i < mEnemyList.size(); i++) {
		if (i == 0) {
			mNearTarget = mEnemyList[i];
		}
		//死亡状態だった時
		if (mEnemyList[i]->mState == CXEnemy::EDEATH) {
			DeathCount++; //カウント加算
		}
		mEnemyList[i]->mIsTarget = false;
		CVector pos = mEnemyList[i]->mPosition - CXPlayer::GetInstance()->mPosition;
		float len2 = pos.Length();
		if (len1 > len2) {
			len1 = len2;
			mNearTarget = mEnemyList[i];
		}
	}

	mNearTarget->mIsTarget = true;
	mEnemyDeathNum = DeathCount;
}

bool CEnemyManager::mIsEnemyAllDeath()
{
	return(mEnemyList.size() == mEnemyDeathNum);
}



