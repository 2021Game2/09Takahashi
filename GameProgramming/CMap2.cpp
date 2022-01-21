#include "CMap2.h"
#include "CTaskManager.h"

CMap2* CMap2::mInstance;

//ƒRƒ“ƒXƒgƒ‰ƒNƒ^
CMap2::CMap2(CModel* model, CVector position,
	CVector rotation, CVector scale)
{
	//ƒ‚ƒfƒ‹AˆÊ’uA‰ñ“]AŠgk‚ðÝ’è‚·‚é
	mpModel = model; //ƒ‚ƒfƒ‹‚ÌÝ’è
	mPosition = position; //ˆÊ’u‚ÌÝ’è
	mRotation = rotation; //‰ñ“]‚ÌÝ’è
	mScale = scale; //Šgk‚ÌÝ’è
	//—Dæ“x‚ð1‚É•ÏX‚·‚é
	mPriority = 1;
	CTaskManager::Get()->Remove(this); //íœ‚µ‚Ä
	CTaskManager::Get()->Add(this); //’Ç‰Á‚·‚é

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
