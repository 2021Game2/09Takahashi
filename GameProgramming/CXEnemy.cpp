#include "CXEnemy.h"
#include "CXPlayer.h"

#define HP_MAX 1000	//�̗�
#define DAMAGE_BODY 10	//�_���[�W(��)
#define DAMAGE_HEAD 20	//�_���[�W(��)
#define MARGIN 0.01f
#define ATTACK_DISTANCE 3.0f	//�����ƍU�����\�ɂȂ鋗��
#define COUNT 90
#define FRONTVEC CVector(0.0f, 0.0f, -1.0f) 
#define SPEED_MOVE 0.05f	//�ʏ�ړ��̃X�s�[�h
#define SPEED_CHASE 0.1f	//�ǐՒ��̈ړ����x

extern int EHp;	//�G�̗̑͊m�F�p�A��ō폜

CXEnemy::CXEnemy()
	: mColSphereBody(this, nullptr, CVector(0.5f, -1.0f, 0.0f), 1.0f)
	, mColSphereHead(this, nullptr, CVector(0.0f, 1.f, 0.0f), 1.0f)
	, mColSphereSword0(this, nullptr, CVector(0.7f, 3.5f, -0.2f), 0.5f)
	, mColSphereSword1(this, nullptr, CVector(0.5f, 2.5f, -0.2f), 0.5f)
	, mColSphereSword2(this, nullptr, CVector(0.3f, 1.5f, -0.2f), 0.5f)
	, mColSearch(this, &mMatrix, CVector(0.0f, 0.0f, 0.0f), 15.0f)
	, mHp(HP_MAX)
	, mPlayerPosition(0.0f, 0.0f, 0.0f)
	,mPoint(0.0f, 0.0f, 0.0f)
	,mCount(COUNT)
{
	mTag = EENEMY;

	mColSphereBody.mTag = CCollider::EBODY;
	mColSphereHead.mTag = CCollider::EHEAD;
	mColSphereSword0.mTag = CCollider::ESWORD;
	mColSphereSword1.mTag = CCollider::ESWORD;
	mColSphereSword2.mTag = CCollider::ESWORD;

	mColSearch.mTag = CCollider::ESEARCH;

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

	mCount--;

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
				if (m->mTag == CCollider::ESEARCH) {
					if (o->mTag == CCollider::EHEAD || o->mTag == CCollider::EBODY) {
						if (CCollider::Collision(m, o)) {
							mPlayerPosition = o->mpParent->mPosition;
							//mState = ECHASE;
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

	if (mCount == 0) {
		int random = rand() % 2;
		switch (random) {
		case 0:
			//
			break;

		case 1:
			//�ړ���Ԃֈڍs����
			mState = EMOVE;
			mPoint = mPosition * CMatrix().RotateY(rand() % 360);
			break;
		}
		mCount = COUNT;
	}
}

//�ړ�����
void CXEnemy::Move()
{
	//�ړ��A�j���[�V����
	ChangeAnimation(1, true, 70);

	//������(X��)�̃x�N�g�������߂�
	CVector vx = CVector(1.0f, 0.0f, 0.0f) * mMatrixRotate;

	//�ړI�n�_�܂ł̃x�N�g�������߂�
	CVector vp = mPoint - mPosition;
	float dx = vp.Dot(vx); //���x�N�g���Ƃ̓��ς����߂�
	//���E�����։�]
	if (dx > MARGIN)
	{
		mRotation.mY += 2.0f; //���։�]
	}
	else if (dx < -MARGIN)
	{
		mRotation.mY -= 2.0f; //�E�։�]
	}
	//�ړ�����
	mPosition += (FRONTVEC * SPEED_MOVE) * mMatrixRotate;

	if (mCount == 0) {
		int random = rand() % 2;
		switch (random) {
		case 0:
			//�ڕW�n�_�X�V
			mPoint = mPosition * CMatrix().RotateY(rand() % 360);
			break;

		case 1:
			//�ҋ@��Ԃֈڍs����
			mState = EIDLE;
			break;
		}
		mCount = COUNT;
	}
}

//�ǐՏ���
void CXEnemy::Chase() 
{
	//�ړ��A�j���[�V����
	ChangeAnimation(1, true, 70);

	CVector SideVec = CVector(1.0f, 0.0f, 0.0f) * mMatrixRotate;
	SideVec.Normalize();

	CVector PlayerPoint = mPlayerPosition - mPosition;

	float DegreeX = PlayerPoint.Dot(SideVec);
	if (DegreeX > MARGIN)
	{
		mRotation.mY -= 2.0f;
	}
	else if (DegreeX < -MARGIN)
	{
		mRotation.mY += 2.0f;
	}

	float length = PlayerPoint.Length();
	if (length > ATTACK_DISTANCE) {
		mPosition += (FRONTVEC * SPEED_CHASE) * mMatrixRotate;
	}
	else{
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

	CVector SideVec = CVector(1.0f, 0.0f, 0.0f) * mMatrixRotate;
	SideVec.Normalize();

	CVector Point = mPlayerPosition - mPosition;

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