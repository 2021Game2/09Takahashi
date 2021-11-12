#include "CXPlayer.h"
#include "CKey.h"
#include "CCamera.h"
#include "CUtil.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "CInput.h"
#include "CXEnemy.h"

#define GRAVITY 0.9f	//�d��
#define HP_MAX 100	//�̗͍ő�l
#define STAMINA_MAX 100	//�X�^�~�i�ő�l
#define AVOID_STAMINA 40	//������̃X�^�~�i�̌�����
#define AVOID_TIME 30	//�������
#define AVOID_FIRSTSPEED 0.5f	//������̏���
#define SPEED_DEFAULT 0.15f	//�X�s�[�h(�ʏ펞)
#define SPEED_DASH_HIGH 0.2f	//�X�s�[�h(�_�b�V����)
#define SPEED_DASH_LOW 0.05f	//�X�s�[�h(�_�b�V�����A�X�^�~�i�؂�)
#define INVINCIBLETIME_AVOID 30	//���G����(�����)
#define INVINCIBLETIME_DAMAGE 60	//���G����(�_���[�W������)
#define DAMAGE 20	//�_���[�W
#define ATTACK2_FIRSTSPEED 0.6f	//�U��2�g�p���̏���
#define GRACETIME 10	//�h���U���̎�t����

CXPlayer* CXPlayer::mInstance;

