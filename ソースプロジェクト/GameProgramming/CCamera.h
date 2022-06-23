#ifndef CCAMERA_H
#define CCAMERA_H

#include "CVector.h"

#include "CCharacter.h"
#include "CColliderLine.h"

#define DEF_CAMERA_DIST 7.0f	//�J�����̏�������
#define CAMERA_HEAD_ADJUST 3.0f	//�����_�̍�������
#define CAMERA_LOCK_HEIGHT 4.0f	//�J�����̃��b�N���̍���
/*
�J�����N���X
*/
class CCamera :public CCharacter {
private:
	CColliderLine mColliderLine; //���_���璍���_�܂ŐL�т���R���C�_

	//�J������]����
	enum ERotDir {
		LEFT = 0,	//��
		RIGHT		//�E
	};
	ERotDir mRotDir; //�J�����̉�]������������f�p

	bool mIsRot;		//��ԋ߂��G�̕����։�]����t���O
	float mRotRad;		//��]���������p�x
	float mRotedRad;	//��]�����p�x

	float mLerp(float start, float point, float rate); //���`���

public:
	CCamera();
	//���_
	CVector mEye;
	//�����_
	CVector mCenter;
	//�����
	CVector mUp;

	CMatrix mMatrix;

	//�d�����邪�J��������p
	CVector mPos;		//�ʒu
	CVector mTarget;	//�^�[�Q�b�g
	float	mAngleX;	//�A���O��
	float	mAngleY;	//�A���O��
	float	mDist;	//����

	//�J�����̐ݒ�
	//Set(���_, �����_, �����)
	void Set(const CVector& eye, const CVector& center,
		const CVector& up);

	void SetTarget(const CVector& target);

	//�J����������
	void Init();
	//�J�����X�V
	void Update();
	//�J�����K�p
	void Draw();

	//���������Ȃ�Render
	void Render();

	//�x�N�g���擾
	CMatrix GetMat();

	//�X�N���[�����W�ϊ�
	//�߂�l �\���͈�
	//pOut ���ʊi�[�p
	//pos 2D�ɕϊ����������[���h���W
	bool WorldToScreen(CVector* pOut, const CVector& pos);
	
	//�Փ˔���
	void CCamera::Collision(CCollider* m, CCollider* o);
	void CCamera::TaskCollision();

	//�^�[�Q�b�g�ɂȂ��Ă���G�̕����փJ�������������鏈��
	void mTargetLook();
};

//�J�����̊O���Q��
extern CCamera Camera;
#endif
