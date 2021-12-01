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

	int mHp;		//�̗�

	int mStunTime;	//㩂ɂ����������̃X�^�����鎞��

	bool mAttackFlag_1;		//�U��1��Ԃ̎�true��Ԃ�

	CVector mPoint;			//�ړ����̖ڕW�n�_
	CVector mPlayerPoint;	//�ǐՎ��̖ڕW�n�_
	float mPlayerDis;		//�v���C���[�܂ł̋���

	CVector mMoveDir;		//�ړ��������
	float mSpeed;			//�ړ����鑬�x

	CVector mRot;
	float mDot;

	static  CXEnemy* mInstance;

	void Idle();		//�ҋ@����
	void AutoMove();	//�ړ�����
	void Chase();		//�ǐՏ���
	void Attack_1();	//�U��1����
	void KnockBack();	//�m�b�N�o�b�N����
	void Death();		//���S����
	void Stun();
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
		EAUTOMOVE,	//�ړ�
		ECHASE,		//�ǐ�
		EATTACK_1,	//�U��1
		EKNOCKBACK,	//�m�b�N�o�b�N
		EDEATH,		//���S
		ESTUN,		//�X�^��
	};
	EEnemyState mState;	//���

	static CXEnemy* GetInstance();

	void SetPos(CVector hpos);	//�ʒu��ݒ�
	CVector GetPos();	//�ʒu���擾
};

#endif
