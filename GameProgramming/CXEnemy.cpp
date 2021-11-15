#include "CXEnemy.h"
#include "CXPlayer.h"

#define HP_MAX 1000	//�̗�
#define DAMAGE_BODY 100	//�_���[�W(��)
#define DAMAGE_HEAD 200	//�_���[�W(��)
#define MARGIN 0.01f
#define ATTACK_DIS 3.0f			//�U���\�ɂȂ鋗��
#define SPEED_MOVE 0.05f		//�ʏ�ړ��̃X�s�[�h
#define SPEED_CHASE 0.1f		//�ǐՒ��̈ړ����x
#define CHASE_DIS_MAX 20.0f		//�ǐՉ\�ȍő勗��
#define SEARCH_DIS 15.0f		//�ǐՂ��J�n���鋗��

#define FRONTVEC CVector(0.0f, 0.0f, -1.0f)
#define SIDEVEC CVector(1.0f, 0.0f, 0.0f)

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
{
	mTag = EENEMY;

	mColSphereBody.mTag = CCollider::EBODY;
	mColSphereHead.mTag = CCollider::EHEAD;
	mColSphereSword0.mTag = CCollider::ESWORD;
	mColSphereSword1.mTag = CCollider::ESWORD;
	mColSphereSword2.mTag = CCollider::ESWORD;

	mState = EIDLE;	//�ҋ@

	mPoint = mPosition * mMatrixRotate;
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

	if (mHp <= 0) {
		mState = EDEATH;
	}

	if (mHp < 0) mHp = 0;

	EHp = mHp;

	CXCharacter::Update();
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

	//�v���C���[����苗���܂ŋ߂Â��ƒǐՏ�Ԃֈڍs
	if (mPlayerDis <= SEARCH_DIS) {
		mState = ECHASE;
	}

	//������(X��)�̃x�N�g�������߂�
	CVector SideVec = SIDEVEC * mMatrixRotate;
	//�ړI�n�_�܂ł̃x�N�g�������߂�
	CVector Point = mPoint - mPosition;
	float DegreeX = Point.Dot(SideVec); //���x�N�g���Ƃ̓��ς����߂�
	float length = 0.0f;

	length = Point.Length();
	if (length <= 1.0f) {
		mState = EIDLE;
	}

	//���E�����։�]
	if (DegreeX > MARGIN)
	{
		mRotation.mY -= 2.0f; 
	}
	else if (DegreeX < -MARGIN)
	{
		mRotation.mY += 2.0f; 
	}
	//�ړ�����
	mPosition += (FRONTVEC * SPEED_MOVE) * mMatrixRotate;
}

//�ǐՏ���
void CXEnemy::Chase() 
{
	//�ړ��A�j���[�V����
	ChangeAnimation(1, true, 70);

	//������(X��)�̃x�N�g�������߂�
	CVector SideVec = SIDEVEC * mMatrixRotate;
	float DegreeX = mPlayerPoint.Dot(SideVec);

	//�v���C���[�Ƃ̋������ǐՉ\�ȋ����𒴂���Ƒҋ@��Ԃֈڍs
	if (mPlayerDis >= CHASE_DIS_MAX) {
		mState = EIDLE;
	}
	//�v���C���[�Ƃ̋������U���\�ȋ����̏ꍇ
	else if (mPlayerDis <= ATTACK_DIS) {
		//�v���C���[�����ʂɂ���Ƃ�
		if (-1.5f < DegreeX && DegreeX < 1.5f)
		{
			//�U����Ԃֈڍs
			mState = EATTACK_1;
		}
	}

	if (DegreeX > MARGIN)
	{
		mRotation.mY -= 2.0f;
	}
	else if (DegreeX < -MARGIN)
	{
		mRotation.mY += 2.0f;
	}

	mPosition += (FRONTVEC * SPEED_CHASE) * mMatrixRotate;
}

//�U��1����
void CXEnemy::Attack_1()
{
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

	CVector SideVec = SIDEVEC * mMatrixRotate;

	float DegreeX = mPlayerPoint.Dot(SideVec);

	if (DegreeX > MARGIN)
	{
		mRotation.mY -= 1.0f;
	}
	else if (DegreeX < -MARGIN)
	{
		mRotation.mY += 1.0f;
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