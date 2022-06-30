#include "CXEnemy.h"
#include "CXPlayer.h"
#include "CUtil.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "CCamera.h"
#include <windows.h>
#include "CEffect.h"
#include "CCollisionManager.h"
#include "CRes.h"
#include "CEnemyManager.h"

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
#define CHASE_RESTART_TIME 60 //追跡を再開するまでの時間

CXEnemy::CXEnemy()
	: mHp(0)
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
	, mChaseRestartCount(0)
	, mEnemyType(ETYPE_1)
{
	Init(&CRes::sModelXEnemy);

	//タグを設定
	mTag = EENEMY;	//敵

	//初期状態を設定
	mState = EIDLE;	//待機状態

	srand(timeGetTime()); //乱数用
}

void CXEnemy::Init(CModelX* model)
{
	CXCharacter::Init(model);
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

	//状態を判別
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

	//敵の種類がタイプ1のとき
	if (mEnemyType == ETYPE_1) {
		Check tCheck = CUtil::GetCheck2D(mMoveDir.mX, mMoveDir.mZ, 0, 0, mRot.mY);
		float turnspeed = 0.5f;	//回転速度

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
		//表示が180度反転してるので調整
		CVector adjust_rot = mRot;
		adjust_rot.mY += M_PI;
		mRotation = (adjust_rot) * (180.0f / M_PI);
	}
	//それ以外
	else {
		//普通に3次元ベクトル計算で算出したほうが正確だが計算量を懸念する場合は擬似計算で軽量化
		//擬似ベクトル計算
		Check tCheck = CUtil::GetCheck2D(mMoveDir.mX, mMoveDir.mZ, 0, 0, mRotation.mY * (M_PI / 180.0f));

		//回転速度　degreeに直す
		float Turnspeed = (180.0f / M_PI) * 0.5f;

		//急な振り返りを抑制
		if (tCheck.turn > 1.5f) tCheck.turn = 1.5f;

		//移動方向へキャラを向かせる
		if (tCheck.cross > 0.0f) {
			mRotation.mY += tCheck.turn * Turnspeed;
		}
		if (tCheck.cross < 0.0f) {
			mRotation.mY -= tCheck.turn * Turnspeed;
		}
	}

	//移動する
	mPosition += mMoveDir * mSpeed;

	//移動方向リセット
	mMoveDir = CVector(0.0f, 0.0f, 0.0f);
	//移動スピードリセット
	mSpeed = 0.0f;

	//体力が0になると死亡状態へ移行
	if (mHp <= 0) {
		mState = EDEATH;
		mHp = 0;
	}

	CXCharacter::Update();
}

void CXEnemy::Render2D()
{
	//自分が死亡状態の時リターンする
	if (mState == EDEATH)return;

	//2D描画開始
	CUtil::Start2D(0, 800, 0, 600);

	CVector ret;
	CVector tpos;
	//敵の種類を判断
	switch (mEnemyType) {
	case ETYPE_1:
		tpos = mpCombinedMatrix[6].GetPos();
		break;

	case ETYPE_2:
		tpos = mpCombinedMatrix[30].GetPos();
		break;
	}
	Camera.WorldToScreen(&ret, tpos);

	float HpRate = (float)mHp / (float)mHpMax; //体力最大値に対する、現在の体力の割合
	float HpGaugeWid = GAUGE_WID_MAX * HpRate; //体力ゲージの幅
	
	//画面外の時に表示しない
	if (ret.mX > 0 && ret.mX < 800) {
		//ゲージ背景
		CRes::sImageGauge.Draw(ret.mX - GAUGE_WID_MAX, ret.mX + GAUGE_WID_MAX, ret.mY + 30.0f, ret.mY + 45.0f, 210, 290, 63, 0);
		//体力ゲージ
		CRes::sImageGauge.Draw(ret.mX - GAUGE_WID_MAX, (ret.mX - GAUGE_WID_MAX) + HpGaugeWid * 2.0f, ret.mY + 30.0f, ret.mY + 45.0f, 0, 0, 0, 0);
		//プレイヤーの攻撃対象になっているとき
		if (mIsTarget) {
			//ターゲット画像表示
			CRes::sImageTarget.Draw(ret.mX - 30.0f, ret.mX + 30.0f, ret.mY - 30.0f, ret.mY + 30.0f, 0, 255, 255, 0);
		}
		//スタン状態のとき
		if (mState == ESTUN) {
			//スタン画像表示
			CRes::sImageStun.Draw(ret.mX - 20.0f, ret.mX + 20.0f, ret.mY + 45.0f, ret.mY + 85.0f, 0, 255, 255, 0);
		}
	}
	//2Dの描画終了
	CUtil::End2D();
}

