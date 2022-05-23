#pragma once
#include "CCharacter.h"
#include "CColliderMesh.h"

class CMap2 : public CCharacter {
private:
	CMatrix mMatrixCol;
	
	static CMap2* mInstance;

	CColliderMesh mColliderMesh;
public:
	//コンストラクタ
	CMap2(CModel* model, CVector position, CVector rotation, CVector scale);
	~CMap2();

	static void Generate();	//生成
	static void Release();	//解放
	static CMap2* GetInstance(); //インスタンスを取得
};