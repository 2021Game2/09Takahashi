#include "CXPlayer.h"
#include "CKey.h"
#include "CCamera.h"
#include "CUtil.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "CInput.h"
#include "CXEnemy.h"

#define GRAVITY 0.9f	//重力
#define HP_MAX 100	//体力最大値
#define STAMINA_MAX 100	//スタミナ最大値
#define AVOID_STAMINA 40	//回避時のスタミナの減少量
#define AVOID_TIME 30	//回避時間
#define AVOID_FIRSTSPEED 0.5f	//回避時の初速
#define SPEED_DEFAULT 0.15f	//スピード(通常時)
#define SPEED_DASH_HIGH 0.2f	//スピード(ダッシュ時)
#define SPEED_DASH_LOW 0.05f	//スピード(ダッシュ時、スタミナ切れ)
#define INVINCIBLETIME_AVOID 30	//無敵時間(回避時)
#define INVINCIBLETIME_DAMAGE 60	//無敵時間(ダメージ発生時)
#define DAMAGE 20	//ダメージ
#define ATTACK2_FIRSTSPEED 0.6f	//攻撃2使用時の初速
#define GRACETIME 10	//派生攻撃の受付時間

CXPlayer* CXPlayer::mInstance;

extern int S;	//確認用、後で削除
extern int PHp;	//確認用、後で削除

CXPlayer::CXPlayer()
	: mColSphereBody(this, nullptr, CVector(), 0.5f)
	, mColSphereHead(this, nullptr, CVector(0.0f, 5.0f, -3.0f), 0.5f)
	, mColSphereSword(this, nullptr, CVector(-10.0f, 10.0f, 50.0f), 0.3f)
	, mHp(HP_MAX)
	, mStamina(STAMINA_MAX)
	, mAvoid(false)
	, mAvoidTime(0)
	, mAvoidSpeed(0.0f)
	, mSpeed(SPEED_DEFAULT)
	, mMoveKeep(0.0f, 0.0f, 0.0f)
	, mInvincibleFlag(false)
	, mInvincibleTime(0)
	, mMove(0.0f, 0.0f, 0.0f)
	, mMove2(0.0f, 0.0f, 0.0f)
	, mSideVec(0.0f, 0.0f, 0.0f)
	, mFrontVec(0.0f, 0.0f, 0.0f)
	, mTurnspeed(0.0f)
	, mAttackFlag_1(false)
	, mAttackFlag_2(false)
	, mAttack2Speed(0.0f)
	, mAttackFlag_3(false)
	,mGraceTime(0)
	,mHit(false)
{
	//タグにプレイヤーを設定します
	mTag = EPLAYER;
	mColSphereBody.mTag = CCollider::EBODY;
	mColSphereHead.mTag = CCollider::EHEAD;
	mColSphereSword.mTag = CCollider::ESWORD;

	mState = EIDLE;	//待機状態

	mInstance = this;
}

void CXPlayer::Init(CModelX* model)
{
	CXCharacter::Init(model);
	//合成行列の設定
	mColSphereBody.mpMatrix = &mpCombinedMatrix[9];
	//頭
	mColSphereHead.mpMatrix = &mpCombinedMatrix[12];
	//剣
	mColSphereSword.mpMatrix = &mpCombinedMatrix[22];

	mRotation.mY = 0.01f;
}

