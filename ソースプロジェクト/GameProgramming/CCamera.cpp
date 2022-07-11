#include "CCamera.h"
#include "CKey.h"
#include "glut.h"
#include <corecrt_math.h>
#include <stdio.h>
#include "CInput.h"
#include "main.h"
#include "CCollisionManager.h"
#include "CEnemyManager.h"
#include "CXPlayer.h"
#include "CRes.h"

//カメラの外部変数
CCamera Camera;

#define WIN_CENTRAL_X WINDOW_WIDTH/2
#define WIN_CENTRAL_Y WINDOW_HEIGHT/2

#define CAMERA_SENSITIVITY 0.0025f	//カメラ感度
#define DELAY_RATE 0.3f				//カメラアングル移動時の遅延割合
#define TARGETLOOK_Y 0.9f			//ターゲット時のカメラ高さ
#define ROTATION_RATE 1.0f/15.0f	//回転させたい角度に対する回転する割合

void CCamera::Init()
{
	//カメラのパラメータを作成する
	CVector e, c, u;//視点、注視点、上方向
	//視点を求める
	e = CVector(1.0f, 2.0f, 10.0f);
	//注視点を求める
	c = CVector();
	//上方向を求める
	u = CVector(0.0f, 1.0f, 0.0f);

	//カメラクラスの設定
	Set(e, c, u);
}

float CCamera::mLerp(float start, float point, float rate)
{
	return start + rate * (point - start);

}

void CCamera::SetCameraMode(ECameraMode cameramode)
{
	mCameraMode = cameramode;
}

CCamera::ECameraMode CCamera::GetCameraMode()
{
	return mCameraMode;
}

CCamera::CCamera()
	:mAngleX(0.0f)
	, mAngleY(0.0f)
	, mDist(0.0f)
	, mColliderLine(this, nullptr, CVector(0.0f, 0.0f, 0.0f), CVector(0.0f, 0.0f, 0.0f))
	, mRotRad(0.0f)
	, mIsRot(false)
	, mAngleDelayX(0.0f)
	, mAngleDelayY(0.0f)
	, mCameraMode(NORMAL)
{
	ChangePriority(10);
}

void CCamera::Set(const CVector &eye, const CVector &center,
	const CVector &up) {
	mEye = eye;
	mCenter = center;
	mUp = up;

	mPos = eye;
	mTarget = center;
	mAngleX = 0.0f;
	mAngleY = 1.0f;
	mAngleDelayX = mAngleX;
	mAngleDelayY = mAngleY;
	mDist = DEF_CAMERA_DIST;
}

void CCamera::SetTarget(const CVector& target)
{
	mTarget = target;
}

void CCamera::Update() {
	static int oldMouseX(0), oldMouseY(0);
	int mouseX(0), mouseY(0);
	CInput::GetMousePosW(&mouseX, &mouseY);

	float moveX = (float)(oldMouseX - mouseX);
	float moveY = (float)(oldMouseY - mouseY);

	//マウスを画面中央に固定
	int X = WIN_CENTRAL_X;
	int Y = WIN_CENTRAL_Y;
	CInput::SetMousePosW(X, Y);
	oldMouseX = X;
	oldMouseY = Y;

	/*
	int wheel = CInput::GetWheelValue();
	if (wheel != 0) {
		mDist -= (float)(wheel) * 0.5f;
	}
	*/

	//X軸＋回転
	//if (CKey::Push('K')) {
	//	Matrix = Matrix * CMatrix().RotateX(1);
	//}
	//if (CKey::Push('I')) {
	//	Matrix = Matrix * CMatrix().RotateX(-1);
	//}
	////Y軸＋回転
	//if (CKey::Push('L')) {
	//	Matrix = Matrix * CMatrix().RotateY(1);
	//}
	//if (CKey::Push('J')) {
	//	Matrix = Matrix * CMatrix().RotateY(-1);
	//}

	if (CKey::Push(VK_RIGHT)) {
		mAngleX += 0.1f;
	}
	if (CKey::Push(VK_LEFT)) {
		mAngleX -= 0.1f;
	}
	if (CKey::Push(VK_UP)) {
		mAngleY += 0.01f;
	}
	if (CKey::Push(VK_DOWN)) {
		mAngleY -= 0.01f;
	}

	//カメラのモードを判断
	switch (mCameraMode) {
	case NORMAL: //通常モード
		if (moveX != 0) mAngleDelayX += (moveX * CAMERA_SENSITIVITY);
		if (moveY != 0) mAngleDelayY += (moveY * CAMERA_SENSITIVITY);

		mAngleX = mLerp(mAngleX, mAngleDelayX, DELAY_RATE);
		mAngleY = mLerp(mAngleY, mAngleDelayY, DELAY_RATE);

		//Eキーを押したとき
		if (CKey::Once('E')) {
			mCameraMode = TARGET_LOOK;		//ターゲット状態の敵の方へ向くモードへ移行
			CRes::sSETargetLookOn.Play();	//効果音を再生
		}
		break;

	case TARGET_LOOK: //ターゲット状態の敵の方へ向くモード
		mTargetLook(); //ターゲットになっている敵の方向へカメラを向かせる処理
		//Eキーを押したとき
		if (CKey::Once('E')) {
			mCameraMode = NORMAL;			//ターゲット状態の敵の方へ向くモードへ移行
			CRes::sSETargetLookOff.Play();	//効果音を再生
		}
		break;
	}

	//Y軸制限 0～3.14が180度範囲
	if (mAngleY < 0.05f) mAngleY = 0.05f;
	if (mAngleY > 1.51f) mAngleY = 1.51f;
	if (mAngleDelayY < 0.05f) mAngleDelayY = 0.05f;
	if (mAngleDelayY > 1.51f) mAngleDelayY = 1.51f;

	mPos.mX = mTarget.mX + (sinf(mAngleX)) * (mDist * sinf(mAngleY));
	mPos.mY = mTarget.mY + cosf(mAngleY) * mDist;
	mPos.mZ = mTarget.mZ + (cosf(mAngleX)) * (mDist * sinf(mAngleY));

	mCenter = mTarget;
	mCenter.mY += CAMERA_HEAD_ADJUST;//頭上補正
	mEye = mPos;

	//線コライダセット
	mColliderLine.Set(this, nullptr, mEye, mCenter);

	/*
	oldMouseX = mouseX;
	oldMouseY = mouseY;
	*/
	//CInput::InputReset();
}

