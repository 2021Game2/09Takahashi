#include "CMap2.h"
#include "CTaskManager.h"
#include "CRes.h"

#define MODEL_CYLINDER "Resource\\Cylinder.obj", "Resource\\Cylinder.mtl" //�V�����_�[���f��

#define POSITION CVector(0.0f, -5.0f, 0.0f) //�ʒu
#define ROTATION CVector() //��]
#define SCALE CVector(4.0f, 3.0f, 4.0f) //�X�P�[��

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

	CModel Cylinder; //�V�����_�[���f��
	Cylinder.Load(MODEL_CYLINDER); //�V�����_�[���f���ǂݍ���
	mMatrixCol = CMatrix().Scale(1.0f, 1.0f, 1.0f) * mMatrix; //�X�P�[��
	mColliderMesh.Set(this, &mMatrixCol, &Cylinder); //�R���C�_���b�V����ݒ�

	//�^�O��ݒ�
	mTag = EMAP; //�}�b�v
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

void CMap2::Generate()
{
	mInstance = new CMap2(&CRes::sModelMap2, POSITION, ROTATION, SCALE);
}
