#include "CXEnemy2.h"
#include "CRes.h"
#include "CCollisionManager.h"
#include "CXPlayer.h"
#include "CEnemyManager.h"

#define SEARCH_DIS 15.0f	//追跡を開始する距離
#define SPEED_MOVE 0.05f	//通常移動のスピード
#define SPEED_CHASE 0.1f	//追跡中の移動速度
#define ATTACK_DIS 4.0f		//攻撃可能になる距離
#define CHASE_DIS_MAX 15.0f	//追跡可能な最大距離
#define AVOID_DIS 4.0f		//回避可能になる距離
#define CHASE_RESTART_TIME 60 //追跡を再開するまでの時間
#define SPEED_ATTACK_2 0.2f //攻撃2状態の移動速度

CXEnemy2::CXEnemy2()
	:mColSphereBody(this, nullptr, CVector(0.0f, -1.0f, 0.0f), 1.3f)
	, mColSphereHead(this, nullptr, CVector(0.0f, -0.5f, 0.0f), 1.1f)
	, mColSphereSword0(this, nullptr, CVector(0.0f, 0.0f, -0.5f), 0.9f)
	, mColSphereSword1(this, nullptr, CVector(0.0f, 0.0f, 0.0f), 0.9f)
{
	//初期化
	Init(&CRes::sModelXEnemy2);

	//コライダのタグを設定
	mColSphereBody.mTag = CCollider::EBODY;		//本体
	mColSphereHead.mTag = CCollider::EHEAD;		//頭
	mColSphereSword0.mTag = CCollider::ESWORD;	//剣
	mColSphereSword1.mTag = CCollider::ESWORD;	//剣

	//敵の種類を設定
	mEnemyType = ETYPE_2;
}

CXEnemy2::~CXEnemy2()
{
}

void CXEnemy2::Init(CModelX* model)
{
	CXCharacter::Init(model);
	//合成行列の設定
	mColSphereBody.mpMatrix = &mpCombinedMatrix[2];		//体
	mColSphereHead.mpMatrix = &mpCombinedMatrix[30];	//頭
	mColSphereSword0.mpMatrix = &mpCombinedMatrix[45];	//剣
	mColSphereSword1.mpMatrix = &mpCombinedMatrix[52];	//剣
}

void CXEnemy2::TaskCollision()
{
	CCollisionManager::Get()->Collision(&mColSphereBody, COLLISIONRANGE);
	CCollisionManager::Get()->Collision(&mColSphereHead, COLLISIONRANGE);
	CCollisionManager::Get()->Collision(&mColSphereSword0, COLLISIONRANGE);
	CCollisionManager::Get()->Collision(&mColSphereSword1, COLLISIONRANGE);
}

void CXEnemy2::Idle()
{
	//待機アニメーション
	ChangeAnimation(1, true, 100);

	//プレイヤーが死亡状態では無いとき
	if (CXPlayer::GetInstance()->GetState() != CXPlayer::EPlayerState::EDEATH) {
		//プレイヤーが一定距離まで近づくと追跡状態へ移行
		if (mPlayerDis <= SEARCH_DIS) {
			mState = ECHASE;
		}
	}

	//待機状態中ランダムで移動状態へ移行
	int random = rand() % 240;
	if (random == 0) {
		//目標地点を設定
		mPoint = mPosition;
		//mPointのmXとmZの値へ同時に0.0fが入ると敵が消えてしまうので、最後に値を足している
		mPoint.mX += -15.0f + (float)(rand() % 30) + 0.001f;
		mPoint.mZ += -15.0f + (float)(rand() % 30) + 0.001f;
		//移動状態へ移行する
		mState = EAUTOMOVE;
	}
}

void CXEnemy2::AutoMove()
{
	//移動アニメーション
	ChangeAnimation(5, true, 80);

	//移動スピードを変更
	mSpeed = SPEED_MOVE;

	//目的地点までのベクトルを求める
	CVector Point = mPoint - mPosition;
	//mMoveDirに目標地点方向のベクトルを入れる
	mMoveDir = Point.Normalize();

	//プレイヤーが死亡状態では無いとき
	if (CXPlayer::GetInstance()->GetState() != CXPlayer::EPlayerState::EDEATH) {
		//プレイヤーが一定距離まで近づくと追跡状態へ移行
		if (mPlayerDis <= SEARCH_DIS) {
			mState = ECHASE;
		}
	}

	//目標地点までの距離
	float length = Point.Length();
	//目標地点付近まで移動すると待機状態へ移行
	if (length <= 1.0f) {
		mState = EIDLE;
	}
}

