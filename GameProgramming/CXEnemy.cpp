#include "CXEnemy.h"
#include "CXPlayer.h"
#include "CUtil.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "CCamera.h"
#include <time.h>
#include "CEffect.h"
#include "CSound.h"
#include "CCollisionManager.h"
#include "CRes.h"
#include "CEnemyManager.h"

#define HP_MAX 150			//体力最大値
#define DAMAGE_BODY 10		//ダメージ(体)
#define DAMAGE_HEAD 20		//ダメージ(頭)
#define ATTACK_DIS 4.0f		//攻撃可能になる距離
#define SPEED_MOVE 0.05f	//通常移動のスピード
#define SPEED_CHASE 0.1f	//追跡中の移動速度
#define SPEED_ATTACK_2 0.2f //攻撃2状態の移動速度
#define CHASE_DIS_MAX 15.0f	//追跡可能な最大距離
#define SEARCH_DIS 15.0f	//追跡を開始する距離
#define STUN_TIME 420		//罠にかかった時のスタン時間
#define AVOID_DIS 4.0f		//回避可能になる距離
#define GAUGE_WID_MAX 50.0f	//ゲージの幅の最大値

#define FONT "Resource\\FontG.png" //フォント
#define IMAGE_GAUGE "Resource\\Gauge.png" //ゲージ画像
#define IMAGE_TARGET "Resource\\Image_Target.png" //ターゲット画像

extern CSound SE_Attack_Hit_1;	//攻撃ヒット時の効果音
extern CSound SE_Knight_Walk;	//敵(ナイト)の歩行時の効果音
extern CSound SE_Knight_Run;	//敵(ナイト)の走行時の効果音

CXEnemy::CXEnemy()
	: mColSphereBody(this, nullptr, CVector(0.5f, -1.0f, 0.0f), 1.2f)
	, mColSphereHead(this, nullptr, CVector(0.0f, 1.f, 0.0f), 1.2f)
	, mColSphereSword0(this, nullptr, CVector(0.7f, 3.5f, -0.2f), 0.8f)
	, mColSphereSword1(this, nullptr, CVector(0.5f, 2.5f, -0.2f), 0.8f)
	, mColSphereSword2(this, nullptr, CVector(0.3f, 1.5f, -0.2f), 0.8f)
	, mHp(HP_MAX)
	, mPoint(0.0f, 0.0f, 0.0f)
	, mPlayerPoint(0.0f, 0.0f, 0.0f)
	, mPlayerDis(0.0f)
	, mMoveDir(0.0f, 0.0f, 0.0f)
	, mRot(0.0f, 0.0f, 0.0f)
	, mSpeed(0.0f)
	, mDot(0.0f)
	, mStunTime(0)
	, mHit(false)
	, mIsTarget(false)
	, mIsInvincible(false)
{
	Init(&CRes::sKnight);

	mTag = EENEMY;	//敵

	mColSphereBody.mTag = CCollider::EBODY;		//本体
	mColSphereHead.mTag = CCollider::EHEAD;		//頭
	mColSphereSword0.mTag = CCollider::ESWORD;	//剣
	mColSphereSword1.mTag = CCollider::ESWORD;	//剣
	mColSphereSword2.mTag = CCollider::ESWORD;	//剣

	mState = EIDLE;	//待機

	mFont.LoadTexture(FONT, 1, 4096 / 64);

	mImageGauge.Load(IMAGE_GAUGE);

	mImageTarget.Load(IMAGE_TARGET);

	srand((unsigned)time(NULL)); //乱数用
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
}

void CXEnemy::Update()
{
	//プレイヤーの攻撃判定が無効になると敵の無敵判定を解除する
	if (CXPlayer::GetInstance()->mHit == false) {
		mIsInvincible = false;
	}

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

	case EATTACK_IDLE: //攻撃待機状態
		Attack_Idle(); //攻撃待機処理を呼ぶ
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

	CXCharacter::Update();
}

