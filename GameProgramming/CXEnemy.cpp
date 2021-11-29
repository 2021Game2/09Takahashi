#include "CXEnemy.h"
#include "CXPlayer.h"
#include "CUtil.h"
#define _USE_MATH_DEFINES
#include <math.h>

#define HP_MAX 1000	//�̗�
#define DAMAGE_BODY 10	//�_���[�W(��)
#define DAMAGE_HEAD 20	//�_���[�W(��)
#define ATTACK_DIS 3.0f			//�U���\�ɂȂ鋗��
#define SPEED_MOVE 0.05f		//�ʏ�ړ��̃X�s�[�h
#define SPEED_CHASE 0.1f		//�ǐՒ��̈ړ����x
#define CHASE_DIS_MAX 20.0f		//�ǐՉ\�ȍő勗��
#define SEARCH_DIS 15.0f		//�ǐՂ��J�n���鋗��

CXEnemy* CXEnemy::mInstance;

extern int EHp;	//�G�̗̑͊m�F�p�A��ō폜

CXEnemy::CXEnemy()
	: mColSphereBody(this, nullptr, CVector(0.5f, -1.0f, 0.0f), 1.2f)
	, mColSphereHead(this, nullptr, CVector(0.0f, 1.f, 0.0f), 1.2f)
	, mColSphereSword0(this, nullptr, CVector(0.7f, 3.5f, -0.2f), 0.5f)
	, mColSphereSword1(this, nullptr, CVector(0.5f, 2.5f, -0.2f), 0.5f)
	, mColSphereSword2(this, nullptr, CVector(0.3f, 1.5f, -0.2f), 0.5f)
	, mHp(HP_MAX)
	,mPoint(0.0f, 0.0f, 0.0f)
	,mAttackFlag_1(false)
	,mPlayerPoint(0.0f,0.0f,0.0f)
	,mPlayerDis(0.0f)
	,mMove(0.0f,0.0f,0.0f)
	,mRot(0.0f,0.0f,0.0f)
	,mSpeed(0.0f)
	,mDot(0.0f)
{
	mTag = EENEMY;

	mColSphereBody.mTag = CCollider::EBODY;
	mColSphereHead.mTag = CCollider::EHEAD;
	mColSphereSword0.mTag = CCollider::ESWORD;
	mColSphereSword1.mTag = CCollider::ESWORD;
	mColSphereSword2.mTag = CCollider::ESWORD;

	mState = EIDLE;	//�ҋ@

	mPoint = mPosition * mMatrixRotate;

	mInstance = this;
}

void CXEnemy::Init(CModelX* model)
{
	CXCharacter::Init(model);
	//�����s��̐ݒ�
	mColSphereBody.mpMatrix = &mpCombinedMatrix[1];
	//��
	mColSphereHead.mpMatrix = &mpCombinedMatrix[1];
	//��
	mColSphereSword0.mpMatrix = &mpCombinedMatrix[26];
	mColSphereSword1.mpMatrix = &mpCombinedMatrix[26];
	mColSphereSword2.mpMatrix = &mpCombinedMatrix[26];

	/*
	mColSphereBody.mpMatrix = &mpCombinedMatrix[8];
	//��
	mColSphereHead.mpMatrix = &mpCombinedMatrix[11];
	//��
	mColSphereSword.mpMatrix = &mpCombinedMatrix[21];
	*/
}

void CXEnemy::Update()
{
	mPlayerPoint = CXPlayer::GetInstance()->mPosition - mPosition;
	//�v���C���[�܂ł̋��������߂�
	mPlayerDis = mPlayerPoint.Length();

	switch (mState) {
	case EIDLE:	//�ҋ@���
		Idle();	//�ҋ@�������Ă�
		break;

	case EAUTOMOVE:	//�ړ����
		AutoMove();	//�ړ��������Ă�
		break;

	case ECHASE: //�ǐՏ��
		Chase(); //�ǐՏ������Ă�
		break;

	case EATTACK_1:	//�U�����
		Attack_1();	//�U��1�������Ă�
		if (mAttackFlag_1 == false) {
			mState = EIDLE;
		}
		break;

	case EDAMAGED:
		Damaged();
		break;

	case EDEATH: //���S���
		Death(); //���S�������Ă�
		break;
	}

	Check tCheck = CUtil::GetCheck2D(mMove.mX, mMove.mZ, 0, 0, mRot.mY);
	float turnspeed = 0.5f;

	mDot = tCheck.dot;

	//�}�ȐU��Ԃ��}��
	if (tCheck.turn > 1.5f) tCheck.turn = 1.5f;

	//�ړ������փL��������������
	if (tCheck.cross > 0.0f) {
		mRot.mY += tCheck.turn * turnspeed;
	}
	if (tCheck.cross < 0.0f) {
		mRot.mY -= tCheck.turn * turnspeed;
	}

	//�ړ�
	if (mState == EAUTOMOVE || mState == ECHASE) {
		mPosition += mMove * mSpeed;
	}

	//�\����180�x���]���Ă�̂Œ���
	CVector adjust_rot = mRot;
	adjust_rot.mY += M_PI;
	mRotation = (adjust_rot) * (180.0f / M_PI);

	mMove = CVector(0.0f, 0.0f, 0.0f);

	if (mHp <= 0) {
		mState = EDEATH;
	}

	if (mHp < 0) mHp = 0;

	EHp = mHp;

	CXCharacter::Update();
}