void CXEnemy2::Chase()
{
	//移動アニメーション
	ChangeAnimation(5, true, 80);

	//移動スピードを変更
	mSpeed = SPEED_CHASE;

	//mMoveDirにプレイヤー方向のベクトルを入れる
	mMoveDir = mPlayerPoint.Normalize();

	//プレイヤーが攻撃可能な距離にいるとき
	if (mPlayerDis <= ATTACK_DIS) {
		mState = EATTACK_IDLE; //攻撃待機状態へ移行
	}

	//プレイヤーが追跡可能な距離にいないとき
	if (mPlayerDis >= CHASE_DIS_MAX) {
		mState = EIDLE; //待機状態へ移行
	}

	int random = 0;
	//攻撃可能なとき
	if (CEnemyManager::GetInstance()->mIsEnemyAttack()) {
		//追跡状態中にランダムで攻撃2状態へ移行
		random = rand() % 480;
		if (random == 0) {
			mState = EATTACK_2;
		}
	}
}

void CXEnemy2::Attack_Idle()
{
	//プレイヤーが死亡状態だったとき待機状態に移行する
	if (CXPlayer::GetInstance()->GetState() == CXPlayer::EPlayerState::EDEATH)mState = EIDLE;

	//攻撃待機アニメーション
	ChangeAnimation(3, true, 100);

	//mMoveDirにプレイヤー方向のベクトルを入れる
	mMoveDir = mPlayerPoint.Normalize();

	int random = 0;

	//プレイヤーが攻撃可能な距離にいるとき
	if (mPlayerDis <= ATTACK_DIS) {
		mChaseRestartCount = 0; //カウントをリセットする
		//攻撃可能なとき
		if (CEnemyManager::GetInstance()->mIsEnemyAttack()) {
			//ランダムで攻撃を行うかどうかを判定する
			random = rand() % 120;
			if (random == 0) {
				//ランダムで攻撃の種類を決める
				random = rand() % 2;
				switch (random) {
				case 0:
					mState = EATTACK_1; //攻撃1状態へ移行
					break;
				case 1:
					mState = EATTACK_2; //攻撃2状態へ移行
					break;
				}
			}
		}
	}
	//プレイヤーが攻撃可能な距離にいないとき
	else {
		mChaseRestartCount++; //カウントを加算する
		//カウントが追跡を再開する時間になると状態を移行する
		if (mChaseRestartCount == CHASE_RESTART_TIME) {
			//プレイヤーが追跡可能な距離にいるとき
			if (mPlayerDis <= CHASE_DIS_MAX) {
				mState = ECHASE; //追跡状態へ移行
			}
			else {
				mState = EIDLE; //待機状態へ移行
			}
		}
	}
}

void CXEnemy2::Attack_1()
{
	//mMoveDirにプレイヤー方向のベクトルを入れる
	mMoveDir = mPlayerPoint.Normalize();

	//攻撃1アニメーション
	ChangeAnimation(2, false, 100);

	if (mAnimationIndex == 2)
	{
		//ヒット判定発生
		if (mAnimationFrame == 30) {
			mIsHit = true;
		}
		//ヒット判定終了
		if (mAnimationFrame == 60) {
			mIsHit = false;
		}
		//アニメーション終了時
		if (mIsAnimationEnd())
		{
			mState = EIDLE; //待機状態へ移行
		}
	}
}

void CXEnemy2::Attack_2()
{
	//mMoveDirにプレイヤー方向のベクトルを入れる
	mMoveDir = mPlayerPoint.Normalize();
	//攻撃2アニメーション中ではないとき、攻撃可能な距離までダッシュで近づく
	if (mAnimationIndex != 4) {
		//ダッシュアニメーション
		ChangeAnimation(6, true, 50);
		//移動スピードを変更
		mSpeed = SPEED_ATTACK_2;
	}

	//攻撃可能な距離まで近づくと攻撃
	if (mPlayerDis <= ATTACK_DIS) {
		//攻撃2アニメーション
		ChangeAnimation(4, false, 90);
	}

	if (mAnimationIndex == 4)
	{
		//ヒット判定発生
		if (mAnimationFrame == 40) {
			mIsHit = true;
		}
		//ヒット判定終了
		if (mAnimationFrame == 70) {
			mIsHit = false;
		}
		//アニメーション終了時
		if (mIsAnimationEnd())
		{
			mState = EIDLE;	//待機状態へ移行
		}
	}
}

void CXEnemy2::KnockBack()
{
	//ノックバックアニメーション
	ChangeAnimation(8, false, 30);

	//ノックバック方向
	CVector KnockBackVec = mPlayerPoint.Normalize();
	//ノックバック量
	float KnockBackAmount = 0.15f;

	//ノックバックさせる
	mPosition -= KnockBackVec * KnockBackAmount;

	if (mAnimationIndex == 8)
	{
		//アニメーション終了時
		if (mIsAnimationEnd())
		{
			mState = EIDLE;	//待機状態へ移行
		}
	}
}

void CXEnemy2::Death()
{
	//死亡アニメーション
	ChangeAnimation(7, false, 40);
}

void CXEnemy2::Stun()
{
	//スタンアニメーション
	ChangeAnimation(9, true, 100);

	//スタン時間を減少させる
	mStunTime--;
	//スタン時間が0になると待機状態へ移行
	if (mStunTime <= 0) {
		mState = EIDLE;
	}
}


