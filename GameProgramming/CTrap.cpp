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
	if (m->mType == CCollider::ESPHERE)
	{
		if (o->mType == CCollider::ESPHERE)
		{
			if (o->mpParent->mTag == EENEMY)
			{
				if (o->mTag == CCollider::EBODY)
				{
					if (CCollider::Collision(m, o))
					{
						if (((CXEnemy*)(o->mpParent))->mState != CXEnemy::ESTUN && ((CXEnemy*)(o->mpParent))->mState != CXEnemy::EDEATH)
						{
							mEnemyCol = true;
							CTrapManager::GetInstance()->mMapTrap = false;
							//㩃A�C�e���쓮���̌��ʉ����Đ�����
							SE_Trap_Active.Play();
						}
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

