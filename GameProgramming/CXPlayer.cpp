#include "CXPlayer.h"
#include "CKey.h"
#include "CCamera.h"
#include "CUtil.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "CInput.h"
#include "CXEnemy.h"
#include "CTrap.h"

#define GRAVITY 0.9f			//重力
#define HP_MAX 100				//体力最大値
#define STAMINA_MAX 100			//スタミナ最大値
#define AVOID_STAMINA 30		//回避時のスタミナの減少量
#define AVOID_TIME 30			//回避時間
#define AVOID_FIRSTSPEED 0.5f	//回避時の初速
#define SPEED_DEFAULT 0.15f		//スピード(通常時)
#define SPEED_DASH_HIGH 0.2f	//スピード(ダッシュ時)
#define SPEED_DASH_LOW 0.05f	//スピード(ダッシュ時、スタミナ切れ)
#define INVINCIBLETIME_AVOID 30	//無敵時間(回避時)
#define INVINCIBLETIME_DAMAGE 60//無敵時間(ダメージ発生時)
#define DAMAGE 20				//ダメージ
#define ATTACK2_FIRSTSPEED 0.6f	//攻撃2使用時の初速
#define GRACETIME 10			//派生攻撃の受付時間
#define HEAL_AMOUNT HP_MAX		//回復量

#define GAUGE_WID_MAX 350.0f	//ゲージの幅の最大値

