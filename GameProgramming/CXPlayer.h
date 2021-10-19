#ifndef CXPLAYER_H
#define CXPLAYER_H

#include "CXCharacter.h"
#include "CCollider.h"

class CXPlayer : public CXCharacter
{
private:
	int mStamina;	//�X�^�~�i
	bool mDash;		//�_�b�V����Ԃ̎���true��Ԃ�
	bool mAvoid;	//�����Ԃ̎���true��Ԃ�
	int mAvoidTime;	//�������
	float mAvoidSpeed;	//��𒆂̃X�s�[�h
	bool mMoveFlag;	//�ړ���true��Ԃ�
	float mSpeed;	//�X�s�[�h
	CVector mAvoidMove;	//�������Move��ۑ�
	int mInvincibleTime;	//���G����
	bool mInvincible;	//���G��Ԃ̎�true��Ԃ�

	CVector mMove;	

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
};

#endif
