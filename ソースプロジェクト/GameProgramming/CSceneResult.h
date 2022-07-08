#pragma once
#include "CScene.h"

//���U���g��ʂ̃V�[���N���X
class CSceneResult :public CScene {
private:
	int mNewRecord;	//�V�L�^�̏��ʂ��L�^

	int count; //�����_�ŗp�J�E���g

	//�t�F�[�h
	enum EFade {
		EFADE_STOP = 0,	//�t�F�[�h��~
		EFADE_IN,		//�t�F�[�h�C��
		EFADE_OUT		//�t�F�[�h�A�E�g
	};
	EFade mFade; //�t�F�[�h���f�p

	EScene mSceneTransitionKeep; //�V�[���̑J�ڐ��ۑ�����

	bool mIsButtonPush; //�{�^���������Ă����true
public:
	CSceneResult();	//�f�t�H���g�R���X�g���N�^

	void Init();	//����������
	void Update();	//�X�V����
	void Render();	//�`�揈��

	EScene GetNextScene(); //���̃V�[�����擾

	static float record[6];	//�N���A���Ԃ̋L�^�p
};