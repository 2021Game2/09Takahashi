#include "CXPlayer.h"
#include "CKey.h"
#include "CCamera.h"
#include "CUtil.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "CInput.h"

#define GRAVITY 0.9f	//�d��
#define STAMINA_MAX 100	//�X�^�~�i�ő�l
#define AVOID_STAMINA 40	//������̃X�^�~�i�̌�����
#define AVOID_TIME 30	//�������
#define AVOID_FIRSTSPEED 0.5f	//�������
#define SPEED_DASH_HIGH 0.2f	//�_�b�V�����x
#define SPEED_DASH_LOW 0.05f	//�_�b�V�����x(�X�^�~�i�؂�)
#define SPEED_DEFAULT 0.15f	//�f�t�H���g�X�s�[�h
#define INVINCIBLETIME_AVOID 30	//������̖��G����

extern int S;	//�m�F�p�A��ō폜

CXPlayer::CXPlayer()
	: mColSphereBody(this, nullptr, CVector(), 0.5f)
	, mColSphereHead(this, nullptr, CVector(0.0f, 5.0f, -3.0f), 0.5f)
	, mColSphereSword(this, nullptr, CVector(-10.0f, 10.0f, 50.0f), 0.3f)
	, mStamina(STAMINA_MAX)
	, mAvoid(false)
	, mAvoidTime(0)
	, mAvoidSpeed(0.0f)
	, mSpeed(SPEED_DEFAULT)
	, mAvoidMove(0.0f, 0.0f, 0.0f)
	, mInvincible(false)
	, mInvincibleTime(0)
	, mMove(0.0f, 0.0f, 0.0f)
	, mMove2(0.0f, 0.0f, 0.0f)
	, mSideVec(0.0f, 0.0f, 0.0f)
	, mFrontVec(0.0f, 0.0f, 0.0f)
	, mTurnspeed(0.0f)
	, mAttackFlag_1(false)
{
	//�^�O�Ƀv���C���[��ݒ肵�܂�
	mTag = EPLAYER;
	mColSphereBody.mTag = CCollider::EBODY;
	mColSphereHead.mTag = CCollider::EHEAD;
	mColSphereSword.mTag = CCollider::ESWORD;

	mState = EIDLE;
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
	switch (mState) {
	case EIDLE:	//�ҋ@���
		Idle();	//�ҋ@�������Ă�
		//���N���b�N�������ƍU��1�ֈڍs
		if (CInput::GetMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
			mState = EATTACK_1;
		}
		//WASD�L�[�������ƈړ��ֈڍs
		else if (CKey::Push('W') || CKey::Push('A') || CKey::Push('S') || CKey::Push('D')) {
			mState = EMOVE;
		}
		break;

	case EATTACK_1:	//�U��1���
		Attack_1();	//�U��1�̏������Ă�
		if (mAttackFlag_1 == false) {
			mState = EIDLE;
		}
		break;

	case EMOVE:	//�ړ����
		//���N���b�N�������ƍU��1�ֈڍs
		if (CInput::GetMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
			mState = EATTACK_1;
		}
		//SHIFT�L�[�������ƃ_�b�V���ֈڍs
		else if (CKey::Push(VK_SHIFT)) {
			mState = EDASH;
		}
		//SPACE�L�[������������ɕK�v�ȗʂ̃X�^�~�i������Ƃ�����ֈڍs
		else if (CKey::Once(VK_SPACE) && mStamina >= AVOID_STAMINA) {
			mState = EAVOID;
		}
		//WASD�L�[�������ƈړ�
		else if (CKey::Push('W') || CKey::Push('A') || CKey::Push('S') || CKey::Push('D')) {
			Move();	//�ړ��������Ă�
		}
		//�ҋ@��Ԃֈڍs
		else {
			mState = EIDLE;
		}
		break;

	case EDASH:	//�_�b�V�����
		//���N���b�N�������ƍU��1�ֈڍs
		if (CInput::GetMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
			mState = EATTACK_1;
		}
		//SPACE�L�[������������ɕK�v�ȗʂ̃X�^�~�i������Ƃ�����ֈڍs
		else if (CKey::Once(VK_SPACE) && mStamina >= AVOID_STAMINA) {
			mState = EAVOID;
		}
		//SHIFT�L�[�������ƃ_�b�V��
		else if (CKey::Push(VK_SHIFT)) {
			Dash();	//�_�b�V���������Ă�
		}
		//WASD�L�[�������ƈړ��ֈڍs
		else if (CKey::Push('W') || CKey::Push('A') || CKey::Push('S') || CKey::Push('D')) {
			mState = EMOVE;
		}
		//�ҋ@��Ԃֈڍs
		else {
			mState = EIDLE;
		}
		break;

	case EAVOID:	//������
		Avoid();	//����������Ă�
		if (mAvoid == false) {
			//����I����WASD�L�[��SHIFT�L�[��������Ă���ƃ_�b�V���ֈڍs
			if (CKey::Push('W') || CKey::Push('A') || CKey::Push('S') || CKey::Push('D')&&CKey::Push(VK_SHIFT)) {
				mState = EDASH;
			}
			//����I����WASD�L�[��������Ă���ƈړ��ֈڍs
			else if (CKey::Push('W') || CKey::Push('A') || CKey::Push('S') || CKey::Push('D')) {
				mState = EMOVE;
			}
			//�ҋ@��Ԃֈڍs
			else {
				mState = EIDLE;
			}	
		}
		break;
	}

	//�_�b�V���A��������Ă��Ȃ���Ԃ̎��X�^�~�i���񕜂�����
	if (mState != EDASH && mState != EAVOID && mStamina < STAMINA_MAX) {
		mStamina++;
	}

	//���G���Ԃ̃J�E���g�_�E��
	if (mInvincibleTime > 0) {
		mInvincibleTime--;
		mInvincible = true;	//���G���
	}
	//���G���Ԑ؂�
	else {
		mInvincible = false;	//���G��ԏI��
	}

	//���W�ړ�
	mPosition += mMove2;

	//�ړ��ʂ����炷
	mMove2 = mMove2 * GRAVITY;

	//���Z�b�g
	mMove = CVector(0, 0, 0);

	S = mStamina;	//�m�F�p�A��ō폜

	//�����_
	Camera.SetTarget(mPosition);

	CXCharacter::Update();
}

