#pragma once
#include "CCharacter.h"
#include "CColliderMesh.h"

class CMap2 : public CCharacter {
private:
	CMatrix mMatrixCol;
	
	static CMap2* mInstance;

	CColliderMesh mColliderMesh;
public:
	//�R���X�g���N�^
	CMap2(CModel* model, CVector position, CVector rotation, CVector scale);
	~CMap2();

	static void Generate();	//����
	static void Release();	//���
	static CMap2* GetInstance(); //�C���X�^���X���擾
};