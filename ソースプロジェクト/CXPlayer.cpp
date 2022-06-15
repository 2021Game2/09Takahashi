#include "CXPlayer.h"
#include "CKey.h"
#include "CCamera.h"
#include "CUtil.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "CInput.h"
#include "CTrapManager.h"
#include "CEffect2.h"
#include "CCollisionManager.h"
#include "CEnemyManager.h"
#include "CRes.h"

#define GRAVITY 0.9f			//重力
#define HP_MAX 100				//体力最大値
#define STAMINA_MAX 100			//スタミナ最大値
#define AVOID_STAMINA STAMINA_MAX*0.3f //回避時のスタミナの減少量、スタミナ最大値の3割消費する
#define AVOID_TIME 30			//回避時間
#define AVOID_FIRSTSPEED 0.5f	//回避時の初速
#define SPEED_DEFAULT 0.15f		//スピード(通常時)
#define SPEED_DASH_HIGH 0.2f	//スピード(ダッシュ時)
#define SPEED_DASH_LOW 0.05f	//スピード(ダッシュ時、スタミナ切れ)
#define INVINCIBLETIME_AVOID 30	//無敵時間(回避時)
#define INVINCIBLETIME_DAMAGE 60//無敵時間(ダメージ発生時)
#define KNOCKBACK_AMOUNT 0.1f	//ノックバック時の移動量
#define DAMAGE 20				//ダメージ
#define ATTACK2_FIRSTSPEED 0.6f	//攻撃2使用時の初速
#define GRACETIME 15			//派生攻撃の受付時間
#define COMBO_MAX 4				//攻撃を連続で派生させられる上限

#define PORTION_QUANTITY 5		//回復薬の所持数
#define HEAL_AMOUNT HP_MAX*0.3f	//回復薬を使用したときの回復量、体力最大値の3割回復する

#define GAUGE_WID_MAX 400.0f	//ゲージの幅の最大値
#define GAUGE_LEFT 20			//ゲージ描画時の左端

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
	, mMoveDirKeep(0.0f, 0.0f, 0.0f)
	, mInvincibleFlag(false)
	, mInvincibleTime(0)
	, mMoveDir(0.0f, 0.0f, 0.0f)
	, mMove(0.0f, 0.0f, 0.0f)
	, mSideVec(0.0f, 0.0f, 0.0f)
	, mFrontVec(0.0f, 0.0f, 0.0f)
	, mTurnspeed(0.0f)
	, mAttackFlag_1(false)
	, mAttackFlag_2(false)
	, mAttack2Speed(0.0f)
	, mAttackFlag_3(false)
	, mGraceTime(0)
	, mHit(false)
	, mItemSelect(HEAD + 1)
	, mAttackFlag_Once(false)
	, mComboCount(0)
	, mPortionQuantity(PORTION_QUANTITY)
	, mKnockBackDir(0.0f, 0.0f, 0.0f)
	, mAttackDir(0.0f, 0.0f, 0.0f)
	, mpTargetEnemy(nullptr)
{
	//タグを設定
	mTag = EPLAYER;	//プレイヤー

	//コライダのタグを設定
	mColSphereBody.mTag = CCollider::EBODY;		//体
	mColSphereHead.mTag = CCollider::EHEAD;		//頭
	mColSphereSword.mTag = CCollider::ESWORD;	//剣

	//初期状態を設定
	mState = EIDLE;	//待機状態
}

CXPlayer::~CXPlayer()
{
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

	mPosition.Set(0.0f, 0.0f, 20.0);	//位置を設定
	mRotation.Set(0.0f, 180.0f, 0.0f);	//回転を設定
}

