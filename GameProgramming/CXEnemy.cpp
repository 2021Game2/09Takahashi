#include "CXEnemy.h"
#include "CXPlayer.h"
#include "CUtil.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "CCamera.h"

#define HP_MAX 500			//体力最大値
#define DAMAGE_BODY 10		//ダメージ(体)
#define DAMAGE_HEAD 20		//ダメージ(頭)
#define ATTACK_DIS 4.0f		//攻撃可能になる距離
#define SPEED_MOVE 0.05f	//通常移動のスピード
#define SPEED_CHASE 0.1f	//追跡中の移動速度
#define SPEED_ATTACK_2 0.2f //攻撃2状態の移動速度
#define CHASE_DIS_MAX 20.0f	//追跡可能な最大距離
#define SEARCH_DIS 15.0f	//追跡を開始する距離
#define STUN_TIME 300		//罠にかかった時のスタン時間
#define AVOID_DIS 6.0f		//回避可能になる距離
#define GAUGE_WID_MAX 50.0f	//ゲージの幅の最大値

CXEnemy* CXEnemy::mInstance;

#ifdef _DEBUG
extern int EHp;	//敵の体力確認用、後で削除
#endif

CXEnemy::CXEnemy()
	: mColSphereBody(this, nullptr, CVector(0.5f, -1.0f, 0.0f), 1.2f)
	, mColSphereHead(this, nullptr, CVector(0.0f, 1.f, 0.0f), 1.2f)
	, mColSphereSword0(this, nullptr, CVector(0.7f, 3.5f, -0.2f), 0.6f)
	, mColSphereSword1(this, nullptr, CVector(0.5f, 2.5f, -0.2f), 0.6f)
	, mColSphereSword2(this, nullptr, CVector(0.3f, 1.5f, -0.2f), 0.6f)
	, mHp(HP_MAX)
	, mPoint(0.0f, 0.0f, 0.0f)
	, mPlayerPoint(0.0f, 0.0f, 0.0f)
	, mPlayerDis(0.0f)
	, mMoveDir(0.0f, 0.0f, 0.0f)
	, mRot(0.0f, 0.0f, 0.0f)
	, mSpeed(0.0f)
	, mDot(0.0f)
	, mStunTime(0)
{
	mTag = EENEMY;	//敵

	mColSphereBody.mTag = CCollider::EBODY;		//本体
	mColSphereHead.mTag = CCollider::EHEAD;		//頭
	mColSphereSword0.mTag = CCollider::ESWORD;	//剣
	mColSphereSword1.mTag = CCollider::ESWORD;	//剣
	mColSphereSword2.mTag = CCollider::ESWORD;	//剣

	mState = EIDLE;	//待機

	mInstance = this;

	mTexture.Load("Gauge.png");
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
	//プレイヤー方向のベクトルを求める
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

	case EATTACK_1:	//攻撃1状態
		Attack_1();	//攻撃1処理を呼ぶ
		break;

	case EATTACK_2: //攻撃2状態
		Attack_2(); //攻撃2処理を呼ぶ
		break;

	case EKNOCKBACK: //ノックバック状態
		KnockBack(); //ノックバック処理を呼ぶ
		break;

	case EDEATH: //死亡状態
		Death(); //死亡処理を呼ぶ
		break;

	case ESTUN:	//スタン状態
		Stun();	//スタン処理を呼ぶ
		break;

	case EAVOID: //回避状態
		Avoid(); //回避処理を呼ぶ
		break;
	}

	Check tCheck = CUtil::GetCheck2D(mMoveDir.mX, mMoveDir.mZ, 0, 0, mRot.mY);
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

	//移動する
	mPosition += mMoveDir * mSpeed;

	//移動方向リセット
	mMoveDir = CVector(0.0f, 0.0f, 0.0f);
	//移動スピードリセット
	mSpeed = 0.0f;

	//表示が180度反転してるので調整
	CVector adjust_rot = mRot;
	adjust_rot.mY += M_PI;
	mRotation = (adjust_rot) * (180.0f / M_PI);


	//体力が0になると死亡状態へ移行
	if (mHp <= 0) {
		mState = EDEATH;
		mHp = 0;
	}

	//////////////////////
	//後で削除する
