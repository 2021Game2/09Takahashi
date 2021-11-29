#include "CXEnemy.h"
#include "CXPlayer.h"
#include "CUtil.h"
#define _USE_MATH_DEFINES
#include <math.h>

#define HP_MAX 1000	//体力
#define DAMAGE_BODY 10	//ダメージ(体)
#define DAMAGE_HEAD 20	//ダメージ(頭)
#define ATTACK_DIS 3.0f			//攻撃可能になる距離
#define SPEED_MOVE 0.05f		//通常移動のスピード
#define SPEED_CHASE 0.1f		//追跡中の移動速度
#define CHASE_DIS_MAX 20.0f		//追跡可能な最大距離
#define SEARCH_DIS 15.0f		//追跡を開始する距離

CXEnemy* CXEnemy::mInstance;

extern int EHp;	//敵の体力確認用、後で削除

CXEnemy::CXEnemy()
	: mColSphereBody(this, nullptr, CVector(0.5f, -1.0f, 0.0f), 1.2f)
	, mColSphereHead(this, nullptr, CVector(0.0f, 1.f, 0.0f), 1.2f)
	, mColSphereSword0(this, nullptr, CVector(0.7f, 3.5f, -0.2f), 0.5f)
	, mColSphereSword1(this, nullptr, CVector(0.5f, 2.5f, -0.2f), 0.5f)
	, mColSphereSword2(this, nullptr, CVector(0.3f, 1.5f, -0.2f), 0.5f)
	, mHp(HP_MAX)
	,mPoint(0.0f, 0.0f, 0.0f)
	,mAttackFlag_1(false)
	,mPlayerPoint(0.0f,0.0f,0.0f)
	,mPlayerDis(0.0f)
	,mMove(0.0f,0.0f,0.0f)
	,mRot(0.0f,0.0f,0.0f)
	,mSpeed(0.0f)
	,mDot(0.0f)
{
	mTag = EENEMY;

	mColSphereBody.mTag = CCollider::EBODY;
	mColSphereHead.mTag = CCollider::EHEAD;
	mColSphereSword0.mTag = CCollider::ESWORD;
	mColSphereSword1.mTag = CCollider::ESWORD;
	mColSphereSword2.mTag = CCollider::ESWORD;

	mState = EIDLE;	//待機

	mPoint = mPosition * mMatrixRotate;

	mInstance = this;
}

void CXEnemy::Init(CModelX* model)
{
	CXCharacter::Init(model);
	//合成行列の設定
	mColSphereBody.mpMatrix = &mpCombinedMatrix[1];
	//頭
	mColSphereHead.mpMatrix = &mpCombinedMatrix[1];
	//剣
	mColSphereSword0.mpMatrix = &mpCombinedMatrix[26];
	mColSphereSword1.mpMatrix = &mpCombinedMatrix[26];
	mColSphereSword2.mpMatrix = &mpCombinedMatrix[26];

	/*
	mColSphereBody.mpMatrix = &mpCombinedMatrix[8];
	//頭
	mColSphereHead.mpMatrix = &mpCombinedMatrix[11];
	//剣
	mColSphereSword.mpMatrix = &mpCombinedMatrix[21];
	*/
}

void CXEnemy::Update()
{
	mPlayerPoint = CXPlayer::GetInstance()->mPosition - mPosition;
	//プレイヤーまでの距離を求める
	mPlayerDis = mPlayerPoint.Length();

	switch (mState) {
	case EIDLE:	//待機状態
		Idle();	//待機処理を呼ぶ
		break;

	case EAUTOMOVE:	//移動状態
		AutoMove();	//移動処理を呼ぶ
		break;

	case ECHASE: //追跡状態
		Chase(); //追跡処理を呼ぶ
		break;

	case EATTACK_1:	//攻撃状態
		Attack_1();	//攻撃1処理を呼ぶ
		if (mAttackFlag_1 == false) {
			mState = EIDLE;
		}
		break;

	case EDAMAGED:
		Damaged();
		break;

	case EDEATH: //死亡状態
		Death(); //死亡処理を呼ぶ
		break;
	}

	Check tCheck = CUtil::GetCheck2D(mMove.mX, mMove.mZ, 0, 0, mRot.mY);
	float turnspeed = 0.5f;

	mDot = tCheck.dot;

	//急な振り返りを抑制
	if (tCheck.turn > 1.5f) tCheck.turn = 1.5f;

	//移動方向へキャラを向かせる
	if (tCheck.cross > 0.0f) {
		mRot.mY += tCheck.turn * turnspeed;
	}
	if (tCheck.cross < 0.0f) {
		mRot.mY -= tCheck.turn * turnspeed;
	}

	//移動
	if (mState == EAUTOMOVE || mState == ECHASE) {
		mPosition += mMove * mSpeed;
	}

	//表示が180度反転してるので調整
	CVector adjust_rot = mRot;
	adjust_rot.mY += M_PI;
	mRotation = (adjust_rot) * (180.0f / M_PI);

	mMove = CVector(0.0f, 0.0f, 0.0f);

	if (mHp <= 0) {
		mState = EDEATH;
	}

	if (mHp < 0) mHp = 0;

	EHp = mHp;

	CXCharacter::Update();
}

