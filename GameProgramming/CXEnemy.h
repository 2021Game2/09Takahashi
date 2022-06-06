#ifndef CXENEMY_H
#define CXENEMY_H

#include "CXCharacter.h"
#include "CCollider.h"

//�G�l�~�[(�G)�̃x�[�X�N���X
class CXEnemy : public CXCharacter
{
protected:
	int mHp;		//�̗�
	int mHpMax;

	int mStunTime;	//㩂ɂ����������̃X�^�����鎞��

	CVector mPoint;			//�ړ����̖ڕW�n�_
	CVector mPlayerPoint;	//�ǐՎ��̖ڕW�n�_
	float mPlayerDis;		//�v���C���[�܂ł̋���

	int mChaseRestartCount;	//�v���C���[�����ꂽ�Ƃ��ɁA�ǐՍĊJ����܂ł̃J�E���g

	CVector mMoveDir;		//�ړ��������
	float mSpeed;			//�ړ����鑬�x

	CVector mRot;			//��]
	float mDot;				//����

	bool mIsInvincible;	//���G�t���O�A�v���C���[�̍U�����q�b�g�����true��Ԃ�

	virtual void Idle();		//�ҋ@����
	virtual void AutoMove();	//�ړ�����
	virtual void Chase();		//�ǐՏ���
	virtual void Attack_Idle();	//�U���ҋ@����
	virtual void Attack_1();	//�U��1����
	virtual void Attack_2();	//�U��2����
	virtual void KnockBack();	//�m�b�N�o�b�N����
	virtual void Death();		//���S����
	virtual void Stun();		//�X�^������
	virtual void Avoid();		//�������
public:

	CXEnemy();
	/*
	������(X���f���N���X�̃|�C���^)
	*/
	virtual void Init(CModelX* model);

	//�Փˏ���
	//m:�����̃R���C�_ o:����̃R���C�_
	void CXEnemy::Collision(CCollider* m, CCollider* o);
	void CXEnemy::TaskCollision();

	void Update();
	void Render2D();

	//�G�̏��
	enum EEnemyState
	{
		EIDLE = 0,		//�ҋ@
		EAUTOMOVE,		//�ړ�
		ECHASE,			//�ǐ�
		EATTACK_IDLE,	//�U���ҋ@
		EATTACK_1,		//�U��1
		EATTACK_2,		//�U��2
		EKNOCKBACK,		//�m�b�N�o�b�N
		EDEATH,			//���S
		ESTUN,			//�X�^��
		EAVOID,			//���
	};
	EEnemyState mState;	//���

	//�G�̎��
	enum EEnemyType {
		ETYPE_1 = 0,	//�^�C�v1
		ETYPE_2			//�^�C�v2
	};
	EEnemyType mEnemyType; //�G�̎�ޔ��f�p

	void SetPos(CVector hpos);	//�ʒu��ݒ�
	CVector GetPos();			//�ʒu���擾

	void SetHp(int hp);			//�̗͂�ݒ�

	bool mHit;	//�U������true��Ԃ��@�G�ɍU����������or�U���I������false��Ԃ�

	bool mIsTarget; //�v���C���[�̍U�����̑ΏۂɂȂ��Ă��鎞true��Ԃ�

	bool mIsAttack();	//�U����Ԃ̎���true��Ԃ�
	bool mIsDeath();	//���S��Ԃ̎���true��Ԃ�
	bool mIsKnockBack();//�m�b�N�o�b�N���\�ȂƂ�true��Ԃ�
};

#endif
