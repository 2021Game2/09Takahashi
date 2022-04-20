#pragma once
#include "CXEnemy.h"

class CEnemyManager 
{
private:
	static CEnemyManager* mInstance; //インスタンス

	std::vector<CXEnemy*> mEnemyList; //格納

	CXEnemy* mNearTarget; //一番近い敵を格納

	int mEnemyDeathNum; //死亡状態の敵の数
public:
	CEnemyManager();
	~CEnemyManager();

	static void Generate(); //生成
	static void Release(); //破棄
	static CEnemyManager* GetInstance(); //インスタンス取得

	void EnemyGenerate(int num); //敵生成、引数で生成数を指定する
	CXEnemy* GetNearEnemy(); //一番近い敵を取得する

	void Update();
	void Render();

	void AIUpdate();

	bool mIsEnemyAllDeath(); //敵が全て死亡状態になるとtrueを返す
};