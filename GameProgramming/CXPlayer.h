#ifndef CXPLAYER_H
#define CXPLAYER_H

#include "CXCharacter.h"
#include "CCollider.h"
#include "CText.h"
#include "CTexture.h"

class CXPlayer : public CXCharacter
{
private:
	CText mFont;
	CTexture mTexture;
	CTexture mItemTexture;
	CTexture mItemTexture2;

	//コライダの宣言
	CCollider mColSphereBody;	//体
	CCollider mColSphereHead;	//頭
	CCollider mColSphereSword;	//剣

	static CXPlayer* mInstance;

	int mHp;		//体力
	int mStamina;	//スタミナ
	float mSpeed;	//スピード

	CVector mMoveKeep;		//Moveを保存する

	bool mAvoid;			//回避状態の時にtrueを返す
	int mAvoidTime;			//回避時間
	float mAvoidSpeed;		//回避中のスピード

	int mInvincibleTime;	//無敵時間
	bool mInvincibleFlag;	//無敵状態の時trueを返す

	bool mAttackFlag_1;		//攻撃1状態の時trueを返す
	bool mAttackFlag_2;		//攻撃2状態の時trueを返す
	float mAttack2Speed;	//攻撃2の時のスピード
	bool mAttackFlag_3;		//攻撃3状態の時trueを返す

	int mGraceTime;			//派生攻撃の受付時間
	int mCombo;				//派生攻撃を連続で行った回数

	CVector mMoveDir;	
	CVector mMove2;

	CVector mMoveSide;
	CVector mMoveFront;

	CVector mSideVec;
	CVector mFrontVec;
	float mTurnspeed;

	enum EItem {
		HEAD = 0,
		ETRAP,		//罠
		EPORTION,	//回復薬
		TAIL,
	};
	int mItemSelect;		//選択中のアイテム
	int mPortionQuantity;	//回復薬の所持数

	void Idle();		//待機処理
	void Move();		//移動処理
	void Dash();		//ダッシュ処理
	void Attack_1();	//攻撃1処理
	void Attack_2();	//攻撃2処理
	void Attack_3();	//攻撃3処理
	void Avoid();		//回避処理
	void Death();		//死亡処理
	void ItemUse();		//アイテム使用処理
	void ItemSelect();	//アイテム選択処理
	void KnockBack();	//ノックバック処理
public:
	CXPlayer();

	/*
	初期化(Xモデルクラスのポインタ)
	*/
	void Init(CModelX* model);

	void Update();
	void Render2D();

	//衝突処理
	//m:自分のコライダ o:相手のコライダ
	void Collision(CCollider* m, CCollider* o);

	static CXPlayer* GetInstance();

	//プレイヤーの状態
	enum EPlayerState
	{
		EIDLE,		//待機
		EMOVE,		//移動
		EDASH,		//ダッシュ
		EAVOID,		//回避
		EATTACK_1,	//攻撃1
		EATTACK_2,	//攻撃2
		EATTACK_3,	//攻撃3(攻撃1から派生)
		EDEATH,		//死亡
		EITEMUSE,	//アイテム使用
		EKNOCKBACK,	//ノックバック
	};
	EPlayerState mState;

	bool mHit;	//攻撃時にtrueを返す　敵に攻撃が当たるor攻撃終了時にfalseを返す
	bool mAttackFlag_Once; //攻撃した瞬間だけtrueを返す
};

#endif
