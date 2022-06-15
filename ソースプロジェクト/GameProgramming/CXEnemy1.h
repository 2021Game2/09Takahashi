#pragma once

#include "CXEnemy.h"

//エネミー(敵)クラス1
class CXEnemy1 :public CXEnemy {
public:
	CXEnemy1();
	~CXEnemy1();

	//コライダ
	CCollider mColSphereBody;	//体
	CCollider mColSphereHead;	//頭
	CCollider mColSphereSword0;	//剣
	CCollider mColSphereSword1;	//剣
	CCollider mColSphereSword2;	//剣

	void Init(CModelX* model);		//初期化
	void CXEnemy1::TaskCollision();	//衝突処理を呼ぶ
private:

};
