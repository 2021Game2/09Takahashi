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
	float mAvoidSpeed;	//回避中のスピード
	bool mMoveFlag;	//移動中trueを返す
	float mSpeed;	//スピード
	CVector mAvoidMove;	//回避時のMoveを保存
	int mInvincibleTime;	//無敵時間
	bool mInvincible;	//無敵状態の時trueを返す

	CVector mMove;	

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
