#include "CMap2.h"
#include "CTaskManager.h"

CMap2* CMap2::mInstance;

//コンストラクタ
CMap2::CMap2(CModel* model, CVector position,
	CVector rotation, CVector scale)
{
	//モデル、位置、回転、拡縮を設定する
	mpModel = model; //モデルの設定
	mPosition = position; //位置の設定
	mRotation = rotation; //回転の設定
	mScale = scale; //拡縮の設定
	//優先度を1に変更する
	mPriority = 1;
	CTaskManager::Get()->Remove(this); //削除して
	CTaskManager::Get()->Add(this); //追加する

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
