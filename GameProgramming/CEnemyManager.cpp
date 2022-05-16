#include "CEnemyManager.h"
#include "CRes.h"
#include "CXPlayer.h"

#define MODEL_ENEMY "Resource\\knight\\knight_low.x" //敵モデル

#define ATTACK_NUM_MAX 1 //同時に攻撃できる敵の数

CEnemyManager* CEnemyManager::mInstance;

CEnemyManager::CEnemyManager()
	:mNearTarget(nullptr)
	,mEnemyDeathNum(0)
	,mEnemyAttackNum(0)
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
	CRes::sKnight.SeparateAnimationSet(0, 170, 220, "dash");//14:ダッシュ
	CRes::sKnight.SeparateAnimationSet(0, 380, 430, "jump");//15:ジャンプ

	EnemyGenerate(ENEMY_GENERATE_NUM); //敵を生成する
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
		tPos.mX = mEnemyStartPos[i].mX;
		tPos.mZ = mEnemyStartPos[i].mZ;

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
}

void CEnemyManager::Render()
{
}

void CEnemyManager::AIUpdate()
{
	//リセット
	mEnemyDeathNum = 0;
	mEnemyAttackNum = 0;
	float len1 = 10000.0f;

	for (size_t i = 0; i < mEnemyList.size(); i++) {
		if (i == 0) {
			mNearTarget = mEnemyList[i];
		}
		mEnemyList[i]->mIsTarget = false;
		//死亡状態だった時
		if (mEnemyList[i]->mIsDeath()) {
			mEnemyDeathNum++; //カウント加算
			continue; //読み飛ばし
		}
		//攻撃状態だったとき攻撃状態の敵の数を加算
		if (mEnemyList[i]->mIsAttack())mEnemyAttackNum++;
		//プレイヤーとの距離が一番近い敵を求める
		CVector pos = mEnemyList[i]->mPosition - CXPlayer::GetInstance()->mPosition;
		float len2 = pos.Length();
		if (len1 > len2) {
			len1 = len2;
			mNearTarget = mEnemyList[i];
		}
	}

	if (mIsEnemyAllDeath() == false) {
		mNearTarget->mIsTarget = true; //プレイヤーに一番近い敵の攻撃対象フラグをtrueにする
	}
}

bool CEnemyManager::mIsEnemyAllDeath()
{
	return(mEnemyList.size() == mEnemyDeathNum);
}

bool CEnemyManager::mIsEnemyAttack()
{
	return (mEnemyAttackNum < ATTACK_NUM_MAX);
}



