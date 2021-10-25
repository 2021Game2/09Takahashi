#ifndef CXPLAYER_H
#define CXPLAYER_H

#include "CXCharacter.h"
#include "CCollider.h"

class CXPlayer : public CXCharacter
{
private:
	int mStamina;	//スタミナ
	float mSpeed;	//スピード

	bool mAvoid;	//回避状態の時にtrueを返す
	int mAvoidTime;	//回避時間
	float mAvoidSpeed;	//回避中のスピード
	CVector mAvoidMove;	//回避時のMoveを保存

	int mInvincibleTime;	//無敵時間
	bool mInvincible;	//無敵状態の時trueを返す

	bool mAttackFlag_1;	//攻撃1状態の時trueを返す

	CVector mMove;	
	CVector mMove2;
	CVector mSideVec;
	CVector mFrontVec;
	float mTurnspeed;

	void Idle();		//待機処理
	void Move();		//移動処理
	void Dash();		//ダッシュ処理
	void Attack_1();	//攻撃1処理
	void Avoid();		//回避処理

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

	//衝突処理
	//m:自分のコライダ o:相手のコライダ
	void Collision(CCollider* m, CCollider* o);

	//プレイヤーの状態
	enum EPlayerState
	{
		EIDLE,		//待機
		EMOVE,		//移動
		EDASH,		//ダッシュ
		EAVOID,		//回避
		EATTACK_1,	//攻撃1
	};
	EPlayerState mState;
};

#endif