#ifdef _DEBUG
	EHp = mHp;
#endif
	//////////////////////

	CXCharacter::Update();
}

void CXEnemy::Render2D()
{
	//体力の割合
	float hpRate = (float)mHp / (float)HP_MAX;
	//体力ゲージの幅
	float hpGaugeWid = GAUGE_WID_MAX * hpRate;

	CVector ret;
	Camera.WorldToScreen(&ret, mPosition);

	//2D描画開始
	CUtil::Start2D(0, 800, 0, 600);

	//体力が減ると幅が減少する、左揃え
	//mFont.DrawString("0", ret.mX - GAUGE_WID_MAX * (1 - hpRate), ret.mY + 150.0f, hpGaugeWid, 10);

	//ゲージ背景
	mTexture.Draw(ret.mX - GAUGE_WID_MAX, ret.mX+GAUGE_WID_MAX, ret.mY + 137.5f, ret.mY + 152.5f, 210, 290, 63, 0);
	//体力ゲージ
	mTexture.Draw(ret.mX - GAUGE_WID_MAX, (ret.mX-GAUGE_WID_MAX)+hpGaugeWid*2.0f, ret.mY + 137.5f, ret.mY + 152.5f, 0, 0, 0, 0);

	//2Dの描画終了
	CUtil::End2D();
}

CXEnemy* CXEnemy::GetInstance()
{
	return mInstance;
}

