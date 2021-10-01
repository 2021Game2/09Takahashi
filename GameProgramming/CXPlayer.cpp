#include "CXPlayer.h"
#include "CKey.h"

#define STAMINA_MAX 60	//�X�^�~�i�ő�l

extern int S;	//�m�F�p�A��ō폜

CXPlayer::CXPlayer()
	: mColSphereBody(this, nullptr, CVector(), 0.5f)
	, mColSphereHead(this, nullptr, CVector(0.0f, 5.0f, -3.0f), 0.5f)
	, mColSphereSword(this, nullptr, CVector(-10.0f, 10.0f, 50.0f), 0.3f)
	,mDash(false)
	,mStamina(STAMINA_MAX)
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
		if (CKey::Push('A'))
		{
			mRotation.mY += 2.0f;
		}
		if (CKey::Push('D'))
		{
			mRotation.mY -= 2.0f;
		}
		if (CKey::Push(' '))
		{
			ChangeAnimation(3, true, 30);
		}
		else if (CKey::Push('W'))
		{
			//�_�b�V��
			if (CKey::Push(VK_SHIFT)) {
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
			else {
				ChangeAnimation(1, true, 60);
				mPosition += CVector(0.0f, 0.0f, 0.1f) * mMatrixRotate;
			}	
		}
		else {
			ChangeAnimation(0, true, 60);
		}
	}

	//�_�b�V�����Ă��Ȃ���Ԃ̎��X�^�~�i���񕜂�����
	if (mDash == false && mStamina < STAMINA_MAX) {
		mStamina++;
	}

	mDash = false;

	CXCharacter::Update();

	S = mStamina;	//�m�F�p�A��ō폜
}
