#include "CXPlayer.h"
#include "CKey.h"
#include "CCamera.h"
#include "CUtil.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "CInput.h"

#define GRAVITY 0.98f	//�d��
#define STAMINA_MAX 100	//�X�^�~�i�ő�l
#define AVOID_STAMINA 40	//������̃X�^�~�i�̌�����
#define AVOID_TIME 40	//�������
#define AVOID_FIRSTSPEED 0.3f	//�������
#define SPEED_DASH_HIGH 0.2f	//�_�b�V�����x
#define SPEED_DASH_LOW	0.05f;	//�_�b�V�����x(�X�^�~�i�؂�)

extern int S;	//�m�F�p�A��ō폜

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
	//�^�O�Ƀv���C���[��ݒ肵�܂�
	mTag = EPLAYER;
	mColSphereSword.mTag = CCollider::ESWORD;
}

void CXPlayer::Init(CModelX* model)
{
	CXCharacter::Init(model);
	//�����s��̐ݒ�
	mColSphereBody.mpMatrix = &mpCombinedMatrix[9];
	//��
	mColSphereHead.mpMatrix = &mpCombinedMatrix[12];
	//��
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
		//�J�������_�ړ��@�ʏ̖��o�ړ�

		//�J�����̍��E�ƑO��̃x�N�g�����擾
		CVector SideVec = Camera.GetMat().GetXVec();
		CVector FrontVec = Camera.GetMat().GetZVec();
		//�����ړ��̓J�b�g����
		SideVec.mY = 0.0f;
		FrontVec.mY = 0.0f;
		//���K������
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

		//�ړ��ʐ��K���@��������Ȃ��Ǝ΂߈ړ��������Ȃ��Ă��܂��̂Œ���
		//�W�����v���Ȃǂ�Y���𐳋K�����Ȃ��悤����
		Move.Normalize();
		//�_�b�V��
		if (CKey::Push(VK_SHIFT) && mAvoid == false) {
			ChangeAnimation(1, true, 60);
			//�X�^�~�i���c���Ă���Ƃ�
			if (mStamina > 0) {
				speed = SPEED_DASH;
				mStamina--;
			}
			//�X�^�~�i�؂���
			else {
				speed = 0.05f;
			}
			mDash = true;
		}
		//���s�ړ���
		Move = Move * speed;

		//���ʂ�3�����x�N�g���v�Z�ŎZ�o�����ق������m�����v�Z�ʂ����O����ꍇ�͋[���v�Z�Ōy�ʉ�
		//�[���x�N�g���v�Z
		Check tCheck = CUtil::GetCheck2D(Move.mX, Move.mZ, 0, 0, mRotation.mY * (M_PI / 180.0f));

		//��]���x�@degree�ɒ���
		float turnspeed = (180.0f / M_PI) * 0.5f;

		//�}�ȐU��Ԃ��}��
		if (tCheck.turn > 1.5f) tCheck.turn = 1.5f;

		//�ړ������փL��������������
		if (tCheck.cross > 0.0f) {
			mRotation.mY += tCheck.turn * turnspeed;
		}
		if (tCheck.cross < 0.0f) {
			mRotation.mY -= tCheck.turn * turnspeed;
		}

		//���W�ړ�
		mPosition += Move;

		/*
		else if (CKey::Push('W'))
		{
			//���
			if (CKey::Once(VK_SPACE) && mAvoid == false) {
				//�X�^�~�i������ɕK�v�ȃX�^�~�i�ʂ𒴂��Ă���Ƃ�
				if (mStamina >= AVOID_STAMINA) {
					mAvoid = true;
					mAvoidTime = AVOID_TIME;
					mAvoidSpeed = AVOID_FIRSTSPEED;
					mStamina -= AVOID_STAMINA;
				}
			}
			//�_�b�V��
			if (CKey::Push(VK_SHIFT) && mAvoid == false) {
				ChangeAnimation(1, true, 60);
				//�X�^�~�i���c���Ă���Ƃ�
				if (mStamina > 0) {
					mPosition += CVector(0.0f, 0.0f, 0.2f) * mMatrixRotate;
					mStamina--;
				}
				//�X�^�~�i�؂���
				else {
					mPosition += CVector(0.0f, 0.0f, 0.05f) * mMatrixRotate;
				}
				mDash = true;
			}
			//�ʏ�ړ�
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

	//�_�b�V���A��������Ă��Ȃ���Ԃ̎��X�^�~�i���񕜂�����
	if (mDash == false && mAvoid == false && mStamina < STAMINA_MAX) {
		mStamina++;
	}

	mDash = false;

	S = mStamina;	//�m�F�p�A��ō폜

	//�����_
	Camera.SetTarget(mPosition);

	CXCharacter::Update();
}
