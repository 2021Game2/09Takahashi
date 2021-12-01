#include "CTrap.h"
#include "CXPlayer.h"
#include "CRes.h"

CTrap* CTrap::mInstance;

CTrap::CTrap()
	:mCollider(this, &mMatrix, CVector(0.0f, 0.0f, 0.0f), 1.0f)
{
	Init(&CRes::sModelX);

	mTag = ETRAP;	//罠
	mCollider.mTag = CCollider::EBODY;	//本体
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
						mEnabled = false;	//敵に当たると消える
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

//位置を設定
void CTrap::SetPos(CVector hpos)
{
	mPosition = hpos;
}

