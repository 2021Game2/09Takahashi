#pragma once

#include "CXEnemy.h"

//�G�l�~�[(�G)�N���X1
class CXEnemy1 :public CXEnemy {
private:
	//�R���C�_
	CCollider mColSphereBody;	//��
	CCollider mColSphereHead;	//��
	CCollider mColSphereSword0;	//��
	CCollider mColSphereSword1;	//��
	CCollider mColSphereSword2;	//��
public:
	CXEnemy1();		//�f�t�H���g�R���X�g���N�^
	~CXEnemy1();	//�f�X�g���N�^

	void Init(CModelX* model);		//������
	void CXEnemy1::TaskCollision();	//�Փˏ������Ă�
};