void CXPlayer::Collision(CCollider* m, CCollider* o)
{

}

void CXPlayer::Idle()
{
	ChangeAnimation(0, true, 60);	//�ҋ@���[�V����
}

void CXPlayer::Move()
{
	ChangeAnimation(1, true, 60);
	//�_�b�V�����ɃX�s�[�h���㏑�����Ȃ��p
	if (mState == EMOVE) {
		mSpeed = SPEED_DEFAULT;
	}

	//�J�������_�ړ��@�ʏ̖��o�ړ�
	//�J�����̍��E�ƑO��̃x�N�g�����擾
	mSideVec = Camera.GetMat().GetXVec();
	mFrontVec = Camera.GetMat().GetZVec();
	//�����ړ��̓J�b�g����
	mSideVec.mY = 0.0f;
	mFrontVec.mY = 0.0f;
	//���K������
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

	//�ړ��ʐ��K���@��������Ȃ��Ǝ΂߈ړ��������Ȃ��Ă��܂��̂Œ���
	//�W�����v���Ȃǂ�Y���𐳋K�����Ȃ��悤����
	mMove.Normalize();
	mAvoidMove = mMove;	//Move�ۑ�(���p)
	if (mMove.Length() != 0.0f) {
		//���s�ړ���
		mMove2 = mMove * mSpeed;
	}

	//���ʂ�3�����x�N�g���v�Z�ŎZ�o�����ق������m�����v�Z�ʂ����O����ꍇ�͋[���v�Z�Ōy�ʉ�
	//�[���x�N�g���v�Z
	Check tCheck = CUtil::GetCheck2D(mMove.mX, mMove.mZ, 0, 0, mRotation.mY * (M_PI / 180.0f));

	//��]���x�@degree�ɒ���
	mTurnspeed = (180.0f / M_PI) * 0.5f;

	//�}�ȐU��Ԃ��}��
	if (tCheck.turn > 1.5f) tCheck.turn = 1.5f;

	//�ړ������փL��������������
	if (tCheck.cross > 0.0f) {
		mRotation.mY += tCheck.turn * mTurnspeed;
	}
	if (tCheck.cross < 0.0f) {
		mRotation.mY -= tCheck.turn * mTurnspeed;
	}
}

void CXPlayer::Dash()
{
	//�X�^�~�i���c���Ă���Ƃ�
	if (mStamina > 0) {
		mSpeed = SPEED_DASH_HIGH;
		mStamina--;
	}
	//�X�^�~�i�؂���
	else {
		mSpeed = SPEED_DASH_LOW;
	}

	Move();	//�ړ��������Ă�
}

void CXPlayer::Attack_1()
{
	if (mAttackFlag_1 == false) {
		ChangeAnimation(3, true, 30);
		mAttackFlag_1 = true;
	}

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
			mAttackFlag_1 = false;
		}
	}
}

void CXPlayer::Avoid()
{
	if (mAvoid == false) {
		mAvoid = true;					//���
		mStamina -= AVOID_STAMINA;		//�X�^�~�i����	
		mAvoidTime = AVOID_TIME;		//�������
		mAvoidSpeed = AVOID_FIRSTSPEED;	//����
		mInvincibleTime = INVINCIBLETIME_AVOID;	//���G����
	}

	mMove2 = mAvoidMove * mAvoidSpeed;
	//������ԃJ�E���g�_�E��
	mAvoidTime--;
	mAvoidSpeed = mAvoidSpeed * GRAVITY;	//�X�s�[�h�������Ă���
	//������Ԑ؂�
	if (mAvoidTime <= 0) {
		mAvoid = false;	//����I��
	}
}
