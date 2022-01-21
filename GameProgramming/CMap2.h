#pragma once
#include "CCharacter.h"
#include "CColliderMesh.h"

class CMap2 : public CCharacter {
private:
	CMatrix mMatrixCol;
	
	static CMap2* mInstance;
public:
	//コンストラクタ
	CMap2(CModel* model, CVector position, CVector rotation, CVector scale);
	~CMap2();

	CColliderMesh mColliderMesh;

	static CMap2* GetInstance();
	static void Release();
};