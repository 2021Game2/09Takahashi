#include "CXEnemy.h"

#define HP 100	//体力
#define DAMAGE_BODY 10	//ダメージ(体)
#define DAMAGE_HEAD 20	//ダメージ(頭)
#define INVINCIBLE_TIME 60

extern int EHp;	//敵の体力確認用、後で削除

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
	mColSphereBody.mTag = CCollider::EBODY;
	mColSphereHead.mTag = CCollider::EHEAD;
	mColSphereSword0.mTag = CCollider::ESWORD;
	mColSphereSword1.mTag = CCollider::ESWORD;
	mColSphereSword2.mTag = CCollider::ESWORD;
}

void CXEnemy::Init(CModelX* model)
{
	CXCharacter::Init(model);
	//合成行列の設定
	mColSphereBody.mpMatrix = &mpCombinedMatrix[1];
	//頭
	mColSphereHead.mpMatrix = &mpCombinedMatrix[1];
	//剣
	mColSphereSword0.mpMatrix = &mpCombinedMatrix[26];
	mColSphereSword1.mpMatrix = &mpCombinedMatrix[26];
	mColSphereSword2.mpMatrix = &mpCombinedMatrix[26];

	/*
	mColSphereBody.mpMatrix = &mpCombinedMatrix[8];
	//頭
	mColSphereHead.mpMatrix = &mpCombinedMatrix[11];
	//剣
	mColSphereSword.mpMatrix = &mpCombinedMatrix[21];
	*/
}

void CXEnemy::Update()
{
	if (mInvincibleTime > 0) {
		mInvincibleTime--;
	}
	else {
		mInvincibleFlag = false;
	}

	if (mHp <= 0) {
		//30フレームかけてダウンし、繰り返さない
		ChangeAnimation(11, false, 30);
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
