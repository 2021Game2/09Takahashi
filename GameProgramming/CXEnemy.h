#ifndef CXENEMY_H
#define CXENEMY_H

#include "CXCharacter.h"
#include "CCollider.h"

class CXEnemy : public CXCharacter
{
private:
	CCollider mColSphereSword0;	//剣
	CCollider mColSphereSword1;	//剣
	CCollider mColSphereSword2;	//剣

	int mHp;	//体力

	bool mAttackFlag_1;	//攻撃1状態の時trueを返す

	void Idle();	//待機処理
	void Move();	//移動処理
	void Attack_1();	//攻撃1処理
	void Death();	//死亡処理
public:
	//コライダの宣言
	CCollider mColSphereBody;	//体
	CCollider mColSphereHead;	//頭
//	CCollider mColSphereSword;	//剣

	CXEnemy();
	/*
	初期化(Xモデルクラスのポインタ)
	*/
	void Init(CModelX* model);

	//衝突処理
	//m:自分のコライダ o:相手のコライダ
	void Collision(CCollider* m, CCollider* o);

	void Update();

	//敵の状態
	enum EEnemyState
	{
		EIDLE,		//待機
		EMOVE,		//移動
		EATTACK_1,	//攻撃1
		EDEATH,		//死亡
	};
	EEnemyState mState;
};

#endif
