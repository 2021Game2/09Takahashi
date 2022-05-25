#pragma once
#include "CXEnemy.h"

#define ENEMY_GENERATE_NUM 3 //敵の生成数

#define ENEMY_START_POS CVector(10.0f, 0.0f, 0.0f),CVector(-10.0f, 0.0f, 0.0f),CVector(0.0f, 0.0f, -10.0f) //敵の初期座標

//敵を管理するクラス
class CEnemyManager
{
private:
	static CEnemyManager* mInstance; //敵管理のインスタンス

	std::vector<CXEnemy*> mEnemyList; //敵格納

	CXEnemy* mNearTarget; //一番近い敵を格納

	int mEnemyDeathNum; //死亡状態の敵の数

	int mEnemyAttackNum; //攻撃状態の敵の数

	CVector mEnemyStartPos[ENEMY_GENERATE_NUM] = { ENEMY_START_POS }; //敵の初期座標
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

	void AIUpdate(); //AI更新

	bool mIsEnemyAllDeath(); //敵が全て死亡状態になるとtrueを返す

	bool mIsEnemyAttack(); //敵が攻撃可能なときtrueを返す
};