void CXPlayer::Update()
{
	mAttackFlag_Once = false;

	//アイテム選択処理
	ItemSelect();

	//状態を判別
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
			CRes::sSEPlayerWalk.Repeat(); //効果音を再生する
		}
		//右クリックでアイテム使用可能な時アイテムを使用する
		else if (CKey::Once(VK_RBUTTON) && mIsItemUse()) {
			mState = EITEMUSE;
		}
		break;

	case EATTACK_1:	//攻撃1状態の時
		Attack_1();	//攻撃1の処理を呼ぶ
		break;

	case EATTACK_2:	//攻撃2状態の時
		Attack_2();	//攻撃2の処理を呼ぶ
		break;

	case EATTACK_3:	//攻撃3状態の時
		Attack_3();	//攻撃3の処理を呼ぶ
		break;

	case EMOVE:	//移動状態
		//左クリックで攻撃1へ移行
		if (CKey::Once(VK_LBUTTON)) {
			mState = EATTACK_2;
		}
		//SPACEキーを押す＆回避に必要な量のスタミナがあるとき回避へ移行
		else if (CKey::Once(VK_SPACE) && mStamina >= AVOID_STAMINA) {
			mState = EAVOID;
			CRes::sSEPlayerAvoid.Play(); //効果音を再生
		}
		//右クリックでアイテム使用可能な時アイテムを使用する
		else if (CKey::Once(VK_RBUTTON) && mIsItemUse()) {
			mState = EITEMUSE;
		}
		//WASDキーを押すと移動
		else if (CKey::Push('W') || CKey::Push('A') || CKey::Push('S') || CKey::Push('D')) {
			//SHIFTキーを押しているとダッシュへ移行
			if (CKey::Push(VK_SHIFT)) {
				mState = EDASH;
				CRes::sSEPlayerRun.Repeat(); //効果音を再生する
			}
			else {
				Move();	//移動処理を呼ぶ
			}
		}
		//待機状態へ移行
		else {
			mState = EIDLE;
		}
		//状態が移行したとき、効果音を停止する
		if (mState != EMOVE) {
			CRes::sSEPlayerWalk.Stop();
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
			CRes::sSEPlayerAvoid.Play(); //効果音を再生
		}
		//右クリックでアイテム使用可能な時アイテムを使用する
		else if (CKey::Once(VK_RBUTTON) && mIsItemUse()) {
			mState = EITEMUSE;
		}
		//WASDキーを押すと移動
		else if (CKey::Push('W') || CKey::Push('A') || CKey::Push('S') || CKey::Push('D')) {
			//SHIFTキーを押しているとダッシュ
			if (CKey::Push(VK_SHIFT)) {
				Dash();	//ダッシュ処理を呼ぶ
			}
			else {
				mState = EMOVE;
				CRes::sSEPlayerWalk.Play(); //効果音を再生
			}
		}
		//待機状態へ移行
		else {
			mState = EIDLE;
		}
		//状態が移行したとき、効果音を停止する
		if (mState != EDASH) {
			CRes::sSEPlayerRun.Stop();
		}
		break;

	case EAVOID:	//回避状態
		Avoid();	//回避処理を呼ぶ
		//回避状態が終了したとき
		if (mAvoid == false) {
			//回避終了時WASDキーが押されていると移動
			if (CKey::Push('W') || CKey::Push('A') || CKey::Push('S') || CKey::Push('D')) {
				//SHIFTキーを押しているとダッシュへ移行
				if (CKey::Push(VK_SHIFT)) {
					mState = EDASH;
					CRes::sSEPlayerRun.Play(); //効果音を再生
				}
				else {
					mState = EMOVE;
					CRes::sSEPlayerWalk.Play(); //効果音を再生
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

	case EKNOCKBACK: //ノックバック状態
		//効果音を停止する
		CRes::sSEPlayerWalk.Stop();
		CRes::sSEPlayerRun.Stop();
		CRes::sSEPlayerAvoid.Stop();
		KnockBack(); //ノックバック処理を呼ぶ
		break;
	}

	//ダッシュ、回避をしていない状態の時スタミナを回復させる
	if (mState != EDASH && mState != EAVOID && mStamina < STAMINA_MAX) {
		mStamina++;
	}

	//座標移動
	mPosition += mMove;

	//減速させる
	mMove = mMove * GRAVITY; 

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

	//体力が0になると死亡
	if (mHp <= 0) {
		mState = EDEATH;	//死亡状態へ移行
		mHp = 0;
	}

	//注視点
	Camera.SetTarget(mPosition);

	CXCharacter::Update();
}

void CXPlayer::Render2D()
{
	//2D描画開始
	CUtil::Start2D(0, 800, 0, 600);

	float HpRate = (float)mHp / (float)HP_MAX;	//体力最大値に対する、現在の体力の割合
	float HpGaugeWid = GAUGE_WID_MAX * HpRate;	//体力ゲージの幅

	CRes::sImageGauge.Draw(GAUGE_LEFT, GAUGE_LEFT + GAUGE_WID_MAX, 560, 590, 210, 290, 63, 0);	//体力ゲージ背景を表示
	CRes::sImageGauge.Draw(GAUGE_LEFT, GAUGE_LEFT + HpGaugeWid, 560, 590, 0, 0, 0, 0);;			//体力ゲージを表示

	float StaminaRate = (float)mStamina / (float)STAMINA_MAX;	//スタミナ最大値に対する、現在のスタミナの割合
	float StaminaGaugeWid = GAUGE_WID_MAX * StaminaRate;		//スタミナゲージの幅

	CRes::sImageGauge.Draw(GAUGE_LEFT, GAUGE_LEFT + GAUGE_WID_MAX, 520, 550, 210, 290, 63, 0);	//スタミナゲージ背景を表示
	CRes::sImageGauge.Draw(GAUGE_LEFT, GAUGE_LEFT + StaminaGaugeWid, 520, 550, 110, 190, 63, 0);	//スタミナゲージを表示

	char buf[64];
	CRes::sImageGauge.Draw(630, 750, 30, 150, 310, 390, 63, 0);	//アイテム背景を表示
	//選択中のアイテム
	switch (mItemSelect) {	
	case ETRAP: //罠
		CRes::sImageTrap.Draw(640, 740, 40, 140, 0, 255, 255, 0); //罠画像を表示
		sprintf(buf, "%d", CTrapManager::GetInstance()->mTrapQuantity); //罠の所持数
		break;
	case EPORTION: //回復
		CRes::sImagePortion.Draw(640, 740, 40, 140, 0, 255, 255, 0); //回復薬画像を表示
		sprintf(buf, "%d", mPortionQuantity); //回復の所持数
		break;
	}
	//アイテムが使用不可能な時
	if (mIsItemUse() == false) {
		CRes::sImageNixsign.Draw(640, 740, 40, 140, 255, 0, 255, 0); //禁止マーク画像を表示
	}

	CRes::sFont.DrawString(buf, 730, 50, 15, 15); //アイテムの所持数を表示

	//2Dの描画終了
	CUtil::End2D();
}

void CXPlayer::Collision(CCollider* m, CCollider* o)
{
	//死亡状態の時はreturnする
	if (mState == EDEATH) return;

	//自分と相手が球コライダの時
	if (m->mType == CCollider::ESPHERE && o->mType == CCollider::ESPHERE)
	{
		//相手の親のタグが敵
		if (o->mpParent->mTag == EENEMY)
		{
			//敵が死亡状態のときリターンする
			if (((CXEnemy*)(o->mpParent))->mState == CXEnemy::EDEATH)return;
			//相手のコライダのタグが剣
			if (o->mTag == CCollider::ESWORD)
			{
				//プレイヤーが無敵状態ではないとき
				if (mInvincibleFlag == false)
				{
					//キャスト変換
					//敵の攻撃のヒット判定が有効なとき
					if (((CXEnemy*)(o->mpParent))->mHit == true)
					{
						//球コライダ同士の衝突判定
						if (CCollider::Collision(m, o)) {
							//攻撃を受けた箇所
							switch (m->mTag) {
							case CCollider::EHEAD:	//頭
							case CCollider::EBODY:	//体
								mHp -= DAMAGE;		//ダメージを受ける
								((CXEnemy*)(o->mpParent))->mHit = false; //敵の攻撃のヒット判定を終了させる
								mState = EKNOCKBACK;	//ノックバック状態へ移行
								mKnockBackDir = ((CXEnemy*)(o->mpParent))->mPosition - mPosition; //ノックバックする方向を求める
								mInvincibleFlag = true;	//無敵フラグを有効にする
								mHit = false;			//自分の攻撃のヒット判定を無効にする
								mAttackFlag_1 = false;
								mAttackFlag_2 = false;
								mAttackFlag_3 = false;
								mMove = CVector(0.0f, 0.0f, 0.0f);
								CRes::sSEAttackHit2.Play();	//攻撃ヒット時の効果音再生
								break;
							}
						}
					}
				}
			}
			//プレイヤーと敵のすり抜け防止処理
			//自分と相手のコライダのタグが体のとき
			if (m->mTag == CCollider::EBODY && o->mTag == CCollider::EBODY) {
				CVector adjust; //調整用
				//球コライダ同士の衝突判定(Y座標を調整しない)
				if (CCollider::CollisionAdjust(m, o, &adjust)) {
					//敵がスタン状態ではないとき、プレイヤーが攻撃2状態ではないとき
					if (((CXEnemy*)(o->mpParent))->mState != CXEnemy::ESTUN && mState != EATTACK_2) {
						//敵のポジションを調整
						CXEnemy* Enemy = (CXEnemy*)o->mpParent;
						Enemy->SetPos(Enemy->GetPos() + adjust);
					}
					else {
						//プレイヤーのポジションを調整
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
					mPosition += adjust; //ポジションを調整する
				}
			}
		}
	}
}

void CXPlayer::TaskCollision()
{
	CCollisionManager::Get()->Collision(&mColSphereBody, COLLISIONRANGE);
	CCollisionManager::Get()->Collision(&mColSphereHead, COLLISIONRANGE);
	CCollisionManager::Get()->Collision(&mColSphereSword, COLLISIONRANGE);
}

void CXPlayer::Generate()
{
	mInstance = new CXPlayer;
}

void CXPlayer::Release()
{
	if (mInstance) {
		delete mInstance;
		mInstance = nullptr;
	}
}

CXPlayer* CXPlayer::GetInstance()
{
	return mInstance; //インスタンスを返す
}

//剣のコライダの座標を取得する
CVector CXPlayer::GetSwordColPos()
{
	return mColSphereSword.mpMatrix->GetPos();	//剣のコライダの座標を返す
}

//待機処理
void CXPlayer::Idle()
{
	ChangeAnimation(0, true, 60); //待機モーション
	mComboCount = 0; //コンボ数をリセット
}

//移動処理
void CXPlayer::Move()
{
	ChangeAnimation(1, true, 65);
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
	mMoveDirKeep = mMoveDir;	//MoveDir保存
	mMove = mMoveDir * mSpeed;
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
	//攻撃発生時一度だけ通る
	if (mAttackFlag_1 == false) {
		ChangeAnimation(3, true, 20);
		mAttackFlag_1 = true;
		mGraceTime = 0;
		mAttackFlag_Once = true;
		mComboCount++; //コンボ数加算
		mpTargetEnemy = CEnemyManager::GetInstance()->GetNearEnemy(); //プレイヤーに一番近い敵を取得する
		if (mpTargetEnemy) {
			mAttackDir = mpTargetEnemy->mPosition - mPosition; //攻撃時の向きを求める
			mMoveDirKeep = mAttackDir.Normalize();
		}
		else {
			mAttackDir = mMoveDirKeep.Normalize();
		}
	}

	if (mGraceTime > 0) {
		mGraceTime--;	//受付時間を減少
	}

	if (mAnimationIndex == 3)
	{
		//攻撃方向に移動させる
		mPosition += mAttackDir.Normalize() * 0.04f;
		//ヒット判定発生
		if (mAnimationFrame == 5) {
			mHit = true;
		}
		//アニメーション終了時
		if (mIsAnimationEnd())
		{
			mHit = false; //ヒット判定終了
			ChangeAnimation(4, false, 30);
			mGraceTime = GRACETIME;	//受付時間を入れる
		}
	}
	else if (mAnimationIndex == 4)
	{
		//アニメーションの再生フレーム数が派生攻撃の受付時間内のとき
		if (mAnimationFrame < GRACETIME) {
			//左クリックをしているとき
			if (CKey::Once(VK_LBUTTON)) {
				//コンボ数がコンボの上限回数に達していないとき
				if (mComboCount < COMBO_MAX) {
					mState = EATTACK_3; //攻撃3状態へ移行
				}
				else {
					mState = EATTACK_2; //攻撃2状態へ移行
				}
				mAttackFlag_1 = false;
			}
		}
		//アニメーション終了時
		if (mIsAnimationEnd())
		{
			mState = EIDLE; //待機状態へ移行
			mAttackFlag_1 = false;
		}
	}

	mMoveDir = mAttackDir.Normalize(); //攻撃時の向きを正規化して代入する
}

//攻撃2処理
void CXPlayer::Attack_2()
{
	//攻撃発生時一度だけ通る
	if (mAttackFlag_2 == false) {
		ChangeAnimation(7, true, 30);
		mAttackFlag_2 = true;
		mAttack2Speed = ATTACK2_FIRSTSPEED;
		mAttackFlag_Once = true;
		mpTargetEnemy = CEnemyManager::GetInstance()->GetNearEnemy(); //プレイヤーに一番近い敵を取得する
	}

	if (mAnimationIndex == 7)
	{
		//敵を追尾させる
		if (mpTargetEnemy) {
			mAttackDir = mpTargetEnemy->mPosition - mPosition; //向きを求める
			mMoveDirKeep = mAttackDir.Normalize();
		}
		else {
			mAttackDir = mMoveDirKeep.Normalize();
		}
		//ヒット判定発生
		if (mAnimationFrame == 8) {
			mHit = true;
		}
		//アニメーション終了時
		if (mIsAnimationEnd())
		{
			mHit = false; //ヒット判定終了
			ChangeAnimation(8, false, 30);
		}
	}
	else if (mAnimationIndex == 8)
	{
		//アニメーション終了時
		if (mIsAnimationEnd())
		{
			mState = EIDLE; //待機状態へ移行
			mAttackFlag_2 = false;
		}
	}

	mMoveDir = mAttackDir.Normalize(); //攻撃時の向きを正規化して代入する

	mMove = mMoveDir * mAttack2Speed;
	mAttack2Speed = mAttack2Speed * GRAVITY;
}

//攻撃3処理
void CXPlayer::Attack_3()
{
	//攻撃時一度だけ通る
	if (mAttackFlag_3 == false) {
		ChangeAnimation(5, true, 15);
		mAttackFlag_3 = true;
		mAttackFlag_Once = true;
		mComboCount++; //コンボ数加算
		mpTargetEnemy = CEnemyManager::GetInstance()->GetNearEnemy(); //プレイヤーに一番近い敵を取得する
		if (mpTargetEnemy) {
			mAttackDir = mpTargetEnemy->mPosition - mPosition; //攻撃時の向きを求める
			mMoveDirKeep = mAttackDir.Normalize();
		}
		else {
			mAttackDir = mMoveDirKeep.Normalize();
		}
	}

	if (mAnimationIndex == 5)
	{
		//攻撃方向に移動させる
		mPosition += mAttackDir.Normalize() * 0.03f;
		//ヒット判定発生
		if (mAnimationFrame == 0) {
			mHit = true;
		}
		//アニメーション終了時
		if (mIsAnimationEnd())
		{
			mHit = false; //ヒット判定終了
			ChangeAnimation(6, false, 30);
		}
	}
	else if (mAnimationIndex == 6)
	{
		//アニメーションの再生フレーム数が派生攻撃の受付時間内のとき
		if (mAnimationFrame < GRACETIME) {
			//左クリックをしているとき
			if (CKey::Once(VK_LBUTTON)) {
				//コンボ数がコンボの上限回数に達していないとき
				if (mComboCount < COMBO_MAX) {
					mState = EATTACK_1; //攻撃1状態へ移行
				}
				else {
					mState = EATTACK_2; //攻撃2状態へ移行
				}
				mAttackFlag_3 = false;
			}
		}
		//アニメーション終了時
		if (mIsAnimationEnd())
		{
			mState = EIDLE; //待機状態へ移行
			mAttackFlag_3 = false;
		}
	}

	mMoveDir = mAttackDir.Normalize(); //攻撃時の向きを正規化して代入する
}

//回避処理
void CXPlayer::Avoid()
{
	//回避時一度だけ通る
	if (mAvoid == false) {
		mAvoid = true;							//回避中
		mStamina -= AVOID_STAMINA;				//スタミナ減少	
		mAvoidTime = AVOID_TIME;				//回避時間
		mAvoidSpeed = AVOID_FIRSTSPEED;			//初速設定
		mInvincibleFlag = true;					//無敵状態
		mInvincibleTime = INVINCIBLETIME_AVOID;	//無敵時間
	}

	//無敵時間をカウントダウン
	mInvincibleTime--;
	//無敵時間が0になったとき
	if (mInvincibleTime == 0) {
		mInvincibleFlag = false; //無敵状態を終了する
	}

	//回避時間をカウントダウン
	mAvoidTime--;
	//回避時間0になった時
	if (mAvoidTime == 0) {
		mAvoid = false;	//回避終了
	}

	mMove = mMoveDirKeep * mAvoidSpeed;
	mAvoidSpeed = mAvoidSpeed * GRAVITY;	//スピードを下げていく
}

//死亡処理
void CXPlayer::Death()
{
	ChangeAnimation(11, false, 50);	//倒れるアニメーション
}

//ノックバック処理
void CXPlayer::KnockBack()
{
	ChangeAnimation(2, false, 20);

	//ノックバック方向正規化
	mKnockBackDir = mKnockBackDir.Normalize();

	//攻撃を当ててきた敵の方を向かせるために代入する
	mMoveDir = mKnockBackDir;

	//ノックバックさせる
	mPosition -= mKnockBackDir * KNOCKBACK_AMOUNT;

	if (mAnimationIndex == 2)
	{
		//アニメーション終了時
		if (mIsAnimationEnd())
		{
			mState = EIDLE;	//待機状態へ移行
			mInvincibleFlag = false; //無敵状態を終了する
		}
	}
}

//アイテムが使用可能か判断する
bool CXPlayer::mIsItemUse()
{
	//選択中のアイテム
	switch (mItemSelect) {
	case ETRAP:	//罠
		//罠アイテムが使用可能な時
		if (CTrapManager::GetInstance()->TrapAvailable()) {
			return true;
		}
		break;

	case EPORTION: //回復薬
		//回復薬の所持数が0より多いとき、現在の体力が体力最大値を下回っているとき
		if (mPortionQuantity > 0 && mHp < HP_MAX) {
			return true;
		}
		break;
	}
	return false;
}

//アイテム使用処理
void CXPlayer::ItemUse()
{
	//選択中のアイテム
	switch (mItemSelect) {
	case ETRAP:	//罠
		//罠生成
		CTrapManager::GetInstance()->TrapGenerate(mPosition, mRotation);
		//罠アイテム使用時の効果音を再生する
		CRes::sSETrapUse.Play();
		break;

	case EPORTION: //回復薬
		//回復薬の所持数を減らす
		mPortionQuantity--;
		//体力を回復させる
		mHp += HEAL_AMOUNT;
		//現在の体力が体力最大値を超えないようにする
		if (mHp > HP_MAX)mHp = HP_MAX;
		//回復アイテム使用時のエフェクトを生成する
		new CEffect2(mPosition, 2.0f, 2.0f, "", 2, 5, 3);
		//回復アイテム使用時の効果音再生
		CRes::sSEPortionUse.Play();
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
		}
		//下方向
		else {
			mItemSelect--;
		}
	}

	if (mItemSelect == TAIL) {
		mItemSelect = HEAD + 1;
	}
	if (mItemSelect == HEAD) {
		mItemSelect = TAIL - 1;
	}
}