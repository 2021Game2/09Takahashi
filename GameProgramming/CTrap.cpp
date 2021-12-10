#include "CTrap.h"
#include "CXPlayer.h"
#include "CXEnemy.h"
#include "CRes.h"

CTrap* CTrap::mInstance;

CTrap::CTrap()
	:mCollider(this, &mMatrix, CVector(0.0f, 0.0f, 0.0f), 1.0f)
{
	Init(&CRes::sModelX);

	mTag = ETRAP;	//ã©
	mCollider.mTag = CCollider::EBODY;	//–{‘Ì
	mScale = CVector(0.3f, 0.3f, 0.3f);

	mInstance = this;
}

void CTrap::Init(CModelX* model)
{
	CXCharacter::Init(model);
}

void CTrap::Update()
{
	CXCharacter::Update();
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
							mEnabled = false;	//“G‚É“–‚½‚é‚ÆÁ‚¦‚é
						}
					}
				}
			}
		}
	}
}

CTrap* CTrap::GetInstance()
{
	return mInstance;
}

//ˆÊ’u‚ğİ’è
void CTrap::SetPos(CVector hpos)
{
	mPosition = hpos;
}

//‰ñ“]‚ğİ’è@
void CTrap::SetRot(CVector hrot)
{
	mRotation = hrot;
}

