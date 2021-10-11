#ifndef CXPLAYER_H
#define CXPLAYER_H

#include "CXCharacter.h"
#include "CCollider.h"

class CXPlayer : public CXCharacter
{
private:
	int mStamina;	//スタミナ
	bool mDash;		//ダッシュ状態の時にtrueを返す
	bool mAvoid;	//回避状態の時にtrueを返す
	int mAvoidTime;	//回避時間
	float mAvoidSpeed;	//回避中の移動速度


public:
	//コライダの宣言
	CCollider mColSphereBody;	//体
	CCollider mColSphereHead;	//頭
	CCollider mColSphereSword;	//剣

	CXPlayer();

	/*
	初期化(Xモデルクラスのポインタ)
	*/
	void Init(CModelX* model);

	void Update();
};

#endif