void CXEnemy::Collision(CCollider* m, CCollider* o)
{
	//自分が死亡状態の時はリターン
	if (mState == EDEATH)return;

	//相手の親が自分の時はリターン
	if (o->mpParent == this)return;

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
							CRes::sSEAttackHit1.Play(); //効果音を再生する
							//ノックバックが可能なとき
							if (mIsKnockBack()) {
								mState = EKNOCKBACK; //ノックバック状態へ移行
								mHit = false; //自分の攻撃のヒット判定を終了させる
							}
							break;

						case CCollider::EHEAD:	//頭
							mHp -= DAMAGE_HEAD;	//ダメージを受ける(頭)
							mIsInvincible = true; //無敵フラグを有効にする
							new CEffect(((CXPlayer*)(o->mpParent))->GetSwordColPos(), 1.5f, 1.5f, "", 3, 5, 2); //エフェクトを生成する
							CRes::sSEAttackHit1.Play(); //効果音を再生する
							//ノックバックが可能なとき
							if (mIsKnockBack()) {
								mState = EKNOCKBACK; //ノックバック状態へ移行
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
				//スタン状態でなければ
				if (mState != ESTUN) 
				{
					//球コライダ同士の衝突判定
					if (CCollider::Collision(m, o))
					{
						mState = ESTUN; //スタン状態へ移行
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

}

//待機処理
void CXEnemy::Idle()
{
	//待機アニメーション
	ChangeAnimation(2, true, 200);

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
		//mPointのmXとmZの値へ同時に0.0fが入ると敵が消えるので、最後に値を足している
		mPoint.mX += -15.0f + (float)(rand() % 30) + 0.001f;
		mPoint.mZ += -15.0f + (float)(rand() % 30) + 0.001f;
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
		random = rand() % 480;
		if (random == 0) {
			mState = EATTACK_2;
		}
	}
}

//攻撃待機処理
void CXEnemy::Attack_Idle()
{
	//プレイヤーが死亡状態だったとき待機状態に移行する
	if (CXPlayer::GetInstance()->GetState() == CXPlayer::EPlayerState::EDEATH)mState = EIDLE;

	//待機アニメーション
	ChangeAnimation(2, true, 200);

	//mMoveDirにプレイヤー方向のベクトルを入れる
	mMoveDir = mPlayerPoint.Normalize();

	int random = 0;

	//プレイヤーとの距離が回避可能な距離のとき
	if (mPlayerDis <= AVOID_DIS) {
		//プレイヤーが攻撃をしたとき
		if (CXPlayer::GetInstance()->mAttackFlag_Once == true) {
			//ランダムで回避を行うかどうか判定する
			random = rand() % 6;
			if (random == 0) {
				mState = EAVOID; //回避状態へ移行
			}
		}
	}

	//プレイヤーが攻撃可能な距離にいるとき
	if (mPlayerDis <= ATTACK_DIS) {
		mChaseRestartCount = 0; //カウントをリセットする
		//攻撃可能なとき
		if (CEnemyManager::GetInstance()->mIsEnemyAttack()) {
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
		//アニメーション終了時
		if (mIsAnimationEnd())
		{
			mState = EIDLE; //待機状態へ移行
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
		//アニメーション終了時
		if (mIsAnimationEnd())
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
		//アニメーション終了時
		if (mIsAnimationEnd())
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
	if (mAnimationIndex == 15) 
	{
		//アニメーション終了時
		if (mIsAnimationEnd())
		{
			int random = rand() % 2;
			//回避後ランダムで攻撃2状態へ移行するかどうか判断
			switch (random) {
			case 0:
				//攻撃可能なとき
				if (CEnemyManager::GetInstance()->mIsEnemyAttack()) {
					mState = EATTACK_2; //攻撃2状態へ移行
				}
				//攻撃可能ではないとき
				else {
					mState = EIDLE; //待機状態へ移行
				}
				break;

			case 1:
				mState = EIDLE; //待機状態へ移行
				break;
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

//敵の状態を取得する
CXEnemy::EEnemyState CXEnemy::GetState()
{
	return mState;
}

//体力を設定する
void CXEnemy::SetHp(int hp)
{
	mHp = hp;		//体力の初期値を設定
	mHpMax = mHp;	//体力の初期値を体力の最大値とする
}

//死亡状態のときtrueを返す
bool CXEnemy::mIsDeath()
{
	return (mState == EDEATH);
}

//ノックバックが可能なときにtrueを返す
bool CXEnemy::mIsKnockBack()
{
	//スタン状態もしくは攻撃状態でなければtrue
	return (mState != ESTUN && mState != EATTACK_1 && mState != EATTACK_2);
}

//攻撃状態のときtrueを返す
bool CXEnemy::mIsAttack()
{
	return (mState == EATTACK_1 || mState == EATTACK_2);
}
