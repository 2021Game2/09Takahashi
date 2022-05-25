#ifndef CXENEMY_H
#define CXENEMY_H

#include "CXCharacter.h"
#include "CCollider.h"

//エネミー(敵)クラス
class CXEnemy : public CXCharacter
{
private:
	//コライダの宣言
	CCollider mColSphereBody;	//体
	CCollider mColSphereHead;	//頭
	CCollider mColSphereSword0;	//剣
	CCollider mColSphereSword1;	//剣
	CCollider mColSphereSword2;	//剣

	int mHp;		//体力

	int mStunTime;	//罠にかかった時のスタンする時間

	CVector mPoint;			//移動時の目標地点
	CVector mPlayerPoint;	//追跡時の目標地点
	float mPlayerDis;		//プレイヤーまでの距離

	int mChaseRestartCount;	//プレイヤーが離れたときに、追跡再開するまでのカウント

	CVector mMoveDir;		//移動する方向
	float mSpeed;			//移動する速度

	CVector mRot;			//回転
	float mDot;				//内積

	bool mIsInvincible;	//無敵フラグ、プレイヤーの攻撃がヒットするとtrueを返す

	void Idle();		//待機処理
	void AutoMove();	//移動処理
	void Chase();		//追跡処理
	void Attack_Idle();	//攻撃待機処理
	void Attack_1();	//攻撃1処理
	void Attack_2();	//攻撃2処理
	void KnockBack();	//ノックバック処理
	void Death();		//死亡処理
	void Stun();		//スタン処理
	void Avoid();		//回避処理
public:

	CXEnemy();
	/*
	初期化(Xモデルクラスのポインタ)
	*/
	void Init(CModelX* model);

	//衝突処理
	//m:自分のコライダ o:相手のコライダ
	void CXEnemy::Collision(CCollider* m, CCollider* o);
	void CXEnemy::TaskCollision();

	void Update();
	void Render2D();

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
	EEnemyState mState;	//状態

	void SetPos(CVector hpos);	//位置を設定
	CVector GetPos();			//位置を取得

	bool mHit;	//攻撃時にtrueを返す　敵に攻撃が当たるor攻撃終了時にfalseを返す

	bool mIsTarget; //プレイヤーの攻撃時の対象になっている時trueを返す

	bool mIsAttack();	//攻撃状態の時にtrueを返す
	bool mIsDeath();	//死亡状態の時にtrueを返す
};

#endif
