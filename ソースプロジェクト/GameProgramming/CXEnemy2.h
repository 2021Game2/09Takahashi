#pragma once

#include "CXEnemy.h"

//�G�l�~�[(�G)�N���X2
class CXEnemy2 :public CXEnemy {
private:
	//�R���C�_
	CCollider mColSphereBody;	//��
	CCollider mColSphereHead;	//��
	CCollider mColSphereSword0;	//��
	CCollider mColSphereSword1;	//��

	void Idle();		//�ҋ@����
	void AutoMove();	//�ړ�����
	void Chase();		//�ǐՏ���
	void Attack_Idle();	//�U���ҋ@����
	void Attack_1();	//�U��1����
	void Attack_2();	//�U��2����
	void KnockBack();	//�m�b�N�o�b�N����
	void Death();		//���S����
	void Stun();		//�X�^������
public:
	CXEnemy2();		//�f�t�H���g�R���X�g���N�^
	~CXEnemy2();	//�f�X�g���N�^

	void Init(CModelX* model);		//������
	void CXEnemy2::TaskCollision();	//�Փˏ������Ă�
};