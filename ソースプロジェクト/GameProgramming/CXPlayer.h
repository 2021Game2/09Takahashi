#ifndef CXPLAYER_H
#define CXPLAYER_H

#include "CXCharacter.h"
#include "CCollider.h"
#include "CXEnemy.h"

//�v���C���[�N���X
class CXPlayer : public CXCharacter
{
public:
	//�v���C���[�̏��
	enum EPlayerState
	{
		EIDLE = 0,	//�ҋ@
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
private:
	//�R���C�_�̐錾
	CCollider mColSphereBody;	//��
	CCollider mColSphereHead;	//��
	CCollider mColSphereSword;	//��

	static CXPlayer* mInstance;	 //�v���C���[�̃C���X�^���X

	CXEnemy* mpTargetEnemy;	//�U���Ώۂ̓G������

	int mHp;		//�̗�
	int mStamina;	//�X�^�~�i
	float mSpeed;	//�X�s�[�h

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
	int mComboCount;		//�h���U����A���ōs������

	CVector mMove;			//�����Ƒ��x���������킹���x�N�g���A�v���C���[�̈ړ���
	CVector mMoveDir;		//�ړ���������A���f���̉�]�ɂ��g�p���Ă���A���t���[�����Z�b�g�����
	CVector mMoveDirKeep;	//�ړ����̕�����ۑ�����
	CVector mKnockBackDir;	//�m�b�N�o�b�N���������ێ�����
	CVector mAttackDir;		//�U�����̕�����ێ�����

	CVector mSideVec;		//�J�����̉��x�N�g��
	CVector mFrontVec;		//�J�����̐��ʃx�N�g��
	float mTurnspeed;		//�^�[���̑��x

	//�A�C�e��
	enum EItem {
		HEAD = 0,
		ETRAP,		//�
		EPORTION,	//�񕜖�
		TAIL,
	};
	int mItemSelect;		//�I�𒆂̃A�C�e��
	int mPortionQuantity;	//�񕜖�̏�����
	bool mIsItemUse();		//�A�C�e�����g�p�\�Ȏ���true��Ԃ�

	void Idle();		//�ҋ@����
	void MoveCamera();	//�J��������ɂ����ړ�����
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

	EPlayerState mState;	//�v���C���[�̏�Ԕ��f�p
public:
	CXPlayer();
	~CXPlayer();

	/*
	������(X���f���N���X�̃|�C���^)
	*/
	void Init(CModelX* model);

	void Update();		//�X�V
	void Render2D();	//2D�`��

	//�Փˏ���
	//m:�����̃R���C�_ o:����̃R���C�_
	void CXPlayer::Collision(CCollider* m, CCollider* o);
	void CXPlayer::TaskCollision();

	static void Generate();	//����
	static void Release();	//���
	static CXPlayer* GetInstance(); //�C���X�^���X���擾

	bool mHit;	//�U������true��Ԃ��@�G�ɍU����������or�U���I������false��Ԃ�
	bool mAttackFlag_Once; //�U�������u�Ԃ���true��Ԃ�

	CVector GetSwordColPos();	//���̃R���C�_�̍��W���擾����

	CXPlayer::EPlayerState GetState();	//�v���C���[�̏�Ԃ��擾����
};

#endif
