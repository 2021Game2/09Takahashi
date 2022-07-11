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

//�J�����̊O���ϐ�
CCamera Camera;

#define WIN_CENTRAL_X WINDOW_WIDTH/2
#define WIN_CENTRAL_Y WINDOW_HEIGHT/2

#define CAMERA_SENSITIVITY 0.0025f	//�J�������x
#define DELAY_RATE 0.3f				//�J�����A���O���ړ����̒x������
#define TARGETLOOK_Y 0.9f			//�^�[�Q�b�g���̃J��������
#define ROTATION_RATE 1.0f/15.0f	//��]���������p�x�ɑ΂����]���銄��

void CCamera::Init()
{
	//�J�����̃p�����[�^���쐬����
	CVector e, c, u;//���_�A�����_�A�����
	//���_�����߂�
	e = CVector(1.0f, 2.0f, 10.0f);
	//�����_�����߂�
	c = CVector();
	//����������߂�
	u = CVector(0.0f, 1.0f, 0.0f);

	//�J�����N���X�̐ݒ�
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

	//�}�E�X����ʒ����ɌŒ�
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

	//X���{��]
	//if (CKey::Push('K')) {
	//	Matrix = Matrix * CMatrix().RotateX(1);
	//}
	//if (CKey::Push('I')) {
	//	Matrix = Matrix * CMatrix().RotateX(-1);
	//}
	////Y���{��]
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

	//�J�����̃��[�h�𔻒f
	switch (mCameraMode) {
	case NORMAL: //�ʏ탂�[�h
		if (moveX != 0) mAngleDelayX += (moveX * CAMERA_SENSITIVITY);
		if (moveY != 0) mAngleDelayY += (moveY * CAMERA_SENSITIVITY);

		mAngleX = mLerp(mAngleX, mAngleDelayX, DELAY_RATE);
		mAngleY = mLerp(mAngleY, mAngleDelayY, DELAY_RATE);

		//E�L�[���������Ƃ�
		if (CKey::Once('E')) {
			mCameraMode = TARGET_LOOK;		//�^�[�Q�b�g��Ԃ̓G�̕��֌������[�h�ֈڍs
			CRes::sSETargetLookOn.Play();	//���ʉ����Đ�
		}
		break;

	case TARGET_LOOK: //�^�[�Q�b�g��Ԃ̓G�̕��֌������[�h
		mTargetLook(); //�^�[�Q�b�g�ɂȂ��Ă���G�̕����փJ�������������鏈��
		//E�L�[���������Ƃ�
		if (CKey::Once('E')) {
			mCameraMode = NORMAL;			//�^�[�Q�b�g��Ԃ̓G�̕��֌������[�h�ֈڍs
			CRes::sSETargetLookOff.Play();	//���ʉ����Đ�
		}
		break;
	}

	//Y������ 0�`3.14��180�x�͈�
	if (mAngleY < 0.05f) mAngleY = 0.05f;
	if (mAngleY > 1.51f) mAngleY = 1.51f;
	if (mAngleDelayY < 0.05f) mAngleDelayY = 0.05f;
	if (mAngleDelayY > 1.51f) mAngleDelayY = 1.51f;

	mPos.mX = mTarget.mX + (sinf(mAngleX)) * (mDist * sinf(mAngleY));
	mPos.mY = mTarget.mY + cosf(mAngleY) * mDist;
	mPos.mZ = mTarget.mZ + (cosf(mAngleX)) * (mDist * sinf(mAngleY));

	mCenter = mTarget;
	mCenter.mY += CAMERA_HEAD_ADJUST;//����␳
	mEye = mPos;

	//���R���C�_�Z�b�g
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

	//�J�����s��i�[
	glGetFloatv(GL_MODELVIEW_MATRIX, mMatrix.mF);
}

void CCamera::Render()
{
	//�����Ȃ�
}

CMatrix CCamera::GetMat() {
	return mMatrix;
}

bool CCamera::WorldToScreen(CVector* pOut, const CVector& pos)
{
	//���W�ϊ�
	CVector	screen_pos = mMatrix * pos;

	//��ʊO�Ȃ̂Ń��^�[��
	if (screen_pos.mZ >= 0.0f) {
		return false;
	}

	float Z = -screen_pos.mZ;
	//���W����
	screen_pos = screen_pos / -screen_pos.mZ;

	//	printf("%f,%f,%f\n", screen_pos.mX, screen_pos.mY, screen_pos.mZ);

	//�X�N���[���ϊ�
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

//�^�[�Q�b�g�ɂȂ��Ă���G�̕����փJ�������������鏈��
void CCamera::mTargetLook()
{
	//nullptr�Ŗ�����Βʂ�
	if (CEnemyManager::GetInstance()->GetTargetEnemy()) {
		//�v���C���[�Ɉ�ԋ߂��G����v���C���[�ɐL�т�x�N�g�������߂�
		CVector pos = CXPlayer::GetInstance()->mPosition - CEnemyManager::GetInstance()->GetTargetEnemy()->mPosition;
		//pos��Y��0.0�ɂ��Ă���
		pos.mY = 0.0f;
		//�x�N�g���𐳋K��
		pos = pos.Normalize();
		//�v���C���[����̋�����ݒ�
		pos = pos * DEF_CAMERA_DIST;
		//�J�������ړ����������ʒu��ݒ�
		mPos = CXPlayer::GetInstance()->mPosition + pos;

		//�v���C���[���猻�݂̃J�����̈ʒu�܂ł̃x�N�g��
		CVector vec1 = CXPlayer::GetInstance()->mPosition - mEye;
		//�v���C���[����J�������ړ����������ʒu�܂ł̃x�N�g��
		CVector vec2 = CXPlayer::GetInstance()->mPosition - mPos;
		//������0�ɂ���
		vec1.mY = 0.0f;
		vec2.mY = 0.0f;
		//�x�N�g���̒��������Ƃ߂�
		float len1 = vec1.Length();
		float len2 = vec2.Length();

		float dot = (vec1.mX * vec2.mX) + (vec1.mZ * vec2.mZ);	//����
		float cross = (vec1.mX * vec2.mZ) - (vec1.mZ * vec2.mX);//�O��
		float cos_sita = dot / (len1 * len2);	//�R�T�C���V�[�^�����߂�
		mRotRad = acosf(cos_sita);	//��]���������p�x��ݒ�
		//NaN����
		if (isnan(mRotRad)) {
			mRotRad = 0.0f;
		}
		//�O�ςŉ�]����������𔻒f
		if (cross > 0.0f) {
			//�A���O��X�����Z�A�������։�]
			mAngleX -= mLerp(0.0f, mRotRad, ROTATION_RATE);
		}
		else if (cross < 0.0f) {
			//�A���O��X�����Z�A�E�����։�]
			mAngleX += mLerp(0.0f, mRotRad, ROTATION_RATE);
		}
		//�A���O��Y��ύX
		mAngleY = mLerp(mAngleY, TARGETLOOK_Y, 0.1f);
	}
	//�A���O���x���̒l�����킹�Ă���
	mAngleDelayX = mAngleX;
	mAngleDelayY = mAngleY;
}
