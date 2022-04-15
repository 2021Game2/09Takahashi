#ifndef CXENEMY_H
#define CXENEMY_H

#include "CXCharacter.h"
#include "CCollider.h"
#include "CTexture.h"
#include "CText.h"

class CXEnemy : public CXCharacter
{
private:
	CText mFont; //�t�H���g
	CTexture mImageGauge; //�Q�[�W�摜

	CCollider mColSphereSword0;	//��
	CCollider mColSphereSword1;	//��
	CCollider mColSphereSword2;	//��

	int mHp;		//�̗�

	int mStunTime;	//㩂ɂ����������̃X�^�����鎞��

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
	void Attack_2();	//�U��2����
	void KnockBack();	//�m�b�N�o�b�N����
	void Death();		//���S����
	void Stun();		//�X�^������
	void Avoid();		//�������
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
	void CXEnemy::Collision(CCollider* m, CCollider* o);
	void CXEnemy::TaskCollision();

	void Update();
	void Render2D();

	//�G�̏��
	enum EEnemyState
	{
		EIDLE,		//�ҋ@
		EAUTOMOVE,	//�ړ�
		ECHASE,		//�ǐ�
		EATTACK_1,	//�U��1
		EATTACK_2,	//�U��2
		EKNOCKBACK,	//�m�b�N�o�b�N
		EDEATH,		//���S
		ESTUN,		//�X�^��
		EAVOID,		//���
	};
	EEnemyState mState;	//���

	static CXEnemy* GetInstance();

	void SetPos(CVector hpos);	//�ʒu��ݒ�
	CVector GetPos();			//�ʒu���擾
	bool DeathFlag();			//���S��true��Ԃ�

	bool mHit;	//�U������true��Ԃ��@�G�ɍU����������or�U���I������false��Ԃ�
};

#endif
