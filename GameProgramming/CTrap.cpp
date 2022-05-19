#include "CTrap.h"
#include "CXPlayer.h"
#include "CXEnemy.h"
#include "CRes.h"
#include "CTrapManager.h"
#include "CSound.h"
#include "CCollisionManager.h"

extern CSound SE_Trap_Active;	//㩃A�C�e���쓮���̌��ʉ�

CTrap* CTrap::mInstance;

CTrap::CTrap()
	:mCollider(this, &mMatrix, CVector(0.0f, 0.0f, 0.0f), 1.0f)
	,mEnemyCol(false)
{
	mTag = ETRAP;	//�
	mCollider.mTag = CCollider::EBODY;	//�{��
	mScale = CVector(7.0f, 7.0f, 7.0f);

	mpModel = &CTrapManager::sTrap;

	mInstance = this;
}

void CTrap::Update()
{
	//����������Y���W��������
	if (mEnemyCol == true) {
		mPosition.mY = -10.0f;
	}
	CTransform::Update();
}

void CTrap::Collision(CCollider* m, CCollider* o)
{
	//�����Ƒ��肪���R���C�_�̎�
	if (m->mType == CCollider::ESPHERE && o->mType == CCollider::ESPHERE)
	{
		//����̐e�̃^�O���G�̎�
		if (o->mpParent->mTag == EENEMY)
		{
			//�G�����S��Ԃ��ƃ��^�[������
			if (((CXEnemy*)(o->mpParent))->mState == CXEnemy::EDEATH)return;
			//�R���C�_�̃^�O���{�f�B�̎�
			if (o->mTag == CCollider::EBODY)
			{
				//�G�̏�Ԃ��X�^����Ԃł͖����Ƃ�
				if (((CXEnemy*)(o->mpParent))->mState != CXEnemy::ESTUN)
				{
					//���R���C�_���m�̏Փ˔���
					if (CCollider::Collision(m, o))
					{
						mEnemyCol = true; //�G�ɓ����������Ƃ�Ԃ�
						CTrapManager::GetInstance()->mMapTrap = false; //�}�b�v���㩂��Ȃ�����ɂ���
						SE_Trap_Active.Play(); //㩃A�C�e���쓮���̌��ʉ����Đ�����
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

CTrap* CTrap::GetInstance()
{
	return mInstance;
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

