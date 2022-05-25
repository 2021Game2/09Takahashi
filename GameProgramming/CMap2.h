#pragma once
#include "CCharacter.h"
#include "CColliderMesh.h"

//マップ2クラス
class CMap2 : public CCharacter {
private:
	static CMap2* mInstance; //マップ2のインスタンス

	CColliderMesh mColliderMesh; //マップ2のメッシュコライダ

	CMatrix mMatrixCol; //コライダのマトリクス
public:
	//コンストラクタ
	CMap2(CModel* model, CVector position, CVector rotation, CVector scale);
	~CMap2();

	static void Generate();	//生成
	static void Release();	//解放
	static CMap2* GetInstance(); //インスタンスを取得
};