void CCamera::Draw() {
	gluLookAt(mEye.mX, mEye.mY, mEye.mZ,
		mCenter.mX, mCenter.mY, mCenter.mZ,
		mUp.mX, mUp.mY, mUp.mZ);

	//カメラ行列格納
	glGetFloatv(GL_MODELVIEW_MATRIX, mMatrix.mF);
}

void CCamera::Render()
{
	//処理なし
}

CMatrix CCamera::GetMat() {
	return mMatrix;
}

bool CCamera::WorldToScreen(CVector* pOut, const CVector& pos)
{
	//座標変換
	CVector	screen_pos = mMatrix * pos;

	//画面外なのでリターン
	if (screen_pos.mZ >= 0.0f) {
		return false;
	}

	float Z = -screen_pos.mZ;
	//座標調整
	screen_pos = screen_pos / -screen_pos.mZ;

	//	printf("%f,%f,%f\n", screen_pos.mX, screen_pos.mY, screen_pos.mZ);

	//スクリーン変換
	pOut->mX = (1.0f + screen_pos.mX) * WINDOW_WIDTH * 0.5f;
	pOut->mY = (1.0f + screen_pos.mY) * WINDOW_HEIGHT * 0.5f;
	pOut->mZ = Z; //screen_pos.mZ

	return true;
}

void CCamera::Collision(CCollider* m, CCollider* o)
{
	if (o->mType == CCollider::ETRIANGLE) {
		CVector adjust;
		if (CCollider::CollisionTriangleLine(o, m, &adjust)) {
			mEye += (adjust + adjust.Normalize() * 0.5f);
			mColliderLine.Set(this, nullptr, mEye, mCenter);
		}
	}
}

void CCamera::TaskCollision()
{
	CCollisionManager::Get()->Collision(&mColliderLine, COLLISIONRANGE);
}

//ターゲットになっている敵の方向へカメラを向かせる処理
void CCamera::mTargetLook()
{
	//nullptrで無ければ通る
	if (CEnemyManager::GetInstance()->GetTargetEnemy()) {
		//プレイヤーに一番近い敵からプレイヤーに伸びるベクトルを求める
		CVector pos = CXPlayer::GetInstance()->mPosition - CEnemyManager::GetInstance()->GetTargetEnemy()->mPosition;
		//posのYは0.0にしておく
		pos.mY = 0.0f;
		//ベクトルを正規化
		pos = pos.Normalize();
		//プレイヤーからの距離を設定
		pos = pos * DEF_CAMERA_DIST;
		//カメラを移動させたい位置を設定
		mPos = CXPlayer::GetInstance()->mPosition + pos;

		//プレイヤーから現在のカメラの位置までのベクトル
		CVector vec1 = CXPlayer::GetInstance()->mPosition - mEye;
		//プレイヤーからカメラを移動させたい位置までのベクトル
		CVector vec2 = CXPlayer::GetInstance()->mPosition - mPos;
		//高さは0にする
		vec1.mY = 0.0f;
		vec2.mY = 0.0f;
		//ベクトルの長さをもとめる
		float len1 = vec1.Length();
		float len2 = vec2.Length();

		float dot = (vec1.mX * vec2.mX) + (vec1.mZ * vec2.mZ);	//内積
		float cross = (vec1.mX * vec2.mZ) - (vec1.mZ * vec2.mX);//外積
		float cos_sita = dot / (len1 * len2);	//コサインシータを求める
		mRotRad = acosf(cos_sita);	//回転させたい角度を設定
		//NaN判定
		if (isnan(mRotRad)) {
			mRotRad = 0.0f;
		}
		//外積で回転させる方向を判断
		if (cross > 0.0f) {
			//アングルXを減算、左方向へ回転
			mAngleX -= mLerp(0.0f, mRotRad, ROTATION_RATE);
		}
		else if (cross < 0.0f) {
			//アングルXを加算、右方向へ回転
			mAngleX += mLerp(0.0f, mRotRad, ROTATION_RATE);
		}
		//アングルYを変更
		mAngleY = mLerp(mAngleY, TARGETLOOK_Y, 0.1f);
	}
	//アングル遅延の値を合わせておく
	mAngleDelayX = mAngleX;
	mAngleDelayY = mAngleY;
}
