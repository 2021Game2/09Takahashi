#ifndef CXENEMY_H
#define CXENEMY_H

#include "CXCharacter.h"
#include "CCollider.h"

//エネミー(敵)のベースクラス
class CXEnemy : public CXCharacter
{
public:
	//敵の種類
	enum EEnemyType {
		ETYPE_1 = 0,	//タイプ1
		ETYPE_2			//タイプ2
	};

	//敵の状態
	enum EEnemyState
	{
		EIDLE = 0,		//待機
		EAUTOMOVE,		//移動
		ECHASE,			//追跡
		EATTACK_IDLE,	//攻撃待機
		EATTACK_1,		//攻撃1
		EATTACK_2,		//攻撃2
		EKNOCKBACK,		//ノックバック
		EDEATH,			//死亡
		ESTUN,			//スタン
		EAVOID,			//回避
	};
protected:
	int mHp;		//体力
	int mHpMax;		//体力最大値

	int mStunTime;	//罠にかかった時のスタンする時間

	CVector mPoint;			//移動時の目標地点
	CVector mPlayerPoint;	//追跡時の目標地点
	float mPlayerDis;		//プレイヤーまでの距離

	int mChaseRestartCount;	//プレイヤーが離れたときに、追跡再開するまでのカウント

	CVector mMoveDir;		//移動する方向
	float mSpeed;			//移動する速度

	CVector mRot;			//回転
	float mDot;				//内積

	bool mIsInvincible;		//無敵フラグ、プレイヤーの攻撃がヒットするとtrueを返す

	virtual void Idle();		//待機処理
	virtual void AutoMove();	//移動処理
	virtual void Chase();		//追跡処理
	virtual void Attack_Idle();	//攻撃待機処理
	virtual void Attack_1();	//攻撃1処理
	virtual void Attack_2();	//攻撃2処理
	virtual void KnockBack();	//ノックバック処理
	virtual void Death();		//死亡処理
	virtual void Stun();		//スタン処理
	virtual void Avoid();		//回避処理

	EEnemyType mEnemyType;	//敵の種類判断用
	EEnemyState mState;		//敵の状態判断用
public:
	CXEnemy();
	/*
	初期化(Xモデルクラスのポインタ)
	*/
	virtual void Init(CModelX* model);

	//衝突処理
	//m:自分のコライダ o:相手のコライダ
	void CXEnemy::Collision(CCollider* m, CCollider* o);
	void CXEnemy::TaskCollision();

	void Update();		//更新
	void Render2D();	//2D描画

	void SetPos(CVector hpos);	//位置を設定
	CVector GetPos();			//位置を取得

	void SetHp(int hp);			//体力を設定

	bool mHit;	//攻撃時にtrueを返す　敵に攻撃が当たるor攻撃終了時にfalseを返す

	bool mIsTarget; //プレイヤーの攻撃時の対象になっている時trueを返す

	CXEnemy::EEnemyState GetState();	//敵の状態を取得

	bool mIsAttack();	//攻撃状態の時にtrueを返す
	bool mIsDeath();	//死亡状態の時にtrueを返す
	bool mIsKnockBack();//ノックバックが可能なときtrueを返す
};

#endif
