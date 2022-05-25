#include "CMap2.h"
#include "CTaskManager.h"
#include "CRes.h"

#define MODEL_CYLINDER "Resource\\Cylinder.obj", "Resource\\Cylinder.mtl" //シリンダーモデル

#define POSITION CVector(0.0f, -5.0f, 0.0f) //位置
#define ROTATION CVector() //回転
#define SCALE CVector(4.0f, 3.0f, 4.0f) //スケール

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

	CModel Cylinder; //シリンダーモデル
	Cylinder.Load(MODEL_CYLINDER); //シリンダーモデル読み込み
	mMatrixCol = CMatrix().Scale(1.0f, 1.0f, 1.0f) * mMatrix; //スケール
	mColliderMesh.Set(this, &mMatrixCol, &Cylinder); //コライダメッシュを設定

	//タグを設定
	mTag = EMAP; //マップ
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
