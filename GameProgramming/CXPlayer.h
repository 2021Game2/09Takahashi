#ifndef CXPLAYER_H
#define CXPLAYER_H

#include "CXCharacter.h"
#include "CCollider.h"
#include "CText.h"
#include "CTexture.h"
#include "CXEnemy.h"

//プレイヤークラス
class CXPlayer : public CXCharacter
{
private:
	CXEnemy* mpTargetEnemy;	//攻撃対象の敵を入れる

	CText mFont; //フォント
	CTexture mImageGauge;	//ゲージ画像
	CTexture mImagePortion;	//回復薬画像
	CTexture mImageTrap;	//罠画像
	CTexture mImageNixsign; //禁止マーク画像

	//コライダの宣言
	CCollider mColSphereBody;	//体
	CCollider mColSphereHead;	//頭
	CCollider mColSphereSword;	//剣

	static CXPlayer* mInstance;

	int mHp;		//体力
	int mStamina;	//スタミナ
	float mSpeed;	//スピード

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
	int mComboCount;		//派生攻撃を連続で行った回数

	CVector mMove;			//方向と速度をかけ合わせたベクトル
	CVector mMoveDir;		//移動する方向、モデルの回転にも使用している、毎フレームリセットされる
	CVector mMoveDirKeep;	//移動時の方向を保存する
	CVector mKnockBackDir;	//ノックバックする方向を保持する
	CVector mAttackDir;		//攻撃時の方向を保持する

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
	bool mIsItemUse();		//アイテムが使用可能な時にtrueを返す

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
	~CXPlayer();

	/*
	初期化(Xモデルクラスのポインタ)
	*/
	void Init(CModelX* model);

	void Update();
	void Render2D();

	//衝突処理
	//m:自分のコライダ o:相手のコライダ
	void CXPlayer::Collision(CCollider* m, CCollider* o);
	void CXPlayer::TaskCollision();

	static void Generate();	//生成
	static void Release();	//解放
	static CXPlayer* GetInstance(); //インスタンスを取得

	//プレイヤーの状態
	enum EPlayerState
	{
		EIDLE = 0,	//待機
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

	CVector GetSwordColPos();	//剣のコライダの座標を取得する
};

#endif
