#include "CEnemyManager.h"
#include "CRes.h"
#include "CXPlayer.h"
#include "CKey.h"
#include "CXEnemy1.h"
#include "CXEnemy2.h"

#define ATTACK_NUM_MAX 1 //“¯‚ÉUŒ‚‚Å‚«‚é“G‚Ì”

CEnemyManager* CEnemyManager::mInstance;

CEnemyManager::CEnemyManager()
	:mNearTarget(nullptr)
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

void CEnemyManager::EnemyGenerate(int num,int type)
{
	for (int i = 0; i < num; i++) {
		CVector tPos;
		tPos.Set(0, 0, 0);
		tPos.mX = mEnemyStartPos[i].mX;
		tPos.mZ = mEnemyStartPos[i].mZ;

		//¶¬‚·‚é“G‚Ìí—Ş‚ğ”»•Ê
		switch (type) {
		case CXEnemy::ETYPE_1:
		{
			CXEnemy1* tmp = new CXEnemy1;
			tmp->SetPos(tPos);
			tmp->Update();
			mEnemyList.push_back(tmp);
		}
			break;

		case CXEnemy::ETYPE_2:
		{
			CXEnemy2* tmp = new CXEnemy2;
			tmp->SetPos(tPos);
			tmp->Update();
			mEnemyList.push_back(tmp);
		}
			break;
		}
	}
}

CXEnemy* CEnemyManager::GetNearEnemy()
{
	return mNearTarget;
}

void CEnemyManager::Update()
{
	AIUpdate();

#ifdef _DEBUG
	//ƒeƒXƒg—pA“G‚ğˆê‘Ì¶¬‚·‚é
	if (CKey::Once('P')) {
		EnemyGenerate(1, 1);
	}
#endif
}

void CEnemyManager::Render()
{
}

void CEnemyManager::AIUpdate()
{
	//ƒŠƒZƒbƒg
	mEnemyDeathNum = 0;
	mEnemyAttackNum = 0;
	float len1 = 10000.0f;

	for (size_t i = 0; i < mEnemyList.size(); i++) {
		if (i == 0) {
			mNearTarget = nullptr;
		}
		mEnemyList[i]->mIsTarget = false;
		//€–Só‘Ô‚¾‚Á‚½
		if (mEnemyList[i]->mIsDeath()) {
			mEnemyDeathNum++; //€–Só‘Ô‚Ì“G‚ÌƒJƒEƒ“ƒg‰ÁZ
			continue; //“Ç‚İ”ò‚Î‚µ
		}
		//UŒ‚ó‘Ô‚¾‚Á‚½‚Æ‚«UŒ‚ó‘Ô‚Ì“G‚Ì”‚ğ‰ÁZ
		if (mEnemyList[i]->mIsAttack())mEnemyAttackNum++;
		//ƒvƒŒƒCƒ„[‚Æ‚Ì‹——£‚ªˆê”Ô‹ß‚¢“G‚ğ‹‚ß‚é
		CVector pos = mEnemyList[i]->mPosition - CXPlayer::GetInstance()->mPosition;
		float len2 = pos.Length();
		if (len1 > len2) {
			len1 = len2;
			mNearTarget = mEnemyList[i]; //ƒvƒŒƒCƒ„[‚Æ‚Ì‹——£‚ªˆê”Ô‹ß‚¢“G‚ğŠi”[‚·‚é
		}
	}

	if (mNearTarget) {
		mNearTarget->mIsTarget = true; //ƒvƒŒƒCƒ„[‚Éˆê”Ô‹ß‚¢“G‚ÌUŒ‚‘ÎÛƒtƒ‰ƒO‚ğtrue‚É‚·‚é
	}
}

//“G‚ª‘S‚Ä€–Só‘Ô‚Ì‚Æ‚«true‚ğ•Ô‚·
bool CEnemyManager::mIsEnemyAllDeath()
{
	return(mEnemyList.size() == mEnemyDeathNum);
}

//UŒ‚ó‘Ô‚Ì“G‚Ì”‚ª“¯‚ÉUŒ‚‚Å‚«‚é“G‚Ì”‚ğ’´‚¦‚Ä‚¢‚È‚¯‚ê‚Îtrue‚ğ•Ô‚·
bool CEnemyManager::mIsEnemyAttack()
{
	return (mEnemyAttackNum < ATTACK_NUM_MAX);
}



