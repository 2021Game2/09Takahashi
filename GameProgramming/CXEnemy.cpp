#include "CXEnemy.h"
#include "CXPlayer.h"

#define HP 100	//�̗�
#define DAMAGE_BODY 10	//�_���[�W(��)
#define DAMAGE_HEAD 20	//�_���[�W(��)
#define INVINCIBLE_TIME 60

extern int EHp;	//�G�̗̑͊m�F�p�A��ō폜

CXEnemy::CXEnemy()
	: mColSphereBody(this, nullptr, CVector(0.5f, -1.0f, 0.0f), 1.0f)
	, mColSphereHead(this, nullptr, CVector(0.0f, 1.f, 0.0f), 1.5f)
	, mColSphereSword0(this, nullptr, CVector(0.7f, 3.5f, -0.2f), 0.5f)
	, mColSphereSword1(this, nullptr, CVector(0.5f, 2.5f, -0.2f), 0.5f)
	, mColSphereSword2(this, nullptr, CVector(0.3f, 1.5f, -0.2f), 0.5f)
	,mHp(HP)
	,mInvincibleTime(0)
	,mInvincibleFlag(false)
{
	mTag = EENEMY;

	mColSphereBody.mTag = CCollider::EBODY;
	mColSphereHead.mTag = CCollider::EHEAD;
	mColSphereSword0.mTag = CCollider::ESWORD;
	mColSphereSword1.mTag = CCollider::ESWORD;
	mColSphereSword2.mTag = CCollider::ESWORD;

	mState = EATTACK_1;
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
	case EATTACK_1:
		ChangeAnimation(7, true, 80);
		break;
	case EDEATH:
		//30�t���[�������ă_�E�����A�J��Ԃ��Ȃ�
		ChangeAnimation(11, false, 30);
		break;
	}
	
	if (mInvincibleTime > 0) {
		mInvincibleTime--;
	}
	else {
		mInvincibleFlag = false;
	}

	if (mHp <= 0) {
		mState = EDEATH;
	}

	if (mHp < 0) {
		mHp = 0;
	}

	EHp = mHp;

	CXCharacter::Update();
}

void CXEnemy::Collision(CCollider* m, CCollider* o)
{
	if (mInvincibleFlag == false) {
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
							//�v���C���[�̍U��1���󂯂���
							if (((CXPlayer*)(o->mpParent))->mState == CXPlayer::EATTACK_1) {
								switch (m->mTag) {
								case CCollider::EBODY:
									mHp -= DAMAGE_BODY;
									mInvincibleTime = INVINCIBLE_TIME;
									mInvincibleFlag = true;
									break;
								case CCollider::EHEAD:
									mHp -= DAMAGE_HEAD;
									mInvincibleTime = INVINCIBLE_TIME;
									mInvincibleFlag = true;
									break;
								default:
									break;
								}
							}
							//�v���C���[�̍U��2���󂯂���
							else if (((CXPlayer*)(o->mpParent))->mState == CXPlayer::EATTACK_2) {
								switch (m->mTag) {
								case CCollider::EBODY:
									mHp -= DAMAGE_BODY;
									mInvincibleTime = INVINCIBLE_TIME;
									mInvincibleFlag = true;
									break;
								case CCollider::EHEAD:
									mHp -= DAMAGE_HEAD;
									mInvincibleTime = INVINCIBLE_TIME;
									mInvincibleFlag = true;
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
}
