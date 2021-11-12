#ifndef CXENEMY_H
#define CXENEMY_H

#include "CXCharacter.h"
#include "CCollider.h"

class CXEnemy : public CXCharacter
{
private:
	CCollider mColSphereSword0;	//��
	CCollider mColSphereSword1;	//��
	CCollider mColSphereSword2;	//��

	int mHp;	//�̗�

	bool mAttackFlag_1;	//�U��1��Ԃ̎�true��Ԃ�

	CVector mPlayerPosition;	//�v���C���[�̃|�W�V����

	CVector mPoint;	//�ړ����̖ڕW�n�_

	void Idle();	//�ҋ@����
	void Move();	//�ړ�����
	void Chase();
	void Attack_1();	//�U��1����
	void Death();	//���S����
public:
	//�R���C�_�̐錾
	CCollider mColSphereBody;	//��
	CCollider mColSphereHead;	//��
//	CCollider mColSphereSword;	//��

	CXEnemy();
	/*
	������(X���f���N���X�̃|�C���^)
	*/
	void Init(CModelX* model);

	//�Փˏ���
	//m:�����̃R���C�_ o:����̃R���C�_
	void Collision(CCollider* m, CCollider* o);

	void Update();

	//�G�̏��
	enum EEnemyState
	{
		EIDLE,		//�ҋ@
		EMOVE,		//�ړ�
		ECHASE,		//�ǐ�
		EATTACK_1,	//�U��1
		EDEATH,		//���S
	};
	EEnemyState mState;
};

#endif
