#pragma once
#include "CXEnemy.h"

#define ENEMY_GENERATE_NUM 3 //“G‚Ì¶¬”

#define ENEMY_START_POS CVector(0.0f, 0.0f, -10.0f),CVector(10.0f, 0.0f, 0.0f),CVector(-10.0f, 0.0f, 0.0f) //“G‚Ì‰ŠúÀ•W1

//“G‚ğŠÇ—‚·‚éƒNƒ‰ƒX
class CEnemyManager
{
private:
	static CEnemyManager* mInstance; //“GŠÇ—‚ÌƒCƒ“ƒXƒ^ƒ“ƒX

	std::vector<CXEnemy*> mEnemyList; //“GŠi”[

	CXEnemy* mNearTarget; //ˆê”Ô‹ß‚¢“G‚ğŠi”[

	int mEnemyDeathNum; //€–Só‘Ô‚Ì“G‚Ì”

	int mEnemyAttackNum; //UŒ‚ó‘Ô‚Ì“G‚Ì”

	CVector mEnemyStartPos[ENEMY_GENERATE_NUM] = { ENEMY_START_POS }; //“G‚Ì‰ŠúÀ•W
public:
	CEnemyManager();
	~CEnemyManager();

	static void Generate(); //¶¬
	static void Release(); //”jŠü
	static CEnemyManager* GetInstance(); //ƒCƒ“ƒXƒ^ƒ“ƒXæ“¾

	void EnemyGenerate(int num, int type, int hp); //“G¶¬Aˆø”‚Å¶¬”‚Æ“G‚Ìí—Ş‚Æ‰Šú‘Ì—Í(È—ª‰Â)‚ğw’è‚·‚é
	CXEnemy* GetNearEnemy(); //ˆê”Ô‹ß‚¢“G‚ğæ“¾‚·‚é

	void Update();
	void Render();

	void AIUpdate(); //AIXV

	bool mIsEnemyAllDeath(); //“G‚ª‘S‚Ä€–Só‘Ô‚É‚È‚é‚Ætrue‚ğ•Ô‚·

	bool mIsEnemyAttack(); //“G‚ªUŒ‚‰Â”\‚È‚Æ‚«true‚ğ•Ô‚·
};