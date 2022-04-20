#pragma once
#include "CXEnemy.h"

class CEnemyManager 
{
private:
	static CEnemyManager* mInstance; //ƒCƒ“ƒXƒ^ƒ“ƒX

	std::vector<CXEnemy*> mEnemyList; //Ši”[

	CXEnemy* mNearTarget; //ˆê”Ô‹ß‚¢“G‚ğŠi”[

	int mEnemyDeathNum; //€–Só‘Ô‚Ì“G‚Ì”
public:
	CEnemyManager();
	~CEnemyManager();

	static void Generate(); //¶¬
	static void Release(); //”jŠü
	static CEnemyManager* GetInstance(); //ƒCƒ“ƒXƒ^ƒ“ƒXæ“¾

	void EnemyGenerate(int num); //“G¶¬Aˆø”‚Å¶¬”‚ğw’è‚·‚é
	CXEnemy* GetNearEnemy(); //ˆê”Ô‹ß‚¢“G‚ğæ“¾‚·‚é

	void Update();
	void Render();

	void AIUpdate();

	bool mIsEnemyAllDeath(); //“G‚ª‘S‚Ä€–Só‘Ô‚É‚È‚é‚Ætrue‚ğ•Ô‚·
};