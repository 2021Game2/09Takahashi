#pragma once
#include "CScene.h"

//�^�C�g����ʂ̃V�[���N���X
class CSceneTitle :public CScene {
private:
	//�I�����Ă���ꏊ
	enum ESelect {
		EBACKGROUND = 0,//�w�i
		EGAMESTART,		//�Q�[���X�^�[�g
		ERECORD,		//���R�[�h
	};
	ESelect mSelect; //�I�����Ă���ꏊ�̔��f�p

	//�t�F�[�h
	enum EFade {
		EFADE_STOP = 0,	//�t�F�[�h��~
		EFADE_IN,		//�t�F�[�h�C��
		EFADE_OUT		//�t�F�[�h�A�E�g
	};
	EFade mFade; //�t�F�[�h���f�p

	EScene mSceneTransitionKeep; //�V�[���̑J�ڐ��ۑ�����

	bool mIsButtonPush; //�{�^����������true

public:
	CSceneTitle();	//�f�t�H���g�R���X�g���N�^

	void Init();	//����������
	void Update();	//�X�V����
	void Render();	//�`�揈��

	EScene GetNextScene(); //���̃V�[�����擾
};