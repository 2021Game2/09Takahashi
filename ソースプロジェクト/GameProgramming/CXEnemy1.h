#pragma once

#include "CXEnemy.h"

//�G�l�~�[(�G)�N���X1
class CXEnemy1 :public CXEnemy {
public:
	CXEnemy1();
	~CXEnemy1();

	//�R���C�_
	CCollider mColSphereBody;	//��
	CCollider mColSphereHead;	//��
	CCollider mColSphereSword0;	//��
	CCollider mColSphereSword1;	//��
	CCollider mColSphereSword2;	//��

	void Init(CModelX* model);		//������
	void CXEnemy1::TaskCollision();	//�Փˏ������Ă�
private:

};
