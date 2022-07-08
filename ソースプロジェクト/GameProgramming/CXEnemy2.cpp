#include "CXEnemy2.h"
#include "CRes.h"
#include "CCollisionManager.h"
#include "CXPlayer.h"
#include "CEnemyManager.h"

#define SEARCH_DIS 15.0f	//�ǐՂ��J�n���鋗��
#define SPEED_MOVE 0.05f	//�ʏ�ړ��̃X�s�[�h
#define SPEED_CHASE 0.1f	//�ǐՒ��̈ړ����x
#define ATTACK_DIS 4.0f		//�U���\�ɂȂ鋗��
#define CHASE_DIS_MAX 15.0f	//�ǐՉ\�ȍő勗��
#define AVOID_DIS 4.0f		//����\�ɂȂ鋗��
#define CHASE_RESTART_TIME 60 //�ǐՂ��ĊJ����܂ł̎���
#define SPEED_ATTACK_2 0.2f //�U��2��Ԃ̈ړ����x

CXEnemy2::CXEnemy2()
	:mColSphereBody(this, nullptr, CVector(0.0f, -1.0f, 0.0f), 1.3f)
	, mColSphereHead(this, nullptr, CVector(0.0f, -0.5f, 0.0f), 1.1f)
	, mColSphereSword0(this, nullptr, CVector(0.0f, 0.0f, -0.5f), 0.9f)
	, mColSphereSword1(this, nullptr, CVector(0.0f, 0.0f, 0.0f), 0.9f)
{
	//������
	Init(&CRes::sModelXEnemy2);

	//�R���C�_�̃^�O��ݒ�
	mColSphereBody.mTag = CCollider::EBODY;		//�{��
	mColSphereHead.mTag = CCollider::EHEAD;		//��
	mColSphereSword0.mTag = CCollider::ESWORD;	//��
	mColSphereSword1.mTag = CCollider::ESWORD;	//��

	//�G�̎�ނ�ݒ�
	mEnemyType = ETYPE_2;
}

CXEnemy2::~CXEnemy2()
{
}

void CXEnemy2::Init(CModelX* model)
{
	CXCharacter::Init(model);
	//�����s��̐ݒ�
	mColSphereBody.mpMatrix = &mpCombinedMatrix[2];		//��
	mColSphereHead.mpMatrix = &mpCombinedMatrix[30];	//��
	mColSphereSword0.mpMatrix = &mpCombinedMatrix[45];	//��
	mColSphereSword1.mpMatrix = &mpCombinedMatrix[52];	//��
}

void CXEnemy2::TaskCollision()
{
	CCollisionManager::Get()->Collision(&mColSphereBody, COLLISIONRANGE);
	CCollisionManager::Get()->Collision(&mColSphereHead, COLLISIONRANGE);
	CCollisionManager::Get()->Collision(&mColSphereSword0, COLLISIONRANGE);
	CCollisionManager::Get()->Collision(&mColSphereSword1, COLLISIONRANGE);
}

void CXEnemy2::Idle()
{
	//�ҋ@�A�j���[�V����
	ChangeAnimation(1, true, 100);

	//�v���C���[�����S��Ԃł͖����Ƃ�
	if (CXPlayer::GetInstance()->GetState() != CXPlayer::EPlayerState::EDEATH) {
		//�v���C���[����苗���܂ŋ߂Â��ƒǐՏ�Ԃֈڍs
		if (mPlayerDis <= SEARCH_DIS) {
			mState = ECHASE;
		}
	}

	//�ҋ@��Ԓ������_���ňړ���Ԃֈڍs
	int random = rand() % 240;
	if (random == 0) {
		//�ڕW�n�_��ݒ�
		mPoint = mPosition;
		//mPoint��mX��mZ�̒l�֓�����0.0f������ƓG�������Ă��܂��̂ŁA�Ō�ɒl�𑫂��Ă���
		mPoint.mX += -15.0f + (float)(rand() % 30) + 0.001f;
		mPoint.mZ += -15.0f + (float)(rand() % 30) + 0.001f;
		//�ړ���Ԃֈڍs����
		mState = EAUTOMOVE;
	}
}

void CXEnemy2::AutoMove()
{
	//�ړ��A�j���[�V����
	ChangeAnimation(5, true, 80);

	//�ړ��X�s�[�h��ύX
	mSpeed = SPEED_MOVE;

	//�ړI�n�_�܂ł̃x�N�g�������߂�
	CVector Point = mPoint - mPosition;
	//mMoveDir�ɖڕW�n�_�����̃x�N�g��������
	mMoveDir = Point.Normalize();

	//�v���C���[�����S��Ԃł͖����Ƃ�
	if (CXPlayer::GetInstance()->GetState() != CXPlayer::EPlayerState::EDEATH) {
		//�v���C���[����苗���܂ŋ߂Â��ƒǐՏ�Ԃֈڍs
		if (mPlayerDis <= SEARCH_DIS) {
			mState = ECHASE;
		}
	}

	//�ڕW�n�_�܂ł̋���
	float length = Point.Length();
	//�ڕW�n�_�t�߂܂ňړ�����Ƒҋ@��Ԃֈڍs
	if (length <= 1.0f) {
		mState = EIDLE;
	}
}