void CXPlayer::Update()
{
	switch (mState) {
	case EIDLE:	//待機状態
		Idle();	//待機処理を呼ぶ
		//左クリックで攻撃1へ移行
		if (CKey::Once(VK_LBUTTON)) {
			mState = EATTACK_1;
		}
		//WASDキーを押すと移動へ移行
		else if (CKey::Push('W') || CKey::Push('A') || CKey::Push('S') || CKey::Push('D')) {
			mState = EMOVE;
		}
		break;

	case EATTACK_1:	//攻撃1状態
		Attack_1();	//攻撃1の処理を呼ぶ
		//受付時間内に左クリックで攻撃3へ移行
		if (mGraceTime > 0 && CKey::Once(VK_LBUTTON)) {
			mState = EATTACK_3;
		}
		if (mAttackFlag_1 == false) {
			mState = EIDLE;
		}
		break;

	case EATTACK_2:	//攻撃2状態
		Attack_2();	//攻撃2の処理を呼ぶ
		if (mAttackFlag_2 == false) {
			mState = EIDLE;
		}
		break;

	case EATTACK_3:	//攻撃3状態の時
		Attack_3();	//攻撃3の処理を呼ぶ
		if (mAttackFlag_3 == false) {
			mState = EIDLE;
		}
		break;

	case EMOVE:	//移動状態
		//左クリックで攻撃1へ移行
		if (CKey::Once(VK_LBUTTON)) {
			mState = EATTACK_2;
		}
		//SHIFTキーを押すとダッシュへ移行
		else if (CKey::Push(VK_SHIFT)) {
			mState = EDASH;
		}
		//SPACEキーを押す＆回避に必要な量のスタミナがあるとき回避へ移行
		else if (CKey::Once(VK_SPACE) && mStamina >= AVOID_STAMINA) {
			mState = EAVOID;
		}
		//WASDキーを押すと移動
		else if (CKey::Push('W') || CKey::Push('A') || CKey::Push('S') || CKey::Push('D')) {
			Move();	//移動処理を呼ぶ
		}
		//待機状態へ移行
		else {
			mState = EIDLE;
		}
		break;

	case EDASH:	//ダッシュ状態
		//左クリックで攻撃1へ移行
		if (CKey::Once(VK_LBUTTON)) {
			mState = EATTACK_2;
		}
		//SPACEキーを押す＆回避に必要な量のスタミナがあるとき回避へ移行
		else if (CKey::Once(VK_SPACE) && mStamina >= AVOID_STAMINA) {
			mState = EAVOID;
		}
		//SHIFTキーを押すとダッシュ
		else if (CKey::Push(VK_SHIFT)) {
			Dash();	//ダッシュ処理を呼ぶ
		}
		//WASDキーを押すと移動へ移行
		else if (CKey::Push('W') || CKey::Push('A') || CKey::Push('S') || CKey::Push('D')) {
			mState = EMOVE;
		}
		//待機状態へ移行
		else {
			mState = EIDLE;
		}
		break;

	case EAVOID:	//回避状態
		Avoid();	//回避処理を呼ぶ
		if (mAvoid == false) {
			//回避終了時WASDキー＆SHIFTキーが押されているとダッシュへ移行
			if (CKey::Push('W') || CKey::Push('A') || CKey::Push('S') || CKey::Push('D')&&CKey::Push(VK_SHIFT)) {
				mState = EDASH;
			}
			//回避終了時WASDキーが押されていると移動へ移行
			else if (CKey::Push('W') || CKey::Push('A') || CKey::Push('S') || CKey::Push('D')) {
				mState = EMOVE;
			}
			//待機状態へ移行
			else {
				mState = EIDLE;
			}	
		}
		break;

	case EDEATH:	//死亡状態
		Death();	//死亡処理を呼ぶ
		break;
	}

	//ダッシュ、回避をしていない状態の時スタミナを回復させる
	if (mState != EDASH && mState != EAVOID && mStamina < STAMINA_MAX) {
		mStamina++;
	}

	//無敵時間のカウントダウン
	if (mInvincibleTime > 0) {
		mInvincibleTime--;
	}
	//無敵時間切れ
	else {
		mInvincibleFlag = false;	//無敵状態終了
	}

	//座標移動
	mPosition += mMove2;

	//移動量を減らす
	mMove2 = mMove2 * GRAVITY;

	//リセット
	mMove = CVector(0.0f, 0.0f, 0.0f);

	//体力が0になると死亡
	if (mHp <= 0) {
		mState = EDEATH;	//死亡状態へ移行
		mHp = 0;
	}

	//後で削除する
	//////////////////////////////
	if (mHp<=0&&CKey::Once(VK_RETURN)) {
		mHp = HP_MAX;
		mState = EIDLE;
	}
	S = mStamina;
	PHp = mHp;
	//////////////////////////////

	//注視点
	Camera.SetTarget(mPosition);

	CXCharacter::Update();
}

void CXPlayer::Collision(CCollider* m, CCollider* o)
{
	if (m->mType == CCollider::ESPHERE) {
		if (o->mType == CCollider::ESPHERE) {
			if (o->mpParent->mTag == EENEMY) {
				//敵の攻撃を受けた時
				if (o->mTag == CCollider::ESWORD) {
					if (mInvincibleFlag == false) {
						if (CCollider::Collision(m, o)) {
							//キャスト変換
							if (((CXEnemy*)(o->mpParent))->mState == CXEnemy::EATTACK_1) {
								switch (m->mTag) {
								case CCollider::EHEAD:
									mHp -= DAMAGE;
									mInvincibleTime = INVINCIBLETIME_DAMAGE;
									mInvincibleFlag = true;
									break;

								case CCollider::EBODY:
									mHp -= DAMAGE;
									mInvincibleTime = INVINCIBLETIME_DAMAGE;
									mInvincibleFlag = true;
									break;
								}
							}
						}
					}
				}
				//敵に攻撃が当たった時、mHitをfalseにする
				if (m->mTag == CCollider::ESWORD) {
					if (o->mTag == CCollider::EHEAD || o->mTag == CCollider::EBODY) {
						if (mHit == true) {
							if (CCollider::Collision(m, o)) {
								mHit = false;
							}
						}
					}
				}
			}
		}
	}
}

CXPlayer* CXPlayer::GetInstance()
{
	return mInstance;
}

//待機処理
void CXPlayer::Idle()
{
	ChangeAnimation(0, true, 60);	//待機モーション
}