void CXEnemy::Render2D()
{
	//2D描画開始
	CUtil::Start2D(0, 800, 0, 600);

	CVector ret;
	CVector tpos = mpCombinedMatrix[6].GetPos();
	Camera.WorldToScreen(&ret, tpos);

	float HpRate = (float)mHp / (float)HP_MAX; //体力最大値に対する、現在の体力の割合
	float HpGaugeWid = GAUGE_WID_MAX * HpRate; //体力ゲージの幅
	
	//画面外の時に表示しない
	if (ret.mX > 0 && ret.mX < 800) {
		//ゲージ背景
		mImageGauge.Draw(ret.mX - GAUGE_WID_MAX, ret.mX + GAUGE_WID_MAX, ret.mY + 30.0f, ret.mY + 45.0f, 210, 290, 63, 0);
		//体力ゲージ
		mImageGauge.Draw(ret.mX - GAUGE_WID_MAX, (ret.mX - GAUGE_WID_MAX) + HpGaugeWid * 2.0f, ret.mY + 30.0f, ret.mY + 45.0f, 0, 0, 0, 0);
		//プレイヤーの攻撃対象になっているとき
		if (mIsTarget) {
			//ターゲット画像表示
			mImageTarget.Draw(ret.mX - 30.0f, ret.mX + 30.0f, ret.mY - 30.0f, ret.mY + 30.0f, 0, 255, 255, 0);
		}
	}
	//2Dの描画終了
	CUtil::End2D();
}

void CXEnemy::Collision(CCollider* m, CCollider* o)
{
	//自分が死亡状態の時はリターン
	if (mState == EDEATH)return;

	//自分と相手が球コライダの時
	if (m->mType == CCollider::ESPHERE && o->mType == CCollider::ESPHERE)
	{
		//相手の親のタグがプレイヤー
		if (o->mpParent->mTag == EPLAYER)
		{
			//相手のコライダのタグが剣
			if (o->mTag == CCollider::ESWORD)
			{
				//キャスト変換
				//プレイヤーの攻撃のヒット判定が有効なとき、無敵フラグが有効でないとき
				if (((CXPlayer*)(o->mpParent))->mHit == true && mIsInvincible == false)
				{
					//球コライダ同士の衝突判定
					if (CCollider::Collision(m, o))
					{
						//攻撃を受けた箇所
						switch (m->mTag) {
						case CCollider::EBODY:	//体
							mHp -= DAMAGE_BODY;	//ダメージを受ける(体)	
							mIsInvincible = true; //無敵フラグを有効にする
							new CEffect(((CXPlayer*)(o->mpParent))->GetSwordColPos(), 1.0f, 1.0f, "", 3, 5, 2); //エフェクトを生成する
							SE_Attack_Hit_1.Play(); //効果音を再生する
							//スタン状態で無ければノックバック状態へ移行
							if (mState != ESTUN && mState != EATTACK_1 && mState != EATTACK_2) {
								mState = EKNOCKBACK;
								mHit = false; //自分の攻撃のヒット判定を終了させる
							}
							break;

						case CCollider::EHEAD:	//頭
							mHp -= DAMAGE_HEAD;	//ダメージを受ける(頭)
							mIsInvincible = true; //無敵フラグを有効にする
							new CEffect(((CXPlayer*)(o->mpParent))->GetSwordColPos(), 1.5f, 1.5f, "", 3, 5, 2); //エフェクトを生成する
							SE_Attack_Hit_1.Play(); //効果音を再生する
							//スタン状態で無ければノックバック状態へ移行
							if (mState != ESTUN && mState != EATTACK_1 && mState != EATTACK_2) {
								mState = EKNOCKBACK;
								mHit = false; //自分の攻撃のヒット判定を終了させる
							}
							break;

						default: //上記以外
							break;
						}
					}
				}
			}
		}
		//罠にかかった時
		//相手の親のタグが罠
		if (o->mpParent->mTag == ETRAP)
		{
			//自分のコライダのタグが本体
			if (m->mTag == CCollider::EBODY)
			{
				//球コライダ同士の衝突判定
				if (CCollider::Collision(m, o))
				{
					//スタン状態でなければ、スタン状態へ移行
					if (mState != ESTUN) {
						mState = ESTUN;
						mStunTime = STUN_TIME; //スタン時間を入れる
						mHit = false; //攻撃のヒット判定を無効化
					}
				}
			}
		}
		//敵同士のすり抜け防止
		//相手の親が敵のとき
		if (o->mpParent->mTag == EENEMY)
		{
			//敵(相手)が死亡状態のときリターンする
			if (((CXEnemy*)(o->mpParent))->mState == CXEnemy::EDEATH)return;
			//敵のボディに当たっているとき
			if (m->mTag == CCollider::EBODY && o->mTag == CCollider::EBODY) {
				CVector adjust;
				if (CCollider::CollisionAdjust(m, o, &adjust)) {
					//敵(自分)がスタン状態のとき
					if (mState == CXEnemy::ESTUN) {
						//敵(相手)のポジションを調整
						CXEnemy* Enemy = (CXEnemy*)o->mpParent;
						Enemy->SetPos(Enemy->GetPos() + adjust);
					}
					else {
						//敵(自分)のポジションを調整
						mPosition -= adjust;
					}
				}
			}
		}
	}

	//自分が球コライダで相手が三角コライダの時
	if (m->mType == CCollider::ESPHERE && o->mType == CCollider::ETRIANGLE)
	{
		//相手の親のタグがマップ
		if (o->mpParent->mTag == EMAP)
		{
			//自分のコライダのタグが頭or体
			if (m->mTag == CCollider::EHEAD || m->mTag == CCollider::EBODY) {
				CVector adjust; //調整用
				//三角コライダと球コライダの衝突判定
				if (CCollider::CollisionTriangleSphere(o, m, &adjust)) {
					//マップ外に出ていかないよう位置を調整
					mPosition += adjust;
					//移動状態のとき
					if (mState == EAUTOMOVE) {
						//壁に向かって歩き続けないように目標地点を更新
						mPoint = mPosition;
						mPoint.mX += -15.0f + (float)(rand() % 30);
						mPoint.mZ += -15.0f + (float)(rand() % 30);
					}
				}
			}
		}
	}
}

