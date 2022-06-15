#pragma once

#include "CXEnemy.h"

//エネミー(敵)クラス2
class CXEnemy2 :public CXEnemy {
public:
	CXEnemy2();
	~CXEnemy2();

	//コライダ
	CCollider mColSphereBody;	//体
	CCollider mColSphereHead;	//頭
	CCollider mColSphereSword0;	//剣
	CCollider mColSphereSword1;	//剣

	void Init(CModelX* model);		//初期化
	void CXEnemy2::TaskCollision();	//衝突処理を呼ぶ

	void Idle();		//待機処理
	void AutoMove();	//移動処理
	void Chase();		//追跡処理
	void Attack_Idle();	//攻撃待機処理
	void Attack_1();	//攻撃1処理
	void Attack_2();	//攻撃2処理
	void KnockBack();	//ノックバック処理
	void Death();		//死亡処理
	void Stun();		//スタン処理
};