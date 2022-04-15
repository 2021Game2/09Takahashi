#include "CTrap.h"
#include "CXPlayer.h"
#include "CXEnemy.h"
#include "CRes.h"
#include "CTrapManager.h"
#include "CSound.h"
#include "CCollisionManager.h"

extern CSound SE_Trap_Active;	//罠アイテム作動時の効果音

CTrap* CTrap::mInstance;

CTrap::CTrap()
	:mCollider(this, &mMatrix, CVector(0.0f, 0.0f, 0.0f), 1.0f)
	,mEnemyCol(false)
{
	mTag = ETRAP;	//罠
	mCollider.mTag = CCollider::EBODY;	//本体
	mScale = CVector(7.0f, 7.0f, 7.0f);

	mpModel = &CTrapManager::sTrap;

	mInstance = this;
}

void CTrap::Update()
{
	//当たったらY座標を下げる
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
							//罠アイテム作動時の効果音を再生する
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

//位置を設定
void CTrap::SetPos(CVector hpos)
{
	mPosition = hpos;
}

//回転を設定　
void CTrap::SetRot(CVector hrot)
{
	mRotation = hrot;
}

