#include "CXPlayer.h"
#include "CKey.h"
#include "CCamera.h"
#include "CUtil.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "CInput.h"
#include "CXEnemy.h"
#include "CTrapManager.h"

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
#define GRACETIME 15			//派生攻撃の受付時間
#define COMBO_MAX 4				//攻撃を連続で派生させられる上限

#define PORTION_QUANTITY 5		//回復薬の所持数
#define HEAL_AMOUNT 30			//回復薬を使用したときの回復量

#define GAUGE_WID_MAX 400.0f	//ゲージの幅の最大値
#define GAUGE_LEFT 20			//ゲージ描画時の左端

#define FONT "Resource\\FontG.png" //フォント
#define IMAGE_GAUGE "Resource\\Gauge.png"		//ゲージ画像
#define IMAGE_PORTION "Resource\\Portion.png"	//回復薬画像
#define IMAGE_TRAP "Resource\\Trap.png"			//罠画像

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
	, mGraceTime(0)
	, mHit(false)
	, mItemSelect(HEAD + 1)
	, mAttackFlag_Once(false)
	,mCombo(0)
	,mPortionQuantity(PORTION_QUANTITY)
{
	//タグにプレイヤーを設定します
	mTag = EPLAYER;
	mColSphereBody.mTag = CCollider::EBODY;
	mColSphereHead.mTag = CCollider::EHEAD;
	mColSphereSword.mTag = CCollider::ESWORD;

	mState = EIDLE;	//待機状態

	mInstance = this;

	mFont.LoadTexture(FONT, 1, 4096 / 64);

	mImageGauge.Load(IMAGE_GAUGE);
	mImagePortion.Load(IMAGE_PORTION);
	mImageTrap.Load(IMAGE_TRAP);
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
		break;

	case EATTACK_2:	//攻撃2状態
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

	case EKNOCKBACK: //ノックバック状態
		KnockBack(); //ノックバック処理を呼ぶ
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

	mImageGauge.Draw(GAUGE_LEFT, GAUGE_LEFT + GAUGE_WID_MAX, 560, 590, 210, 290, 63, 0);	//体力ゲージ背景を表示
	mImageGauge.Draw(GAUGE_LEFT, GAUGE_LEFT + HpGaugeWid, 560, 590, 0, 0, 0, 0);;			//体力ゲージを表示

	float StaminaRate = (float)mStamina / (float)STAMINA_MAX;	//スタミナ最大値に対する、現在のスタミナの割合
	float StaminaGaugeWid = GAUGE_WID_MAX * StaminaRate;		//スタミナゲージの幅

	mImageGauge.Draw(GAUGE_LEFT, GAUGE_LEFT + GAUGE_WID_MAX, 520, 550, 210, 290, 63, 0);	//スタミナゲージ背景を表示
	mImageGauge.Draw(GAUGE_LEFT, GAUGE_LEFT + StaminaGaugeWid, 520, 550, 110, 190, 63, 0);	//スタミナゲージを表示

	char buf[64];
	mImageGauge.Draw(640, 760, 40, 160, 310, 390, 63, 0);	//アイテム背景を表示
	//選択中のアイテム
	switch (mItemSelect) {	
	case ETRAP: //罠
		mImageTrap.Draw(650, 750, 50, 150, 0, 255, 255, 0); //罠画像を表示
		sprintf(buf, "%d", CTrapManager::GetInstance()->mTrapQuantity); //罠の所持数
		break;
	case EPORTION: //回復
		mImagePortion.Draw(650, 750, 50, 150, 0, 255, 255, 0); //回復薬画像を表示
		sprintf(buf, "%d", mPortionQuantity); //回復の所持数
		break;
	}
	mFont.DrawString(buf, 740, 60, 15, 15); //アイテムの所持数を表示

#ifdef _DEBUG
	//char buf[64];

	sprintf(buf, "COMBO:%d", mCombo);
	mFont.DrawString(buf, 50, 250, 10, 12);

	sprintf(buf, "INVICIBLETIME:%d", mInvincibleTime);
	mFont.DrawString(buf, 50, 150, 10, 12);
#endif

	//2Dの描画終了
	CUtil::End2D();
}

void CXPlayer::Collision(CCollider* m, CCollider* o)
{
	//死亡状態の時はreturnする
	if (mState == EDEATH) return;

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
									mState = EKNOCKBACK; //ノックバック状態へ移行
									mHit = false; //自分の攻撃のヒット判定を無効にする
									mAttackFlag_1 = false;
									mAttackFlag_2 = false;
									mAttackFlag_3 = false;
									mMove2 = CVector(0.0f, 0.0f, 0.0f);
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
						else {
							//プレイヤーのポジションを調整
							mPosition -= adjust;
						}
					}
				}
			}
		}
		//相手が三角コライダ
		if (o->mType == CCollider::ETRIANGLE)
		{
			//相手の親のタグがマップ
			if (o->mpParent->mTag == EMAP)
			{
				//自分のコライダのタグが頭or体
				if (m->mTag == CCollider::EHEAD || m->mTag == CCollider::EBODY) {
					CVector adjust;
					if (CCollider::CollisionTriangleSphere(o, m, &adjust)) {
						mPosition += adjust;
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
	mCombo = 0;
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
		mCombo++;
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
		if (mAnimationFrame < GRACETIME) {
			if (CKey::Once(VK_LBUTTON)) {
				if (mCombo < COMBO_MAX) {
					mState = EATTACK_3;
				}
				else {
					mState = EATTACK_2;
				}
				mAttackFlag_1 = false;
			}
		}
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mState = EIDLE;
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
		if (mAnimationFrame == 8) {
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
			mState = EIDLE;
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
		mCombo++;
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
		if (mAnimationFrame < GRACETIME) {
			if (CKey::Once(VK_LBUTTON)) {
				if (mCombo < COMBO_MAX) {
					mState = EATTACK_1;
				}
				else {
					mState = EATTACK_2;
				}
				mAttackFlag_3 = false;
			}
		}
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mState = EIDLE;
			mAttackFlag_3 = false;
			//mHit = false;
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
	ChangeAnimation(11, false, 50);	//倒れるアニメーション
}

//ノックバック処理
void CXPlayer::KnockBack()
{
	ChangeAnimation(2, false, 20);

	//ノックバック方向
	mMoveDir = CXEnemy::GetInstance()->mPosition - mPosition;
	mMoveDir = mMoveDir.Normalize();

	//ノックバック量
	float KnockBackAmount = 0.1f;

	//ノックバックさせる
	mPosition -= mMoveDir * KnockBackAmount;

	if (mAnimationIndex == 2)
	{
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mState = EIDLE;
		}
	}
}

//アイテム使用処理
void CXPlayer::ItemUse()
{
	switch (mItemSelect) {
	case ETRAP:	//罠
		//罠生成
		CTrapManager::GetInstance()->TrapGenerate(mPosition, mRotation);
		break;

	case EPORTION: //回復薬
		//回復薬の所持数が0より多いとき、現在の体力が体力最大値を下回っているとき
		if (mPortionQuantity > 0 && mHp < HP_MAX) {
			//回復薬の所持数を減らす
			mPortionQuantity--;
			//体力を回復させる
			mHp += HEAL_AMOUNT;
		}
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


