#include "CXPlayer.h"
#include "CKey.h"
#include "CCamera.h"
#include "CUtil.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "CInput.h"

#define GRAVITY 0.98f	//重力
#define STAMINA_MAX 100	//スタミナ最大値
#define AVOID_STAMINA 40	//回避時のスタミナの減少量
#define AVOID_TIME 40	//回避時間
#define AVOID_FIRSTSPEED 0.3f	//回避初速
#define SPEED_DASH_HIGH 0.2f	//ダッシュ速度
#define SPEED_DASH_LOW	0.05f;	//ダッシュ速度(スタミナ切れ)

extern int S;	//確認用、後で削除

CXPlayer::CXPlayer()
	: mColSphereBody(this, nullptr, CVector(), 0.5f)
	, mColSphereHead(this, nullptr, CVector(0.0f, 5.0f, -3.0f), 0.5f)
	, mColSphereSword(this, nullptr, CVector(-10.0f, 10.0f, 50.0f), 0.3f)
	, mStamina(STAMINA_MAX)
	, mDash(false)
	, mAvoid(false)
	,mAvoidTime(0)
	,mAvoidSpeed(0)
{
	//タグにプレイヤーを設定します
	mTag = EPLAYER;
	mColSphereSword.mTag = CCollider::ESWORD;
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

	if (mAnimationIndex == 3)
	{
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			ChangeAnimation(4, false, 30);
		}
	}
	else if (mAnimationIndex == 4)
	{
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			ChangeAnimation(0, true, 60);
		}
	}
	else
	{
		//カメラ視点移動　通称無双移動

		//カメラの左右と前後のベクトルを取得
		CVector SideVec = Camera.GetMat().GetXVec();
		CVector FrontVec = Camera.GetMat().GetZVec();
		//高さ移動はカットする
		SideVec.mY = 0.0f;
		FrontVec.mY = 0.0f;
		//正規化する
		SideVec.Normalize();
		FrontVec.Normalize();

		float speed = 0.15f;
		CVector Move(0, 0, 0);

		if (CKey::Push('A'))
		{
			Move -= SideVec;
		}
		else if (CKey::Push('D'))
		{
			Move += SideVec;
		}
		if (CKey::Push('W')) {
			Move += FrontVec;
		}
		else if (CKey::Push('S'))
		{
			Move -= FrontVec;
		}

		if (CInput::GetMouseButton(GLFW_MOUSE_BUTTON_LEFT))
		{
			ChangeAnimation(3, true, 30);
		}
		else if (Move.Length() != 0.0f) {
			ChangeAnimation(1, true, 60);
		}
		else {
			ChangeAnimation(0, true, 60);
		}

		//移動量正規化　これをしないと斜め移動が早くなってしまうので注意
		//ジャンプ時などはY軸を正規化しないよう注意
		Move.Normalize();
		//ダッシュ
		if (CKey::Push(VK_SHIFT) && mAvoid == false) {
			ChangeAnimation(1, true, 60);
			//スタミナが残っているとき
			if (mStamina > 0) {
				speed = SPEED_DASH;
				mStamina--;
			}
			//スタミナ切れ状態
			else {
				speed = 0.05f;
			}
			mDash = true;
		}
		//平行移動量
		Move = Move * speed;

		//普通に3次元ベクトル計算で算出したほうが正確だが計算量を懸念する場合は擬似計算で軽量化
		//擬似ベクトル計算
		Check tCheck = CUtil::GetCheck2D(Move.mX, Move.mZ, 0, 0, mRotation.mY * (M_PI / 180.0f));

		//回転速度　degreeに直す
		float turnspeed = (180.0f / M_PI) * 0.5f;

		//急な振り返りを抑制
		if (tCheck.turn > 1.5f) tCheck.turn = 1.5f;

		//移動方向へキャラを向かせる
		if (tCheck.cross > 0.0f) {
			mRotation.mY += tCheck.turn * turnspeed;
		}
		if (tCheck.cross < 0.0f) {
			mRotation.mY -= tCheck.turn * turnspeed;
		}

		//座標移動
		mPosition += Move;

		/*
		else if (CKey::Push('W'))
		{
			//回避
			if (CKey::Once(VK_SPACE) && mAvoid == false) {
				//スタミナが回避に必要なスタミナ量を超えているとき
				if (mStamina >= AVOID_STAMINA) {
					mAvoid = true;
					mAvoidTime = AVOID_TIME;
					mAvoidSpeed = AVOID_FIRSTSPEED;
					mStamina -= AVOID_STAMINA;
				}
			}
			//ダッシュ
			if (CKey::Push(VK_SHIFT) && mAvoid == false) {
				ChangeAnimation(1, true, 60);
				//スタミナが残っているとき
				if (mStamina > 0) {
					mPosition += CVector(0.0f, 0.0f, 0.2f) * mMatrixRotate;
					mStamina--;
				}
				//スタミナ切れ状態
				else {
					mPosition += CVector(0.0f, 0.0f, 0.05f) * mMatrixRotate;
				}
				mDash = true;
			}
			//通常移動
			if (mDash == false && mAvoid == false) {
				ChangeAnimation(1, true, 60);
				mPosition += CVector(0.0f, 0.0f, 0.1f) * mMatrixRotate;
			}
		}
		else {
			ChangeAnimation(0, true, 60);
		}
		*/
	}

	if (mAvoidTime > 0) {
		mAvoidTime--;
		mPosition += CVector(0.0f, 0.0f, mAvoidSpeed) * mMatrixRotate;
		mAvoidSpeed = mAvoidSpeed * GRAVITY;
	}
	else if (mAvoidTime == 0) {
		mAvoid = false;
	}

	//ダッシュ、回避をしていない状態の時スタミナを回復させる
	if (mDash == false && mAvoid == false && mStamina < STAMINA_MAX) {
		mStamina++;
	}

	mDash = false;

	S = mStamina;	//確認用、後で削除

	//注視点
	Camera.SetTarget(mPosition);

	CXCharacter::Update();
}
