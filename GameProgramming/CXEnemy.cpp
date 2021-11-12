#include "CXEnemy.h"
#include "CXPlayer.h"

#define HP_MAX 1000	//�̗�
#define DAMAGE_BODY 10	//�_���[�W(��)
#define DAMAGE_HEAD 20	//�_���[�W(��)
#define MARGIN 0.01f
#define ATTACK_DISTANCE 3.0f	//�����ƍU�����\�ɂȂ鋗��
#define SPEED_MOVE 0.05f	//�ʏ�ړ��̃X�s�[�h
#define SPEED_CHASE 0.1f	//�ǐՒ��̈ړ����x
#define CHASE_DIS_MAX 20.0f	//�ǐՂł���ő勗��

#define FRONTVEC CVector(0.0f, 0.0f, -1.0f)
#define SIDEVEC CVector(1.0f, 0.0f, 0.0f)

extern int EHp;	//�G�̗̑͊m�F�p�A��ō폜

CXEnemy::CXEnemy()
	: mColSphereBody(this, nullptr, CVector(0.5f, -1.0f, 0.0f), 1.0f)
	, mColSphereHead(this, nullptr, CVector(0.0f, 1.f, 0.0f), 1.0f)
	, mColSphereSword0(this, nullptr, CVector(0.7f, 3.5f, -0.2f), 0.5f)
	, mColSphereSword1(this, nullptr, CVector(0.5f, 2.5f, -0.2f), 0.5f)
	, mColSphereSword2(this, nullptr, CVector(0.3f, 1.5f, -0.2f), 0.5f)
	, mHp(HP_MAX)
	, mPlayerPosition(0.0f, 0.0f, 0.0f)
	,mPoint(0.0f, 0.0f, 0.0f)
	,mAttackFlag_1(false)
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
	switch (mState) {
	case EIDLE:	//�ҋ@���
		Idle();	//�ҋ@�������Ă�
		break;

	case EMOVE:	//�ړ����
		Move();	//�ړ��������Ă�
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
								break;
							case CCollider::EHEAD:
								mHp -= DAMAGE_HEAD;
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
	
	//������(X��)�̃x�N�g�������߂�
	CVector SideVec = SIDEVEC * mMatrixRotate;
	CVector PPoint = CXPlayer::GetInstance()->mPosition - mPosition;
	float DegreeX = PPoint.Dot(SideVec); //���x�N�g���Ƃ̓��ς����߂�
	float length = PPoint.Length();

	if (length < 15.0f) {
		mState = ECHASE;
	}

	int random = rand() % 180;
	if (random == 0) {
		//�ړ���Ԃֈڍs����
		mState = EMOVE;
		mPoint = mPosition;
		mPoint.mX += -15.0f + (float)(rand() % 30);
		mPoint.mZ += -15.0f + (float)(rand() % 30);
	}
}

//�ړ�����
void CXEnemy::Move()
{
	//�ړ��A�j���[�V����
	ChangeAnimation(1, true, 70);


	//������(X��)�̃x�N�g�������߂�
	CVector SideVec = SIDEVEC * mMatrixRotate;

	//�ړI�n�_�܂ł̃x�N�g�������߂�
	CVector Point = mPoint - mPosition;

	float DegreeX = Point.Dot(SideVec); //���x�N�g���Ƃ̓��ς����߂�

	float length = 0.0f;

	CVector PPoint = CXPlayer::GetInstance()->mPosition - mPosition;

	length = PPoint.Length();
	if (length < 15.0f) {
		mState = ECHASE;
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

	length = Point.Length();
	if (length < 1.0f) {
		mState = EIDLE;
	}
}

//�ǐՏ���
void CXEnemy::Chase() 
{
	//�ړ��A�j���[�V����
	ChangeAnimation(1, true, 70);

	//������(X��)�̃x�N�g�������߂�
	CVector SideVec = SIDEVEC * mMatrixRotate;

	CVector Point = CXPlayer::GetInstance()->mPosition - mPosition;

	float length = Point.Length();

	float DegreeX = Point.Dot(SideVec);
	if (DegreeX > MARGIN)
	{
		mRotation.mY -= 2.0f;
	}
	else if (DegreeX < -MARGIN)
	{
		mRotation.mY += 2.0f;
	}

	mPosition += (FRONTVEC * SPEED_CHASE) * mMatrixRotate;

	if (length >= CHASE_DIS_MAX) {
		mState = EIDLE;
	}
	else if (length <= ATTACK_DISTANCE) {
		if (-1.5f < DegreeX && DegreeX < 1.5f)
		{
			mState = EATTACK_1;
		}
	}
}

//�U��1����
void CXEnemy::Attack_1()
{
	if (mAttackFlag_1 == false) {
		mAttackFlag_1 = true;
		ChangeAnimation(7, true, 80);
	}

	if (mAnimationIndex == 7)
	{
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mAttackFlag_1 = false;
		}
	}

	CVector SideVec = SIDEVEC * mMatrixRotate;

	CVector Point = CXPlayer::GetInstance()->mPosition - mPosition;

	float DegreeX = Point.Dot(SideVec);

	if (DegreeX > MARGIN)
	{
		mRotation.mY -= 1.0f;
	}
	else if (DegreeX < -MARGIN)
	{
		mRotation.mY += 1.0f;
	}
}

//���S����
void CXEnemy::Death()
{
	//���S�A�j���[�V����
	ChangeAnimation(11, false, 60);
}