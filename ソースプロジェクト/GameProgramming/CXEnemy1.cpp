#include "CXEnemy1.h"
#include "CRes.h"
#include "CCollisionManager.h"

CXEnemy1::CXEnemy1()
	:mColSphereBody(this, nullptr, CVector(0.5f, -1.0f, 0.0f), 1.2f)
	, mColSphereHead(this, nullptr, CVector(0.0f, 1.f, 0.0f), 1.2f)
	, mColSphereSword0(this, nullptr, CVector(0.7f, 3.5f, -0.2f), 0.8f)
	, mColSphereSword1(this, nullptr, CVector(0.5f, 2.5f, -0.2f), 0.8f)
	, mColSphereSword2(this, nullptr, CVector(0.3f, 1.5f, -0.2f), 0.8f)
{
	//初期化
	Init(&CRes::sModelXEnemy);

	//コライダのタグを設定
	mColSphereBody.mTag = CCollider::EBODY;		//本体
	mColSphereHead.mTag = CCollider::EHEAD;		//頭
	mColSphereSword0.mTag = CCollider::ESWORD;	//剣
	mColSphereSword1.mTag = CCollider::ESWORD;	//剣
	mColSphereSword2.mTag = CCollider::ESWORD;	//剣

	//敵の種類を設定
	mEnemyType = ETYPE_1;
}

CXEnemy1::~CXEnemy1()
{
}

void CXEnemy1::Init(CModelX* model)
{
	CXCharacter::Init(model);
	//合成行列の設定
	mColSphereBody.mpMatrix = &mpCombinedMatrix[1];		//体
	mColSphereHead.mpMatrix = &mpCombinedMatrix[1];		//頭
	mColSphereSword0.mpMatrix = &mpCombinedMatrix[26];	//剣
	mColSphereSword1.mpMatrix = &mpCombinedMatrix[26];	//剣
	mColSphereSword2.mpMatrix = &mpCombinedMatrix[26];	//剣
}

void CXEnemy1::TaskCollision()
{
	CCollisionManager::Get()->Collision(&mColSphereBody, COLLISIONRANGE);
	CCollisionManager::Get()->Collision(&mColSphereHead, COLLISIONRANGE);
	CCollisionManager::Get()->Collision(&mColSphereSword0, COLLISIONRANGE);
	CCollisionManager::Get()->Collision(&mColSphereSword1, COLLISIONRANGE);
	CCollisionManager::Get()->Collision(&mColSphereSword2, COLLISIONRANGE);
}
