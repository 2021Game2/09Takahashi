#ifndef CXPLAYER_H
#define CXPLAYER_H

#include "CXCharacter.h"
#include "CCollider.h"

class CXPlayer : public CXCharacter
{
private:
	//�R���C�_�̐錾
	CCollider mColSphereBody;	//��
	CCollider mColSphereHead;	//��
	CCollider mColSphereSword;	//��

	static CXPlayer* mInstance;

	int mHp;		//�̗�
	int mStamina;	//�X�^�~�i
	float mSpeed;	//�X�s�[�h

	CVector mMoveKeep;	//Move��ۑ�����

	bool mAvoid;	//�����Ԃ̎���true��Ԃ�
	int mAvoidTime;	//�������
	float mAvoidSpeed;	//��𒆂̃X�s�[�h

	int mInvincibleTime;	//���G����
	bool mInvincibleFlag;	//���G��Ԃ̎�true��Ԃ�

	bool mAttackFlag_1;	//�U��1��Ԃ̎�true��Ԃ�
	bool mAttackFlag_2;	//�U��2��Ԃ̎�true��Ԃ�
	float mAttack2Speed;	//�U��2�̎��̃X�s�[�h
	bool mAttackFlag_3;	//�U��3��Ԃ̎�true��Ԃ�

	int mGraceTime;	//�h���U���̎�t����

	CVector mMove;	
	CVector mMove2;
	CVector mSideVec;
	CVector mFrontVec;
	float mTurnspeed;

	void Idle();		//�ҋ@����
	void Move();		//�ړ�����
	void Dash();		//�_�b�V������
	void Attack_1();	//�U��1����
	void Attack_2();	//�U��2����
	void Attack_3();	//�U��3����
	void Avoid();		//�������
	void Death();		//���S����

public:
	CXPlayer();

	/*
	������(X���f���N���X�̃|�C���^)
	*/
	void Init(CModelX* model);

	void Update();

	//�Փˏ���
	//m:�����̃R���C�_ o:����̃R���C�_
	void Collision(CCollider* m, CCollider* o);

	//�v���C���[�̏��
	enum EPlayerState
	{
		EIDLE,		//�ҋ@
		EMOVE,		//�ړ�
		EDASH,		//�_�b�V��
		EAVOID,		//���
		EATTACK_1,	//�U��1
		EATTACK_2,	//�U��2
		EATTACK_3,	//�U��3(�U��1����h��)
		EDEATH,		//���S
	};
	EPlayerState mState;

	bool mHit;	//�U������true��Ԃ��@�G�ɍU����������or�U���I������false��Ԃ�

	static CXPlayer* GetInstance();
};

#endif