//移動処理
void CXPlayer::Move()
{
	ChangeAnimation(1, true, 60);
	//ダッシュ時にスピードを上書きしない用
	if (mState == EMOVE) {
		mSpeed = SPEED_DEFAULT;
	}

	//カメラ視点移動　通称無双移動
	//カメラの左右と前後のベクトルを取得
	mSideVec = Camera.GetMat().GetXVec();
	mFrontVec = Camera.GetMat().GetZVec();
	//高さ移動はカットする
	mSideVec.mY = 0.0f;
	mFrontVec.mY = 0.0f;
	//正規化する
	mSideVec.Normalize();
	mFrontVec.Normalize();

	if (CKey::Push('A'))
	{
		mMove -= mSideVec;
	}
	else if (CKey::Push('D'))
	{
		mMove += mSideVec;
	}
	if (CKey::Push('W')) {
		mMove += mFrontVec;
	}
	else if (CKey::Push('S'))
	{
		mMove -= mFrontVec;
	}

	//移動量正規化　これをしないと斜め移動が早くなってしまうので注意
	//ジャンプ時などはY軸を正規化しないよう注意
	mMove = mMove.Normalize();
	mMoveKeep = mMove;	//Move保存
	if (mMove.Length() != 0.0f) {
		//平行移動量
		mMove2 = mMove * mSpeed;
	}

	//普通に3次元ベクトル計算で算出したほうが正確だが計算量を懸念する場合は擬似計算で軽量化
	//擬似ベクトル計算
	Check tCheck = CUtil::GetCheck2D(mMove.mX, mMove.mZ, 0, 0, mRotation.mY * (M_PI / 180.0f));

	//回転速度　degreeに直す
	mTurnspeed = (180.0f / M_PI) * 0.5f;

	//急な振り返りを抑制
	if (tCheck.turn > 1.5f) tCheck.turn = 1.5f;

	//移動方向へキャラを向かせる
	if (tCheck.cross > 0.0f) {
		mRotation.mY += tCheck.turn * mTurnspeed;
	}
	if (tCheck.cross < 0.0f) {
		mRotation.mY -= tCheck.turn * mTurnspeed;
	}
}

//ダッシュ処理
void CXPlayer::Dash()
{
	//スタミナが残っているとき
	if (mStamina > 0) {
		mSpeed = SPEED_DASH_HIGH;
		mStamina--;
	}
	//スタミナ切れ状態
	else {
		mSpeed = SPEED_DASH_LOW;
	}

	Move();	//移動処理を呼ぶ
}

//攻撃1処理
void CXPlayer::Attack_1()
{
	if (mAttackFlag_1 == false) {
		ChangeAnimation(3, true, 30);
		mAttackFlag_1 = true;
		mHit = true;
		mGraceTime = 0;
	}

	if (mGraceTime > 0) {
		mGraceTime--;	//受付時間を減少
	}

	if (mAnimationIndex == 3)
	{
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			ChangeAnimation(4, false, 30);
			mGraceTime = GRACETIME;	//受付時間を入れる
		}
	}
	else if (mAnimationIndex == 4)
	{
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mAttackFlag_1 = false;
			mHit = false;
		}
	}
}

//攻撃2処理
void CXPlayer::Attack_2()
{
	if (mAttackFlag_2 == false) {
		ChangeAnimation(7, true, 30);
		mAttackFlag_2 = true;
		mAttack2Speed = ATTACK2_FIRSTSPEED;
		mHit = true;
	}

	if (mAnimationIndex == 7)
	{
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			ChangeAnimation(8, false, 30);
		}
	}
	else if (mAnimationIndex == 8)
	{
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mAttackFlag_2 = false;
			mHit = false;
		}
	}

	mMove2 = mMoveKeep * mAttack2Speed;
	mAttack2Speed = mAttack2Speed * GRAVITY;
}

//攻撃3処理
void CXPlayer::Attack_3()
{
	if (mAttackFlag_3 == false) {
		ChangeAnimation(5, true, 20);
		mAttackFlag_3 = true;
		mHit = true;
	}

	if (mAnimationIndex == 5)
	{
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			ChangeAnimation(6, false, 30);
		}
	}
	else if (mAnimationIndex == 6)
	{
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mAttackFlag_3 = false;
			mAttackFlag_1 = false;
			mHit = false;
		}
	}
}

//回避処理
void CXPlayer::Avoid()
{
	if (mAvoid == false) {
		mAvoid = true;					//回避中
		mStamina -= AVOID_STAMINA;		//スタミナ減少	
		mAvoidTime = AVOID_TIME;		//回避時間
		mAvoidSpeed = AVOID_FIRSTSPEED;	//初速
		mInvincibleFlag = true;			//無敵状態
		mInvincibleTime = INVINCIBLETIME_AVOID;	//無敵時間
	}

	mMove2 = mMoveKeep * mAvoidSpeed;
	//回避時間カウントダウン
	mAvoidTime--;
	mAvoidSpeed = mAvoidSpeed * GRAVITY;	//スピードを下げていく
	//回避時間切れ
	if (mAvoidTime <= 0) {
		mAvoid = false;	//回避終了
	}
}

//死亡処理
void CXPlayer::Death()
{
	ChangeAnimation(11, false, 60);	//倒れるアニメーション
}


