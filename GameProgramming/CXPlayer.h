#ifndef CXPLAYER_H
#define CXPLAYER_H

#include "CXCharacter.h"
#include "CCollider.h"

class CXPlayer : public CXCharacter
{
private:
	int mStamina;	//�X�^�~�i
	float mSpeed;	//�X�s�[�h

	bool mAvoid;	//�����Ԃ̎���true��Ԃ�
	int mAvoidTime;	//�������
	float mAvoidSpeed;	//��𒆂̃X�s�[�h
	CVector mAvoidMove;	//�������Move��ۑ�

	int mInvincibleTime;	//���G����
	bool mInvincible;	//���G��Ԃ̎�true��Ԃ�

	bool mAttackFlag_1;	//�U��1��Ԃ̎�true��Ԃ�

	CVector mMove;	
	CVector mMove2;
	CVector mSideVec;
	CVector mFrontVec;
	float mTurnspeed;

	void Idle();		//�ҋ@����
	void Move();		//�ړ�����
	void Dash();		//�_�b�V������
	void Attack_1();	//�U��1����
	void Avoid();		//�������

public:
	//�R���C�_�̐錾
	CCollider mColSphereBody;	//��
	CCollider mColSphereHead;	//��
	CCollider mColSphereSword;	//��

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
	};
	EPlayerState mState;
};

#endif
