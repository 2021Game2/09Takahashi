#pragma once

#include "CXEnemy.h"

//�G�l�~�[(�G)�N���X2
class CXEnemy2 :public CXEnemy {
public:
	CXEnemy2();
	~CXEnemy2();

	//�R���C�_
	CCollider mColSphereBody;	//��
	CCollider mColSphereHead;	//��
	CCollider mColSphereSword0;	//��
	CCollider mColSphereSword1;	//��

	void Init(CModelX* model);		//������
	void CXEnemy2::TaskCollision();	//�Փˏ������Ă�

	void Idle();		//�ҋ@����
	void AutoMove();	//�ړ�����
	void Chase();		//�ǐՏ���
	void Attack_Idle();	//�U���ҋ@����
	void Attack_1();	//�U��1����
	void Attack_2();	//�U��2����
	void KnockBack();	//�m�b�N�o�b�N����
	void Death();		//���S����
	void Stun();		//�X�^������
};