#ifndef CXPLAYER_H
#define CXPLAYER_H

#include "CXCharacter.h"
#include "CCollider.h"

class CXPlayer : public CXCharacter
{
private:
	int mHp;		//体力
	int mStamina;	//スタミナ
	float mSpeed;	//スピード

	CVector mMoveKeep;	//Moveを保存する

	bool mAvoid;	//回避状態の時にtrueを返す
	int mAvoidTime;	//回避時間
	float mAvoidSpeed;	//回避中のスピード

	int mInvincibleTime;	//無敵時間
	bool mInvincibleFlag;	//無敵状態の時trueを返す

	bool mAttackFlag_1;	//攻撃1状態の時trueを返す
	bool mAttackFlag_2;	//攻撃2状態の時trueを返す
	float mAttack2Speed;	//攻撃2の時のスピード
	bool mAttackFlag_3;	//攻撃3状態の時trueを返す

	CVector mMove;	
	CVector mMove2;
	CVector mSideVec;
	CVector mFrontVec;
	float mTurnspeed;

	void Idle();		//待機処理
	void Move();		//移動処理
	void Dash();		//ダッシュ処理
	void Attack_1();	//攻撃1処理
	void Attack_2();	//攻撃2処理
	void Attack_3();	//攻撃3処理
	void Avoid();		//回避処理
	void Death();		//死亡処理

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
		EATTACK_2,	//攻撃2
		EATTACK_3,	//攻撃3
		EDEATH,		//死亡
	};
	EPlayerState mState;
};

#endif