void CXEnemy::TaskCollision()
{
	CCollisionManager::Get()->Collision(&mColSphereBody, COLLISIONRANGE);
	CCollisionManager::Get()->Collision(&mColSphereHead, COLLISIONRANGE);
	CCollisionManager::Get()->Collision(&mColSphereSword0, COLLISIONRANGE);
	CCollisionManager::Get()->Collision(&mColSphereSword1, COLLISIONRANGE);
	CCollisionManager::Get()->Collision(&mColSphereSword2, COLLISIONRANGE);
}

//待機処理
void CXEnemy::Idle()
{
	//待機アニメーション
	ChangeAnimation(2, true, 200);

	//プレイヤーが死亡状態では無いとき
	if (CXPlayer::GetInstance()->mState != CXPlayer::EPlayerState::EDEATH) {
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

	//プレイヤーが死亡状態では無いとき
	if (CXPlayer::GetInstance()->mState != CXPlayer::EPlayerState::EDEATH) {
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

//追跡処理
void CXEnemy::Chase() 
{
	//移動アニメーション
	ChangeAnimation(1, true, 70);

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
		random = rand() % 360;
		if (random == 0) {
			mState = EATTACK_2;
		}
	}
}

//攻撃待機処理
void CXEnemy::Attack_Idle()
{
	//待機アニメーション
	ChangeAnimation(2, true, 200);

	//mMoveDirにプレイヤー方向のベクトルを入れる
	mMoveDir = mPlayerPoint.Normalize();

	int random = 0;

	//プレイヤーが攻撃をしたとき
	if (CXPlayer::GetInstance()->mAttackFlag_Once == true) {
		//プレイヤーとの距離が回避可能な距離のとき
		if (mPlayerDis <= AVOID_DIS) {
			//ランダムで回避を行うかどうか判定する
			random = rand() % 6;
			if (random == 0) {
				mState = EAVOID; //回避状態へ移行
			}
		}
	}

	//攻撃可能なとき
	if (CEnemyManager::GetInstance()->mIsEnemyAttack()) {
		//プレイヤーが攻撃可能な距離にいるとき
		if (mPlayerDis <= ATTACK_DIS) {
			//ランダムで攻撃を行うかどうかを判定する
			random = rand() % 180;
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
	if (mPlayerDis >= ATTACK_DIS) {
		//ランダムで状態を移行するかどうか判断する
		random = rand() % 120;
		if (random == 0) {
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

//攻撃1処理
void CXEnemy::Attack_1()
{
	//mMoveDirにプレイヤー方向のベクトルを入れる
	mMoveDir = mPlayerPoint.Normalize();

	ChangeAnimation(7, false, 75);

	if (mAnimationIndex == 7)
	{
		//ヒット判定発生
		if (mAnimationFrame == 30) {
			mHit = true;
		}
		//ヒット判定終了
		if (mAnimationFrame == 60) {
			mHit = false;
		}
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
		//ヒット判定発生
		if (mAnimationFrame == 30) {
			mHit = true;
		}
		//ヒット判定終了
		if (mAnimationFrame == 65) {
			mHit = false;
		}
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
			int random = rand() % 2;
			//回避後ランダムで攻撃可能な場合攻撃2状態へ移行する
			if (random == 0) {
				//攻撃可能なとき
				if (CEnemyManager::GetInstance()->mIsEnemyAttack()) {
					mState = EATTACK_2;
				}
				else {
					mState = EIDLE;
				}
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

bool CXEnemy::mIsAttack()
{
	return (mState == EATTACK_1 || mState == EATTACK_2);
}