void CXEnemy2::Chase()
{
	//�ړ��A�j���[�V����
	ChangeAnimation(5, true, 80);

	//�ړ��X�s�[�h��ύX
	mSpeed = SPEED_CHASE;

	//mMoveDir�Ƀv���C���[�����̃x�N�g��������
	mMoveDir = mPlayerPoint.Normalize();

	//�v���C���[���U���\�ȋ����ɂ���Ƃ�
	if (mPlayerDis <= ATTACK_DIS) {
		mState = EATTACK_IDLE; //�U���ҋ@��Ԃֈڍs
	}

	//�v���C���[���ǐՉ\�ȋ����ɂ��Ȃ��Ƃ�
	if (mPlayerDis >= CHASE_DIS_MAX) {
		mState = EIDLE; //�ҋ@��Ԃֈڍs
	}

	int random = 0;
	//�U���\�ȂƂ�
	if (CEnemyManager::GetInstance()->mIsEnemyAttack()) {
		//�ǐՏ�Ԓ��Ƀ����_���ōU��2��Ԃֈڍs
		random = rand() % 480;
		if (random == 0) {
			mState = EATTACK_2;
		}
	}
}

void CXEnemy2::Attack_Idle()
{
	//�v���C���[�����S��Ԃ������Ƃ��ҋ@��ԂɈڍs����
	if (CXPlayer::GetInstance()->GetState() == CXPlayer::EPlayerState::EDEATH)mState = EIDLE;

	//�U���ҋ@�A�j���[�V����
	ChangeAnimation(3, true, 100);

	//mMoveDir�Ƀv���C���[�����̃x�N�g��������
	mMoveDir = mPlayerPoint.Normalize();

	int random = 0;

	//�v���C���[���U���\�ȋ����ɂ���Ƃ�
	if (mPlayerDis <= ATTACK_DIS) {
		mChaseRestartCount = 0; //�J�E���g�����Z�b�g����
		//�U���\�ȂƂ�
		if (CEnemyManager::GetInstance()->mIsEnemyAttack()) {
			//�����_���ōU�����s�����ǂ����𔻒肷��
			random = rand() % 120;
			if (random == 0) {
				//�����_���ōU���̎�ނ����߂�
				random = rand() % 2;
				switch (random) {
				case 0:
					mState = EATTACK_1; //�U��1��Ԃֈڍs
					break;
				case 1:
					mState = EATTACK_2; //�U��2��Ԃֈڍs
					break;
				}
			}
		}
	}
	//�v���C���[���U���\�ȋ����ɂ��Ȃ��Ƃ�
	else {
		mChaseRestartCount++; //�J�E���g�����Z����
		//�J�E���g���ǐՂ��ĊJ���鎞�ԂɂȂ�Ə�Ԃ��ڍs����
		if (mChaseRestartCount == CHASE_RESTART_TIME) {
			//�v���C���[���ǐՉ\�ȋ����ɂ���Ƃ�
			if (mPlayerDis <= CHASE_DIS_MAX) {
				mState = ECHASE; //�ǐՏ�Ԃֈڍs
			}
			else {
				mState = EIDLE; //�ҋ@��Ԃֈڍs
			}
		}
	}
}

void CXEnemy2::Attack_1()
{
	//mMoveDir�Ƀv���C���[�����̃x�N�g��������
	mMoveDir = mPlayerPoint.Normalize();

	//�U��1�A�j���[�V����
	ChangeAnimation(2, false, 100);

	if (mAnimationIndex == 2)
	{
		//�q�b�g���蔭��
		if (mAnimationFrame == 30) {
			mIsHit = true;
		}
		//�q�b�g����I��
		if (mAnimationFrame == 60) {
			mIsHit = false;
		}
		//�A�j���[�V�����I����
		if (mIsAnimationEnd())
		{
			mState = EIDLE; //�ҋ@��Ԃֈڍs
		}
	}
}

void CXEnemy2::Attack_2()
{
	//mMoveDir�Ƀv���C���[�����̃x�N�g��������
	mMoveDir = mPlayerPoint.Normalize();
	//�U��2�A�j���[�V�������ł͂Ȃ��Ƃ��A�U���\�ȋ����܂Ń_�b�V���ŋ߂Â�
	if (mAnimationIndex != 4) {
		//�_�b�V���A�j���[�V����
		ChangeAnimation(6, true, 50);
		//�ړ��X�s�[�h��ύX
		mSpeed = SPEED_ATTACK_2;
	}

	//�U���\�ȋ����܂ŋ߂Â��ƍU��
	if (mPlayerDis <= ATTACK_DIS) {
		//�U��2�A�j���[�V����
		ChangeAnimation(4, false, 90);
	}

	if (mAnimationIndex == 4)
	{
		//�q�b�g���蔭��
		if (mAnimationFrame == 40) {
			mIsHit = true;
		}
		//�q�b�g����I��
		if (mAnimationFrame == 70) {
			mIsHit = false;
		}
		//�A�j���[�V�����I����
		if (mIsAnimationEnd())
		{
			mState = EIDLE;	//�ҋ@��Ԃֈڍs
		}
	}
}

void CXEnemy2::KnockBack()
{
	//�m�b�N�o�b�N�A�j���[�V����
	ChangeAnimation(8, false, 30);

	//�m�b�N�o�b�N����
	CVector KnockBackVec = mPlayerPoint.Normalize();
	//�m�b�N�o�b�N��
	float KnockBackAmount = 0.15f;

	//�m�b�N�o�b�N������
	mPosition -= KnockBackVec * KnockBackAmount;

	if (mAnimationIndex == 8)
	{
		//�A�j���[�V�����I����
		if (mIsAnimationEnd())
		{
			mState = EIDLE;	//�ҋ@��Ԃֈڍs
		}
	}
}

void CXEnemy2::Death()
{
	//���S�A�j���[�V����
	ChangeAnimation(7, false, 40);
}

void CXEnemy2::Stun()
{
	//�X�^���A�j���[�V����
	ChangeAnimation(9, true, 100);

	//�X�^�����Ԃ�����������
	mStunTime--;
	//�X�^�����Ԃ�0�ɂȂ�Ƒҋ@��Ԃֈڍs
	if (mStunTime <= 0) {
		mState = EIDLE;
	}
}