CXPlayer* CXPlayer::mInstance;

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
	, mMoveDir(0.0f, 0.0f, 0.0f)
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
	,mItemSelect(EEMPTY)
	,mAttackFlag_Once(false)
{
	//タグにプレイヤーを設定します
	mTag = EPLAYER;
	mColSphereBody.mTag = CCollider::EBODY;
	mColSphereHead.mTag = CCollider::EHEAD;
	mColSphereSword.mTag = CCollider::ESWORD;

	mState = EIDLE;	//待機状態

	mInstance = this;

	mFont.LoadTexture("FontG.png", 1, 4096 / 64);

	mTexture.Load("Gauge.png");
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
	mAttackFlag_Once = false;

	ItemSelect();

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
		//Eキーを押すとアイテム使用
		else if (CKey::Once('E')) {
			mState = EITEMUSE;
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
		//SPACEキーを押す＆回避に必要な量のスタミナがあるとき回避へ移行
		else if (CKey::Once(VK_SPACE) && mStamina >= AVOID_STAMINA) {
			mState = EAVOID;
		}
		//Eキーを押すとアイテム使用
		else if (CKey::Once('E')) {
			mState = EITEMUSE;
		}
		//WASDキーを押すと移動
		else if (CKey::Push('W') || CKey::Push('A') || CKey::Push('S') || CKey::Push('D')) {
			//SHIFTキーを押しているとダッシュへ移行
			if (CKey::Push(VK_SHIFT)) {
				mState = EDASH;
			}
			else {
				Move();	//移動処理を呼ぶ
			}
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
		//Eキーを押すとアイテム使用
		else if (CKey::Once('E')) {
			mState = EITEMUSE;
		}
		//WASDキーを押すと移動
		else if (CKey::Push('W') || CKey::Push('A') || CKey::Push('S') || CKey::Push('D')) {
			//SHIFTキーを押しているとダッシュ
			if (CKey::Push(VK_SHIFT)) {
				Dash();	//ダッシュ処理を呼ぶ
			}
			else{
				mState = EMOVE;
			}
		}
		//待機状態へ移行
		else {
			mState = EIDLE;
		}
		break;

	case EAVOID:	//回避状態
		Avoid();	//回避処理を呼ぶ
		if (mAvoid == false) {
			//回避終了時WASDキーが押されていると移動
			if (CKey::Push('W') || CKey::Push('A') || CKey::Push('S') || CKey::Push('D')) {
				//SHIFTキーを押しているとダッシュへ移行
				if (CKey::Push(VK_SHIFT)) {
					mState = EDASH;
				}
				else {
					mState = EMOVE;
				}
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

	case EITEMUSE:	//アイテム使用中
		ItemUse();	//アイテム使用処理を呼ぶ
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

	mMove2 = mMove2 * GRAVITY;

	//普通に3次元ベクトル計算で算出したほうが正確だが計算量を懸念する場合は擬似計算で軽量化
	//擬似ベクトル計算
	Check tCheck = CUtil::GetCheck2D(mMoveDir.mX, mMoveDir.mZ, 0, 0, mRotation.mY * (M_PI / 180.0f));

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

	//リセット
	mMoveDir = CVector(0.0f, 0.0f, 0.0f);
	mSpeed = 0.0f;

	//体力が0になると死亡
	if (mHp <= 0) {
		mState = EDEATH;	//死亡状態へ移行
		mHp = 0;
	}

	if (mHp > HP_MAX)mHp = HP_MAX;

	//後で削除する
	//////////////////////////////
#ifdef _DEBUG
	//リターンを押すと復活
	if (mHp<=0&&CKey::Once(VK_RETURN)) {
		mHp = HP_MAX;
		mState = EIDLE;
	}
#endif
	//////////////////////////////

	//注視点
	Camera.SetTarget(mPosition);

	CXCharacter::Update();
}

void CXPlayer::Render2D()
{
	//2D描画開始
	CUtil::Start2D(0, 800, 0, 600);
	/*
	//体力が減ると幅が減少する、左揃え
	mFont.DrawString("0", 400 - GAUGE_WID_MAX * (1.0f - hpRate), 575.0f, hpGaugeWid, 15);
	//スタミナが減ると幅が減少する、左揃え
	mFont.DrawString("1", 400 - GAUGE_WID_MAX * (1.0f - staminaRate), 535.0f, staminaGaugeWid, 15);
	*/

	//体力の割合
	float hpRate = (float)mHp / (float)HP_MAX;
	//体力ゲージの幅
	float hpGaugeWid = GAUGE_WID_MAX * hpRate;

	//スタミナの割合
	float staminaRate = (float)mStamina / (float)STAMINA_MAX;
	//スタミナゲージの幅
	float staminaGaugeWid = GAUGE_WID_MAX * staminaRate;

	mTexture.Draw(20, GAUGE_WID_MAX, 560, 590, 210, 290, 63, 0);	//ゲージ背景
	mTexture.Draw(20, hpGaugeWid, 560, 590, 0, 0, 0, 0);			//体力ゲージ

	mTexture.Draw(20, GAUGE_WID_MAX, 520, 550, 210, 290, 63, 0);	//ゲージ背景
	mTexture.Draw(20, staminaGaugeWid, 520, 550, 110, 190, 63, 0);	//スタミナゲージ

#ifdef _DEBUG
	char buf[64];

	sprintf(buf, "INVICIBLETIME:%d", mInvincibleTime);
	mFont.DrawString(buf, 50, 150, 10, 12);

	switch (mItemSelect) {
	case 1:
		sprintf(buf, "SELECTITEM:EMPTY");
		break;
	case 2:
		sprintf(buf, "SELECTITEM:TRAP");
		break;
	case 3:
		sprintf(buf, "SELECTITEM:PORTION");
		break;
	}
	mFont.DrawString(buf, 50, 50, 10, 12);
#endif

	//2Dの描画終了
	CUtil::End2D();
}

void CXPlayer::Collision(CCollider* m, CCollider* o)
{
	//自分が球コライダ
	if (m->mType == CCollider::ESPHERE) 
	{
		//相手が球コライダ
		if (o->mType == CCollider::ESPHERE) 
		{
			//相手の親のタグが敵
			if (o->mpParent->mTag == EENEMY) 
			{
				//相手のコライダのタグが剣
				if (o->mTag == CCollider::ESWORD) 
				{
					//無敵状態ではないとき
					if (mInvincibleFlag == false) 
					{
						//衝突判定
						if (CCollider::Collision(m, o)) {
							//キャスト変換
							//敵の攻撃のヒット判定が有効なとき
							if (((CXEnemy*)(o->mpParent))->mHit == true)
							{
								//攻撃を受けた箇所
								switch (m->mTag) {
								case CCollider::EHEAD:	//頭
								case CCollider::EBODY:	//体
									mHp -= DAMAGE;		//ダメージを受ける
									((CXEnemy*)(o->mpParent))->mHit = false; //敵の攻撃のヒット判定を終了させる
									break;
								}
							}
						}
					}
				}
				//すり抜け防止
				//プレイヤーのボディと敵のボディが当たっているとき
				if (m->mTag == CCollider::EBODY && o->mTag == CCollider::EBODY) {
					CVector adjust;
					if (CCollider::CollisionAdjust(m, o, &adjust)) {
						//敵がスタン状態ではないとき、プレイヤーが攻撃2状態ではないとき
						if (((CXEnemy*)(o->mpParent))->mState != CXEnemy::ESTUN && mState != EATTACK_2) {
							//敵のポジションを調整
							CXEnemy* Enemy = (CXEnemy*)o->mpParent;
							Enemy->SetPos(Enemy->GetPos() + adjust);
						}
						else{
							//プレイヤーのポジションを調整
							mPosition -= adjust;
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
		mMoveDir -= mSideVec;
	}
	else if (CKey::Push('D'))
	{
		mMoveDir += mSideVec;
	}
	if (CKey::Push('W')) {
		mMoveDir += mFrontVec;
	}
	else if (CKey::Push('S'))
	{
		mMoveDir -= mFrontVec;
	}

	//移動量正規化　これをしないと斜め移動が早くなってしまうので注意
	//ジャンプ時などはY軸を正規化しないよう注意
	mMoveDir = mMoveDir.Normalize();
	mMoveKeep = mMoveDir;	//MoveDir保存
	mMove2 = mMoveDir * mSpeed;
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
		ChangeAnimation(3, true, 20);
		mAttackFlag_1 = true;
		mGraceTime = 0;
		mAttackFlag_Once = true;
	}

	if (mGraceTime > 0) {
		mGraceTime--;	//受付時間を減少
	}

	if (mAnimationIndex == 3)
	{
		//ヒット判定発生
		if (mAnimationFrame == 5) {
			mHit = true;
		}
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mHit = false; //ヒット判定終了
			ChangeAnimation(4, false, 30);
			mGraceTime = GRACETIME;	//受付時間を入れる
		}
	}
	else if (mAnimationIndex == 4)
	{
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mAttackFlag_1 = false;
		}
	}

	mMoveDir = CXEnemy::GetInstance()->mPosition - mPosition;
	mMoveDir = mMoveDir.Normalize();
}

//攻撃2処理
void CXPlayer::Attack_2()
{
	if (mAttackFlag_2 == false) {
		ChangeAnimation(7, true, 30);
		mAttackFlag_2 = true;
		mAttack2Speed = ATTACK2_FIRSTSPEED;
		mAttackFlag_Once = true;
	}

	if (mAnimationIndex == 7)
	{
		//ヒット判定発生
		if (mAnimationFrame == 5) {
			mHit = true;
		}
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mHit = false; //ヒット判定終了
			ChangeAnimation(8, false, 30);
		}
	}
	else if (mAnimationIndex == 8)
	{
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mAttackFlag_2 = false;
			//mHit = false;
		}
	}

	//移動量正規化　これをしないと斜め移動が早くなってしまうので注意
	//ジャンプ時などはY軸を正規化しないよう注意
	mMoveDir = CXEnemy::GetInstance()->mPosition - mPosition;
	mMoveDir = mMoveDir.Normalize();

	mMove2 = mMoveDir * mAttack2Speed;
	mAttack2Speed = mAttack2Speed * GRAVITY;
}

//攻撃3処理
void CXPlayer::Attack_3()
{
	if (mAttackFlag_3 == false) {
		ChangeAnimation(5, true, 15);
		mAttackFlag_3 = true;
		mAttackFlag_Once = true;
	}

	if (mAnimationIndex == 5)
	{
		//ヒット判定発生
		if (mAnimationFrame == 0) {
			mHit = true;
		}
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mHit = false; //ヒット判定終了
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
		mAvoid = true;							//回避中
		mStamina -= AVOID_STAMINA;				//スタミナ減少	
		mAvoidTime = AVOID_TIME;				//回避時間
		mAvoidSpeed = AVOID_FIRSTSPEED;			//初速
		mInvincibleFlag = true;					//無敵状態
		mInvincibleTime = INVINCIBLETIME_AVOID;	//無敵時間
	}

	mMove2 = mMoveKeep * mAvoidSpeed;
	mAvoidSpeed = mAvoidSpeed * GRAVITY;	//スピードを下げていく

	//回避時間カウントダウン
	mAvoidTime--;
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

//アイテム使用処理
void CXPlayer::ItemUse()
{
	switch (mItemSelect) {
	case EEMPTY: //空
		break;

	case ETRAP:	//罠
	{
		//罠生成
		CTrap* trap = new CTrap;
		trap->SetPos(mPosition);
		trap->SetRot(mRotation);
		trap->Update();
	}
	break;

	case EPORTION: //回復
		//体力回復
		mHp += HEAL_AMOUNT;
		break;
	}
	//待機状態へ移行
	mState = EIDLE;
}

//アイテム選択処理
void CXPlayer::ItemSelect()
{
	//ホイ―ル移動量取得
	int wheel = CInput::GetWheelValue();
	//ホイールを動かしているとき
	if (wheel != 0) {
		//上方向
		if (wheel > 0) {
			mItemSelect++;
			if (mItemSelect == mItemTail) {
				mItemSelect = mItemHead + 1;
			}
		}
		//下方向
		else {
			mItemSelect--;
			if (mItemSelect == mItemHead) {
				mItemSelect = mItemTail - 1;
			}
		}
	}
}