extern int S;	//�m�F�p�A��ō폜
extern int PHp;	//�m�F�p�A��ō폜

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
	, mMove(0.0f, 0.0f, 0.0f)
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
{
	//�^�O�Ƀv���C���[��ݒ肵�܂�
	mTag = EPLAYER;
	mColSphereBody.mTag = CCollider::EBODY;
	mColSphereHead.mTag = CCollider::EHEAD;
	mColSphereSword.mTag = CCollider::ESWORD;

	mState = EIDLE;	//�ҋ@���

	mInstance = this;
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
		//���N���b�N�ōU��1�ֈڍs
		if (CKey::Once(VK_LBUTTON)) {
			mState = EATTACK_1;
		}
		//WASD�L�[�������ƈړ��ֈڍs
		else if (CKey::Push('W') || CKey::Push('A') || CKey::Push('S') || CKey::Push('D')) {
			mState = EMOVE;
		}
		break;

	case EATTACK_1:	//�U��1���
		Attack_1();	//�U��1�̏������Ă�
		//��t���ԓ��ɍ��N���b�N�ōU��3�ֈڍs
		if (mGraceTime > 0 && CKey::Once(VK_LBUTTON)) {
			mState = EATTACK_3;
		}
		if (mAttackFlag_1 == false) {
			mState = EIDLE;
		}
		break;

	case EATTACK_2:	//�U��2���
		Attack_2();	//�U��2�̏������Ă�
		if (mAttackFlag_2 == false) {
			mState = EIDLE;
		}
		break;

	case EATTACK_3:	//�U��3��Ԃ̎�
		Attack_3();	//�U��3�̏������Ă�
		if (mAttackFlag_3 == false) {
			mState = EIDLE;
		}
		break;

	case EMOVE:	//�ړ����
		//���N���b�N�ōU��1�ֈڍs
		if (CKey::Once(VK_LBUTTON)) {
			mState = EATTACK_2;
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
		//���N���b�N�ōU��1�ֈڍs
		if (CKey::Once(VK_LBUTTON)) {
			mState = EATTACK_2;
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

	case EDEATH:	//���S���
		Death();	//���S�������Ă�
		break;
	}

	//�_�b�V���A��������Ă��Ȃ���Ԃ̎��X�^�~�i���񕜂�����
	if (mState != EDASH && mState != EAVOID && mStamina < STAMINA_MAX) {
		mStamina++;
	}

	//���G���Ԃ̃J�E���g�_�E��
	if (mInvincibleTime > 0) {
		mInvincibleTime--;
	}
	//���G���Ԑ؂�
	else {
		mInvincibleFlag = false;	//���G��ԏI��
	}

	//���W�ړ�
	mPosition += mMove2;

	//�ړ��ʂ����炷
	mMove2 = mMove2 * GRAVITY;

	//���Z�b�g
	mMove = CVector(0.0f, 0.0f, 0.0f);

	//�̗͂�0�ɂȂ�Ǝ��S
	if (mHp <= 0) {
		mState = EDEATH;	//���S��Ԃֈڍs
		mHp = 0;
	}

	//��ō폜����
	//////////////////////////////
	if (mHp<=0&&CKey::Once(VK_RETURN)) {
		mHp = HP_MAX;
		mState = EIDLE;
	}
	S = mStamina;
	PHp = mHp;
	//////////////////////////////

	//�����_
	Camera.SetTarget(mPosition);

	CXCharacter::Update();
}

void CXPlayer::Collision(CCollider* m, CCollider* o)
{
	if (m->mType == CCollider::ESPHERE) {
		if (o->mType == CCollider::ESPHERE) {
			if (o->mpParent->mTag == EENEMY) {
				//�G�̍U�����󂯂���
				if (o->mTag == CCollider::ESWORD) {
					if (mInvincibleFlag == false) {
						if (CCollider::Collision(m, o)) {
							//�L���X�g�ϊ�
							if (((CXEnemy*)(o->mpParent))->mState == CXEnemy::EATTACK_1) {
								switch (m->mTag) {
								case CCollider::EHEAD:
									mHp -= DAMAGE;
									mInvincibleTime = INVINCIBLETIME_DAMAGE;
									mInvincibleFlag = true;
									break;

								case CCollider::EBODY:
									mHp -= DAMAGE;
									mInvincibleTime = INVINCIBLETIME_DAMAGE;
									mInvincibleFlag = true;
									break;
								}
							}
						}
					}
				}
				//�G�ɍU���������������AmHit��false�ɂ���
				if (m->mTag == CCollider::ESWORD) {
					if (o->mTag == CCollider::EHEAD || o->mTag == CCollider::EBODY) {
						if (mHit == true) {
							if (CCollider::Collision(m, o)) {
								mHit = false;
							}
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

//�ҋ@����
void CXPlayer::Idle()
{
	ChangeAnimation(0, true, 60);	//�ҋ@���[�V����
}

//�ړ�����
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
	mMove = mMove.Normalize();
	mMoveKeep = mMove;	//Move�ۑ�
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

//�_�b�V������
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

//�U��1����
void CXPlayer::Attack_1()
{
	if (mAttackFlag_1 == false) {
		ChangeAnimation(3, true, 30);
		mAttackFlag_1 = true;
		mHit = true;
		mGraceTime = 0;
	}

	if (mGraceTime > 0) {
		mGraceTime--;	//��t���Ԃ�����
	}

	if (mAnimationIndex == 3)
	{
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			ChangeAnimation(4, false, 30);
			mGraceTime = GRACETIME;	//��t���Ԃ�����
		}
	}
	else if (mAnimationIndex == 4)
	{
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mAttackFlag_1 = false;
			mHit = false;
		}
	}
}

//�U��2����
void CXPlayer::Attack_2()
{
	if (mAttackFlag_2 == false) {
		ChangeAnimation(7, true, 30);
		mAttackFlag_2 = true;
		mAttack2Speed = ATTACK2_FIRSTSPEED;
		mHit = true;
	}

	if (mAnimationIndex == 7)
	{
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			ChangeAnimation(8, false, 30);
		}
	}
	else if (mAnimationIndex == 8)
	{
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mAttackFlag_2 = false;
			mHit = false;
		}
	}

	mMove2 = mMoveKeep * mAttack2Speed;
	mAttack2Speed = mAttack2Speed * GRAVITY;
}

//�U��3����
void CXPlayer::Attack_3()
{
	if (mAttackFlag_3 == false) {
		ChangeAnimation(5, true, 20);
		mAttackFlag_3 = true;
		mHit = true;
	}

	if (mAnimationIndex == 5)
	{
		if (mAnimationFrame >= mAnimationFrameSize)
		{
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

//�������
void CXPlayer::Avoid()
{
	if (mAvoid == false) {
		mAvoid = true;					//���
		mStamina -= AVOID_STAMINA;		//�X�^�~�i����	
		mAvoidTime = AVOID_TIME;		//�������
		mAvoidSpeed = AVOID_FIRSTSPEED;	//����
		mInvincibleFlag = true;			//���G���
		mInvincibleTime = INVINCIBLETIME_AVOID;	//���G����
	}

	mMove2 = mMoveKeep * mAvoidSpeed;
	//������ԃJ�E���g�_�E��
	mAvoidTime--;
	mAvoidSpeed = mAvoidSpeed * GRAVITY;	//�X�s�[�h�������Ă���
	//������Ԑ؂�
	if (mAvoidTime <= 0) {
		mAvoid = false;	//����I��
	}
}

//���S����
void CXPlayer::Death()
{
	ChangeAnimation(11, false, 60);	//�|���A�j���[�V����
}


