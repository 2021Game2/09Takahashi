#ifndef CCAMERA_H
#define CCAMERA_H

#include "CVector.h"

#include "CCharacter.h"
#include "CColliderLine.h"

#define DEF_CAMERA_DIST 7.0f	//�J�����̏�������
#define CAMERA_HEAD_ADJUST 3.0f	//�����_�̍�������
/*
�J�����N���X
*/
class CCamera :public CCharacter {
public:
	//�J�����̃��[�h
	enum ECameraMode {
		NORMAL = 0,	//�ʏ탂�[�h
		TARGET_LOOK	//�^�[�Q�b�g��Ԃ̓G�̕��֌������[�h
	};
private:
	CColliderLine mColliderLine; //���_���璍���_�܂ŐL�т���R���C�_

	bool mIsRot;		//��ԋ߂��G�̕����։�]����t���O
	float mRotRad;		//��]���������p�x

	//�A���O���x���p�A�ʏ탂�[�h���Ɏg�p
	float mAngleDelayX;
	float mAngleDelayY;

	ECameraMode mCameraMode;	//�J�����̃��[�h����p
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

	//���`���
	float mLerp(float start, float point, float rate);

	//�J�������[�h��ݒ肷��
	void SetCameraMode(ECameraMode cameramode);
	//�J�������[�h���擾����
	CCamera::ECameraMode GetCameraMode();
};

//�J�����̊O���Q��
extern CCamera Camera;
#endif