void CXEnemy::Collision(CCollider* m, CCollider* o)
{
	//死亡状態の時はリターン
	if (mState == EDEATH)return;

	//自分が球コライダ
	if (m->mType == CCollider::ESPHERE)
	{
		//相手が球コライダ
		if (o->mType == CCollider::ESPHERE)
		{
			//相手の親のタグがプレイヤー
			if (o->mpParent->mTag == EPLAYER)
			{
				//相手のコライダのタグが剣
				if (o->mTag == CCollider::ESWORD)
				{
					//衝突判定
					if (CCollider::Collision(m, o))
					{
						//キャスト変換
						//プレイヤーの攻撃を受けたとき
						if (((CXPlayer*)(o->mpParent))->mHit == true)
						{
							//攻撃を受けた箇所
							switch (m->mTag) {
							case CCollider::EBODY:	//体
								mHp -= DAMAGE_BODY;	//ダメージ(体)	
								break;

							case CCollider::EHEAD:	//頭
								mHp -= DAMAGE_HEAD;	//ダメージ(頭)
								break;

							default:
								return; //上記以外はリターン
								break;
							}
							//スタン状態ではない時ノックバック状態へ移行
							if (mState != ESTUN) {
								mState = EKNOCKBACK;
							}
						}
					}
				}
			}
			//罠にかかったとき
			//相手の親のタグが罠
			if (o->mpParent->mTag == ETRAP) 
			{
				//自分のコライダのタグが本体
				if (m->mTag == CCollider::EBODY) 
				{
					//衝突処理
					if (CCollider::Collision(m, o)) 
					{
						//スタン状態でなければ、スタン状態へ移行
						if (mState != ESTUN){
							mState = ESTUN;
							mStunTime = STUN_TIME;
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

	//待機状態中ランダムで移動状態へ移行
	int random = rand() % 180;
	if (random == 0) {
		//目標地点を設定
		mPoint = mPosition;
		mPoint.mX += -15.0f + (float)(rand() % 30);
		mPoint.mZ += -15.0f + (float)(rand() % 30);
		//移動状態へ移行する
		mState = EAUTOMOVE;
	}
}

//移動処理
void CXEnemy::AutoMove()
{
	//移動アニメーション
	ChangeAnimation(1, true, 70);

	//移動スピードを変更
	mSpeed = SPEED_MOVE;

	//目的地点までのベクトルを求める
	CVector Point = mPoint - mPosition;
	//mMoveDirに目標地点方向のベクトルを入れる
	mMoveDir = Point.Normalize();

	//プレイヤーが一定距離まで近づくと追跡状態へ移行
	if (mPlayerDis <= SEARCH_DIS) {
		mState = ECHASE;
	}

	//目標地点までの距離
	float length = Point.Length();
	//目標地点付近まで移動すると待機状態へ移行
	if (length <= 1.0f) {
		mState = EIDLE;
	}
}

//追跡処理
void CXEnemy::Chase() 
{
	//移動アニメーション
	ChangeAnimation(1, true, 70);

	//移動スピードを変更
	mSpeed = SPEED_CHASE;

	//mMoveDirにプレイヤー方向のベクトルを入れる
	mMoveDir = mPlayerPoint.Normalize();

	//プレイヤーとの距離が追跡可能な距離を超えると待機状態へ移行
	if (mPlayerDis >= CHASE_DIS_MAX) {
		mState = EIDLE;
	}

	int random = 0; 
	//プレイヤーが攻撃をしたとき、ランダムで回避状態へ移行
	if (CXPlayer::GetInstance()->mAttackFlag_Once == true) {
		if (mPlayerDis <= AVOID_DIS) {
			random = rand() % 2;
			if (random == 0) {
				mState = EAVOID;
			}
		}
	}

	//プレイヤーとの距離が攻撃可能な距離のときランダムで攻撃1状態へ移行
	if (mPlayerDis <= ATTACK_DIS) {
		random = rand() % 90;
		if (random == 0){
			mState = EATTACK_1;
		}
	}

	//追跡状態中にランダムで攻撃2状態へ移行
	random = rand() % 350;
	if (random == 0) {
		mState = EATTACK_2;
	}
}

//攻撃1処理
void CXEnemy::Attack_1()
{
	//mMoveDirにプレイヤー方向のベクトルを入れる
	mMoveDir = mPlayerPoint.Normalize();

	ChangeAnimation(7, false, 70);

	if (mAnimationIndex == 7)
	{
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mState = EIDLE;
		}
	}
}

//攻撃2処理
void CXEnemy::Attack_2()
{
	//攻撃2アニメーション中ではないとき、攻撃可能な距離までダッシュで近づく
	if (mAnimationIndex != 8) {
		//mMoveDirにプレイヤー方向のベクトルを入れる
		mMoveDir = mPlayerPoint.Normalize();
		//ダッシュアニメーション
		ChangeAnimation(14, true, 50);
		//移動スピードを変更
		mSpeed = SPEED_ATTACK_2;
	}

	//攻撃可能な距離まで近づくと攻撃
	if (mPlayerDis <= ATTACK_DIS) {
		//攻撃2アニメーション
		ChangeAnimation(8, false, 95);
	}

	if (mAnimationIndex == 8)
	{
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mState = EIDLE;	//待機状態へ移行
		}
	}
}

//ノックバック処理
void CXEnemy::KnockBack()
{
	ChangeAnimation(12, false, 30);
	
	//ノックバック方向
	CVector KnockBackVec = mPlayerPoint.Normalize();
	//ノックバック量
	float KnockBackAmount = 0.2f;

	//ノックバックさせる
	mPosition -= KnockBackVec * KnockBackAmount;

	if (mAnimationIndex == 12)
	{
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mState = EIDLE;	//待機状態へ移行
		}
	}
}

//死亡処理
void CXEnemy::Death()
{
	//死亡アニメーション
	ChangeAnimation(11, false, 60);
}

//スタン処理
void CXEnemy::Stun()
{
	ChangeAnimation(13, true, 80);

	//スタン時間を減少させる
	mStunTime--;
	//スタン時間が0になると待機状態へ移行
	if (mStunTime <= 0) {
		mState = EIDLE;
	}
}

//回避処理
void CXEnemy::Avoid()
{
	//プレイヤーのいる方向を見ながら後ろへ回避
	mMoveDir = mPlayerPoint.Normalize();
	mSpeed = -0.2f;

	ChangeAnimation(15, false, 40);
	if (mAnimationIndex == 15) {
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			int random = rand() % 4;
			//回避後ランダムで攻撃2状態へ移行する
			if (random == 0) {
				mState = EATTACK_2;
			}
			else {
				mState = EIDLE; //待機状態へ移行
			}
		}
	}
}

//位置を設定する
void CXEnemy::SetPos(CVector hpos)
{
	mPosition = hpos;
}

//位置を取得する
CVector CXEnemy::GetPos()
{
	return mPosition;
}

//死亡時trueを返す
bool CXEnemy::DeathFlag()
{
	return (mState == EDEATH);
}
