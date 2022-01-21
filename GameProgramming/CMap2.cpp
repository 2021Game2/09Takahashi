#include "CMap2.h"
#include "CTaskManager.h"

CMap2* CMap2::mInstance;

//�R���X�g���N�^
CMap2::CMap2(CModel* model, CVector position,
	CVector rotation, CVector scale)
{
	//���f���A�ʒu�A��]�A�g�k��ݒ肷��
	mpModel = model; //���f���̐ݒ�
	mPosition = position; //�ʒu�̐ݒ�
	mRotation = rotation; //��]�̐ݒ�
	mScale = scale; //�g�k�̐ݒ�
	//�D��x��1�ɕύX����
	mPriority = 1;
	CTaskManager::Get()->Remove(this); //�폜����
	CTaskManager::Get()->Add(this); //�ǉ�����

	CTransform::Update();

	CModel Cylinder;
	Cylinder.Load("Cylinder.obj", "Cylinder.mtl");
	mMatrixCol = CMatrix().Scale(1.0f, 1.0f, 1.0f) * mMatrix;
	mColliderMesh.Set(this, &mMatrixCol, &Cylinder);

	mTag = EMAP;

	mInstance = this;
}

CMap2::~CMap2()
{
}

CMap2* CMap2::GetInstance()
{
	return mInstance;
}

void CMap2::Release()
{
	if (mInstance) {
		delete mInstance;
		mInstance = NULL;
	}
}
