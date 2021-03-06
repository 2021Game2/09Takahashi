#include "CEnemyManager.h"
#include "CRes.h"
#include "CXPlayer.h"
#include "CKey.h"
#include "CXEnemy1.h"
#include "CXEnemy2.h"

#define ATTACK_NUM_MAX 1 //同時に攻撃できる敵の数

CEnemyManager* CEnemyManager::mInstance;

CEnemyManager::CEnemyManager()
	:mTargetEnemy(nullptr)
	,mEnemyDeathNum(0)
	,mEnemyAttackNum(0)
{
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

//敵の生成処理
//引数で生成数、種類、体力を設定する
void CEnemyManager::EnemyGenerate(int num, int type, int hp,CVector pos[])
{
	for (int i = 0; i < num; i++) {
		CVector tPos;
		tPos.Set(0, 0, 0);
		tPos.mX = pos[i].mX;
		tPos.mZ = pos[i].mZ;

		//生成する敵の種類を判別
		switch (type) {
		case CXEnemy::ETYPE_1:	//タイプ1
		{
			CXEnemy1* tmp = new CXEnemy1;
			tmp->SetPos(tPos);	//座標設定
			tmp->SetHp(hp);		//体力を設定
			tmp->Update();		//更新
			mEnemyList.push_back(tmp);	//リストに追加
		}
		break;

		case CXEnemy::ETYPE_2:	//タイプ2
		{
			CXEnemy2* tmp = new CXEnemy2;
			tmp->SetPos(tPos);	//座標設定
			tmp->SetHp(hp);		//体力を設定
			tmp->Update();		//更新
			mEnemyList.push_back(tmp);	//リストに追加
		}
		break;
		}
	}
}

CXEnemy* CEnemyManager::GetTargetEnemy()
{
	return mTargetEnemy;
}

void CEnemyManager::Update()
{
	AIUpdate();
}

void CEnemyManager::Render()
{
}

void CEnemyManager::AIUpdate()
{
	//リセット
	mEnemyDeathNum = 0;
	mEnemyAttackNum = 0;
	float score1 = -10000.0f;

	for (size_t i = 0; i < mEnemyList.size(); i++) {
		if (i == 0) {
			mTargetEnemy = nullptr;
		}
		mEnemyList[i]->SetIsTarget(false);
		//死亡状態だった時
		if (mEnemyList[i]->mIsDeath()) {
			mEnemyDeathNum++; //死亡状態の敵のカウント加算
			continue; //読み飛ばし
		}
		//攻撃状態だったとき攻撃状態の敵の数を加算
		if (mEnemyList[i]->mIsAttack())mEnemyAttackNum++;

		float score2 = mEnemyList[i]->GetScore();
		if (score1 < score2) {
			score1 = score2;
			mTargetEnemy = mEnemyList[i]; //スコアが高い敵を格納する
		}
	}

	if (mTargetEnemy) {
		mTargetEnemy->SetIsTarget(true); //スコアが一番高い敵の攻撃対象フラグをtrueにする
	}
}

//敵が全て死亡状態のときtrueを返す
bool CEnemyManager::mIsEnemyAllDeath()
{
	return(mEnemyList.size() == mEnemyDeathNum);
}

//攻撃状態の敵の数が同時に攻撃できる敵の数を超えていなければtrueを返す
bool CEnemyManager::mIsEnemyAttack()
{
	return (mEnemyAttackNum < ATTACK_NUM_MAX);
}



