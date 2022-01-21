#include "CCamera.h"
#include "CKey.h"
#include "glut.h"
#include <corecrt_math.h>
#include <stdio.h>
#include "CInput.h"
#include "main.h"

//カメラの外部変数
CCamera Camera;

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

void CCamera::Set(const CVector &eye, const CVector &center,
	const CVector &up) {
	mEye = eye;
	mCenter = center;
	mUp = up;

	mPos = eye;
	mTarget = center;
	mAngleX = 0.0f;
	mAngleY = 1.0f;
	mDist = DEF_CAMERA_DIST;
}

void CCamera::SetTarget(const CVector& target)
{
	mTarget = target;
}

void CCamera::Update() {
	static int oldMouseX(0), oldMouseY(0);
	int mouseX(0), mouseY(0);
	CInput::GetMousePos(&mouseX, &mouseY);


	float moveX = (float)(oldMouseX - mouseX);
	float moveY = (float)(oldMouseY - mouseY);


	if (CInput::GetMouseButton(GLFW_MOUSE_BUTTON_RIGHT)) {
		if (moveX != 0) mAngleX += (moveX * 0.01f);
		if (moveY != 0) mAngleY += (moveY * 0.01f);
	}

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

	//Y軸制限 0～3.14が180度範囲
	if (mAngleY < 0.05f) mAngleY = 0.05f;
	if (mAngleY > 1.51f) mAngleY = 1.51f;

	mPos.mX = mTarget.mX + (sinf(mAngleX)) * (mDist * sinf(mAngleY));
	mPos.mY = mTarget.mY + cosf(mAngleY) * mDist;
	mPos.mZ = mTarget.mZ + (cosf(mAngleX)) * (mDist * sinf(mAngleY));

	mCenter = mTarget;
	mCenter.mY += DEF_CAMERA_HEAD_ADJUST;//頭上補正
	mEye = mPos;

	oldMouseX = mouseX;
	oldMouseY = mouseY;
	CInput::InputReset();

}

void CCamera::Render() {
	gluLookAt(mEye.mX, mEye.mY, mEye.mZ,
		mCenter.mX, mCenter.mY, mCenter.mZ,
		mUp.mX, mUp.mY, mUp.mZ);

	//カメラ行列格納
	glGetFloatv(GL_MODELVIEW_MATRIX, mMatrix.mF);
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