CXEnemy* CXEnemy::GetInstance()
{
	return mInstance;
}

void CXEnemy::Collision(CCollider* m, CCollider* o)
{
	if (m->mType == CCollider::ESPHERE)
	{
		if (o->mType == CCollider::ESPHERE)
		{
			if (o->mpParent->mTag == EPLAYER)
			{
				if (o->mTag == CCollider::ESWORD)
				{
					if (CCollider::Collision(m, o))
					{
						//�L���X�g�ϊ�
						//�v���C���[�̍U�����󂯂���
						if (((CXPlayer*)(o->mpParent))->mHit == true)
						{
							switch (m->mTag) {
							case CCollider::EBODY:
								mHp -= DAMAGE_BODY;
								mState = EDAMAGED;
								mAttackFlag_1 = false;
								break;
							case CCollider::EHEAD:
								mHp -= DAMAGE_HEAD;
								mState = EDAMAGED;
								mAttackFlag_1 = false;
								break;
							default:
								break;
							}
						}
					}
				}
			}
		}
	}
}

//�ҋ@����
void CXEnemy::Idle()
{
	//�ҋ@�A�j���[�V����
	ChangeAnimation(2, true, 200);

	//�v���C���[����苗���܂ŋ߂Â��ƒǐՏ�Ԃֈڍs
	if (mPlayerDis <= SEARCH_DIS) {
		mState = ECHASE;
	}

	int random = rand() % 180;
	if (random == 0) {
		//�ړ���Ԃֈڍs����
		mState = EAUTOMOVE;
		mPoint = mPosition;
		mPoint.mX += -15.0f + (float)(rand() % 30);
		mPoint.mZ += -15.0f + (float)(rand() % 30);
	}
}

//�ړ�����
void CXEnemy::AutoMove()
{
	//�ړ��A�j���[�V����
	ChangeAnimation(1, true, 70);

	mSpeed = SPEED_MOVE;
	//�ړI�n�_�܂ł̃x�N�g�������߂�
	CVector Point = mPoint - mPosition;
	mMove = Point.Normalize();

	//�v���C���[����苗���܂ŋ߂Â��ƒǐՏ�Ԃֈڍs
	if (mPlayerDis <= SEARCH_DIS) {
		mState = ECHASE;
	}

	float length = 0.0f;
	length = Point.Length();
	if (length <= 1.0f) {
		mState = EIDLE;
	}

}

//�ǐՏ���
void CXEnemy::Chase() 
{
	//�ړ��A�j���[�V����
	ChangeAnimation(1, true, 70);

	mSpeed = SPEED_CHASE;
	mMove = mPlayerPoint.Normalize();

	//�v���C���[�Ƃ̋������ǐՉ\�ȋ����𒴂���Ƒҋ@��Ԃֈڍs
	if (mPlayerDis >= CHASE_DIS_MAX) {
		mState = EIDLE;
	}
	//�v���C���[�Ƃ̋������U���\�ȋ����̏ꍇ
	else if (mPlayerDis <= ATTACK_DIS) {
		//�v���C���[�����ʂɂ���Ƃ�
		if (-2.0f < mDot && mDot < 2.0f)
		{
			//�U����Ԃֈڍs
			mState = EATTACK_1;
		}
	}
}

//�U��1����
void CXEnemy::Attack_1()
{
	mMove = mPlayerPoint.Normalize();

	if (mAttackFlag_1 == false) {
		mAttackFlag_1 = true;
		ChangeAnimation(7, false, 80);
	}

	if (mAnimationIndex == 7)
	{
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mAttackFlag_1 = false;
		}
	}
}

void CXEnemy::Damaged()
{
	ChangeAnimation(12, false, 30);
	
	CVector KnockBackVec = mPlayerPoint.Normalize();
	float KnockBackAmount = 0.2f;

	mPosition -= KnockBackVec * KnockBackAmount;

	if (mAnimationIndex == 12)
	{
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mState = EIDLE;
		}
	}
}

//���S����
void CXEnemy::Death()
{
	//���S�A�j���[�V����
	ChangeAnimation(11, false, 60);
}

void CXEnemy::SetPos(CVector hpos)
{
	mPosition = hpos;
}

CVector CXEnemy::GetPos()
{
	return mPosition;
}
