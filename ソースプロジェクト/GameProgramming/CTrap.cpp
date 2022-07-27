#include "CTrap.h"
#include "CXPlayer.h"
#include "CXEnemy.h"
#include "CRes.h"
#include "CTrapManager.h"

#define SCALE CVector(7.0f,7.0f,7.0f) //�X�P�[��

CTrap::CTrap()
	:mCollider(this, &mMatrix, CVector(0.0f, 0.0f, 0.0f), 1.0f)
	,mIsEnemyCol(false)
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
	if (mIsEnemyCol == true) {
		mPosition.mY = -10.0f;
	}
	CTransform::Update();
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

//�G�ƏՓ˂��������f����t���O��ݒ�
void CTrap::SetIsEnemyCol(bool enemycol)
{
	mIsEnemyCol = enemycol;
}

//�G�ƏՓ˂��������f����t���O���擾
bool CTrap::GetIsEnemyCol()
{
	return mIsEnemyCol;
}

