#include "CTrap.h"
#include "CXPlayer.h"
#include "CXEnemy.h"
#include "CRes.h"
#include "CTrapManager.h"
#include "CCollisionManager.h"

#define SCALE CVector(7.0f,7.0f,7.0f) //�X�P�[��

CTrap::CTrap()
	:mCollider(this, &mMatrix, CVector(0.0f, 0.0f, 0.0f), 1.0f)
	,mEnemyCol(false)
{
	//�^�O��ݒ�
	mTag = ETRAP;	//�
	mCollider.mTag = CCollider::EBODY;	//�{��

	mScale = SCALE; //�X�P�[����ݒ�

	mpModel = &CRes::sModelTrap; //���f����ݒ�
}

void CTrap::Update()
{
	//�G�Ɠ������Ă�����Y���W��������
	if (mEnemyCol == true) {
		mPosition.mY = -10.0f;
	}
	CTransform::Update();
}

void CTrap::Collision(CCollider* m, CCollider* o)
{
	//���ɓG�ɓ������Ă���ꍇ���^�[������
	if (mEnemyCol == true)return;

	//�����Ƒ��肪���R���C�_�̎�
	if (m->mType == CCollider::ESPHERE && o->mType == CCollider::ESPHERE)
	{
		//����̐e�̃^�O���G�̎�
		if (o->mpParent->mTag == EENEMY)
		{
			//�G�����S��Ԃ��ƃ��^�[������
			if (((CXEnemy*)(o->mpParent))->GetState() == CXEnemy::EDEATH)return;
			//�R���C�_�̃^�O���{�f�B�̎�
			if (o->mTag == CCollider::EBODY)
			{
				//�G�̏�Ԃ��X�^����Ԃł͖����Ƃ�
				if (((CXEnemy*)(o->mpParent))->GetState() != CXEnemy::ESTUN)
				{
					//���R���C�_���m�̏Փ˔���
					if (CCollider::Collision(m, o))
					{
						mEnemyCol = true; //�G�ɓ����������Ƃ�Ԃ�
						CTrapManager::GetInstance()->mMapTrap = false; //�}�b�v���㩂��Ȃ�����ɂ���
						CRes::sSETrapActive.Play(); //㩃A�C�e���쓮���̌��ʉ����Đ�����
					}
				}
			}
		}
	}
}

void CTrap::TaskCollision()
{
	CCollisionManager::Get()->Collision(&mCollider, COLLISIONRANGE);
}

//�ʒu��ݒ�
void CTrap::SetPos(CVector hpos)
{
	mPosition = hpos;
}

//��]��ݒ�@
void CTrap::SetRot(CVector hrot)
{
	mRotation = hrot;
}