CXEnemy* CXEnemy::GetInstance()
{
	return mInstance;
}

void CXEnemy::Collision(CCollider* m, CCollider* o)
{
	if (m->mType == CCollider::ESPHERE)
	{
		if (o->mType == CCollider::ESPHERE)
		{
			if (o->mpParent->mTag == EPLAYER)
			{
				if (o->mTag == CCollider::ESWORD)
				{
					if (CCollider::Collision(m, o))
					{
						//キャスト変換
						//プレイヤーの攻撃を受けた時
						if (((CXPlayer*)(o->mpParent))->mHit == true)
						{
							switch (m->mTag) {
							case CCollider::EBODY:
								mHp -= DAMAGE_BODY;
								mState = EDAMAGED;
								mAttackFlag_1 = false;
								break;
							case CCollider::EHEAD:
								mHp -= DAMAGE_HEAD;
								mState = EDAMAGED;
								mAttackFlag_1 = false;
								break;
							default:
								break;
							}
						}
					}
				}
			}
		}
	}
}

//待機処理
void CXEnemy::Idle()
{
	//待機アニメーション
	ChangeAnimation(2, true, 200);

	//プレイヤーが一定距離まで近づくと追跡状態へ移行
	if (mPlayerDis <= SEARCH_DIS) {
		mState = ECHASE;
	}

	int random = rand() % 180;
	if (random == 0) {
		//移動状態へ移行する
		mState = EAUTOMOVE;
		mPoint = mPosition;
		mPoint.mX += -15.0f + (float)(rand() % 30);
		mPoint.mZ += -15.0f + (float)(rand() % 30);
	}
}

//移動処理
void CXEnemy::AutoMove()
{
	//移動アニメーション
	ChangeAnimation(1, true, 70);

	mSpeed = SPEED_MOVE;
	//目的地点までのベクトルを求める
	CVector Point = mPoint - mPosition;
	mMove = Point.Normalize();

	//プレイヤーが一定距離まで近づくと追跡状態へ移行
	if (mPlayerDis <= SEARCH_DIS) {
		mState = ECHASE;
	}

	float length = 0.0f;
	length = Point.Length();
	if (length <= 1.0f) {
		mState = EIDLE;
	}

}

//追跡処理
void CXEnemy::Chase() 
{
	//移動アニメーション
	ChangeAnimation(1, true, 70);

	mSpeed = SPEED_CHASE;
	mMove = mPlayerPoint.Normalize();

	//プレイヤーとの距離が追跡可能な距離を超えると待機状態へ移行
	if (mPlayerDis >= CHASE_DIS_MAX) {
		mState = EIDLE;
	}
	//プレイヤーとの距離が攻撃可能な距離の場合
	else if (mPlayerDis <= ATTACK_DIS) {
		//プレイヤーが正面にいるとき
		if (-2.0f < mDot && mDot < 2.0f)
		{
			//攻撃状態へ移行
			mState = EATTACK_1;
		}
	}
}

//攻撃1処理
void CXEnemy::Attack_1()
{
	mMove = mPlayerPoint.Normalize();

	if (mAttackFlag_1 == false) {
		mAttackFlag_1 = true;
		ChangeAnimation(7, false, 80);
	}

	if (mAnimationIndex == 7)
	{
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mAttackFlag_1 = false;
		}
	}
}

void CXEnemy::Damaged()
{
	ChangeAnimation(12, false, 30);
	
	CVector KnockBackVec = mPlayerPoint.Normalize();
	float KnockBackAmount = 0.2f;

	mPosition -= KnockBackVec * KnockBackAmount;

	if (mAnimationIndex == 12)
	{
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mState = EIDLE;
		}
	}
}

//死亡処理
void CXEnemy::Death()
{
	//死亡アニメーション
	ChangeAnimation(11, false, 60);
}

void CXEnemy::SetPos(CVector hpos)
{
	mPosition = hpos;
}

CVector CXEnemy::GetPos()
{
	return mPosition;
}
