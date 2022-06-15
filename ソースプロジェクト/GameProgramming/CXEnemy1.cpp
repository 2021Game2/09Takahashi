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
	//������
	Init(&CRes::sModelXEnemy);

	//�R���C�_�̃^�O��ݒ�
	mColSphereBody.mTag = CCollider::EBODY;		//�{��
	mColSphereHead.mTag = CCollider::EHEAD;		//��
	mColSphereSword0.mTag = CCollider::ESWORD;	//��
	mColSphereSword1.mTag = CCollider::ESWORD;	//��
	mColSphereSword2.mTag = CCollider::ESWORD;	//��

	//�G�̎�ނ�ݒ�
	mEnemyType = ETYPE_1;
}

CXEnemy1::~CXEnemy1()
{
}

void CXEnemy1::Init(CModelX* model)
{
	CXCharacter::Init(model);
	//�����s��̐ݒ�
	mColSphereBody.mpMatrix = &mpCombinedMatrix[1];		//��
	mColSphereHead.mpMatrix = &mpCombinedMatrix[1];		//��
	mColSphereSword0.mpMatrix = &mpCombinedMatrix[26];	//��
	mColSphereSword1.mpMatrix = &mpCombinedMatrix[26];	//��
	mColSphereSword2.mpMatrix = &mpCombinedMatrix[26];	//��
}

void CXEnemy1::TaskCollision()
{
	CCollisionManager::Get()->Collision(&mColSphereBody, COLLISIONRANGE);
	CCollisionManager::Get()->Collision(&mColSphereHead, COLLISIONRANGE);
	CCollisionManager::Get()->Collision(&mColSphereSword0, COLLISIONRANGE);
	CCollisionManager::Get()->Collision(&mColSphereSword1, COLLISIONRANGE);
	CCollisionManager::Get()->Collision(&mColSphereSword2, COLLISIONRANGE);
}
