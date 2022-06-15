#pragma once
#include "CCharacter.h"
#include "CColliderMesh.h"

//�}�b�v2�N���X
class CMap2 : public CCharacter {
private:
	static CMap2* mInstance; //�}�b�v2�̃C���X�^���X

	CColliderMesh mColliderMesh; //�}�b�v2�̃��b�V���R���C�_

	CMatrix mMatrixCol; //�R���C�_�̃}�g���N�X
public:
	//�R���X�g���N�^
	CMap2(CModel* model, CVector position, CVector rotation, CVector scale);
	~CMap2();

	static void Generate();	//����
	static void Release();	//���
	static CMap2* GetInstance(); //�C���X�^���X���擾
};