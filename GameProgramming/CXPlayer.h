#ifndef CXPLAYER_H
#define CXPLAYER_H

#include "CXCharacter.h"
#include "CCollider.h"
#include "CText.h"
#include "CTexture.h"

class CXPlayer : public CXCharacter
{
private:
	CText mFont;
	CTexture mTexture;
	CTexture mItemTexture;
	CTexture mItemTexture2;

	//�R���C�_�̐錾
	CCollider mColSphereBody;	//��
	CCollider mColSphereHead;	//��
	CCollider mColSphereSword;	//��

	static CXPlayer* mInstance;

	int mHp;		//�̗�
	int mStamina;	//�X�^�~�i
	float mSpeed;	//�X�s�[�h

	CVector mMoveKeep;		//Move��ۑ�����

	bool mAvoid;			//�����Ԃ̎���true��Ԃ�
	int mAvoidTime;			//�������
	float mAvoidSpeed;		//��𒆂̃X�s�[�h

	int mInvincibleTime;	//���G����
	bool mInvincibleFlag;	//���G��Ԃ̎�true��Ԃ�

	bool mAttackFlag_1;		//�U��1��Ԃ̎�true��Ԃ�
	bool mAttackFlag_2;		//�U��2��Ԃ̎�true��Ԃ�
	float mAttack2Speed;	//�U��2�̎��̃X�s�[�h
	bool mAttackFlag_3;		//�U��3��Ԃ̎�true��Ԃ�

	int mGraceTime;			//�h���U���̎�t����
	int mCombo;				//�h���U����A���ōs������

	CVector mMoveDir;	
	CVector mMove2;

	CVector mMoveSide;
	CVector mMoveFront;

	CVector mSideVec;
	CVector mFrontVec;
	float mTurnspeed;

	enum EItem {
		HEAD = 0,
		ETRAP,		//�
		EPORTION,	//�񕜖�
		TAIL,
	};
	int mItemSelect;		//�I�𒆂̃A�C�e��
	int mPortionQuantity;	//�񕜖�̏�����

	void Idle();		//�ҋ@����
	void Move();		//�ړ�����
	void Dash();		//�_�b�V������
	void Attack_1();	//�U��1����
	void Attack_2();	//�U��2����
	void Attack_3();	//�U��3����
	void Avoid();		//�������
	void Death();		//���S����
	void ItemUse();		//�A�C�e���g�p����
	void ItemSelect();	//�A�C�e���I������
	void KnockBack();	//�m�b�N�o�b�N����
public:
	CXPlayer();

	/*
	������(X���f���N���X�̃|�C���^)
	*/
	void Init(CModelX* model);

	void Update();
	void Render2D();

	//�Փˏ���
	//m:�����̃R���C�_ o:����̃R���C�_
	void Collision(CCollider* m, CCollider* o);

	static CXPlayer* GetInstance();

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
		EITEMUSE,	//�A�C�e���g�p
		EKNOCKBACK,	//�m�b�N�o�b�N
	};
	EPlayerState mState;

	bool mHit;	//�U������true��Ԃ��@�G�ɍU����������or�U���I������false��Ԃ�
	bool mAttackFlag_Once; //�U�������u�Ԃ���true��Ԃ�
};

#endif
