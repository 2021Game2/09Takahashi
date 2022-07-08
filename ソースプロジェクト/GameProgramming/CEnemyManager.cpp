#include "CEnemyManager.h"
#include "CRes.h"
#include "CXPlayer.h"
#include "CKey.h"
#include "CXEnemy1.h"
#include "CXEnemy2.h"

#define ATTACK_NUM_MAX 1 //“¯‚ÉUŒ‚‚Å‚«‚é“G‚Ì”

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

//“G‚Ì¶¬ˆ—
//ˆø”‚Å¶¬”Aí—ŞA‘Ì—Í‚ğİ’è‚·‚é
void CEnemyManager::EnemyGenerate(int num, int type, int hp,CVector pos[])
{
	for (int i = 0; i < num; i++) {
		CVector tPos;
		tPos.Set(0, 0, 0);
		tPos.mX = pos[i].mX;
		tPos.mZ = pos[i].mZ;

		//¶¬‚·‚é“G‚Ìí—Ş‚ğ”»•Ê
		switch (type) {
		case CXEnemy::ETYPE_1:	//ƒ^ƒCƒv1
		{
			CXEnemy1* tmp = new CXEnemy1;
			tmp->SetPos(tPos);	//À•Wİ’è
			tmp->SetHp(hp);		//‘Ì—Í‚ğİ’è
			tmp->Update();		//XV
			mEnemyList.push_back(tmp);	//ƒŠƒXƒg‚É’Ç‰Á
		}
		break;

		case CXEnemy::ETYPE_2:	//ƒ^ƒCƒv2
		{
			CXEnemy2* tmp = new CXEnemy2;
			tmp->SetPos(tPos);	//À•Wİ’è
			tmp->SetHp(hp);		//‘Ì—Í‚ğİ’è
			tmp->Update();		//XV
			mEnemyList.push_back(tmp);	//ƒŠƒXƒg‚É’Ç‰Á
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
	//ƒŠƒZƒbƒg
	mEnemyDeathNum = 0;
	mEnemyAttackNum = 0;
	float score1 = -10000.0f;

	for (size_t i = 0; i < mEnemyList.size(); i++) {
		if (i == 0) {
			mTargetEnemy = nullptr;
		}
		mEnemyList[i]->SetIsTarget(false);
		//€–Só‘Ô‚¾‚Á‚½
		if (mEnemyList[i]->mIsDeath()) {
			mEnemyDeathNum++; //€–Só‘Ô‚Ì“G‚ÌƒJƒEƒ“ƒg‰ÁZ
			continue; //“Ç‚İ”ò‚Î‚µ
		}
		//UŒ‚ó‘Ô‚¾‚Á‚½‚Æ‚«UŒ‚ó‘Ô‚Ì“G‚Ì”‚ğ‰ÁZ
		if (mEnemyList[i]->mIsAttack())mEnemyAttackNum++;

		float score2 = mEnemyList[i]->GetScore();
		if (score1 < score2) {
			score1 = score2;
			mTargetEnemy = mEnemyList[i]; //ƒXƒRƒA‚ª‚‚¢“G‚ğŠi”[‚·‚é
		}
	}

	if (mTargetEnemy) {
		mTargetEnemy->SetIsTarget(true); //ƒXƒRƒA‚ªˆê”Ô‚‚¢“G‚ÌUŒ‚‘ÎÛƒtƒ‰ƒO‚ğtrue‚É‚·‚é
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



