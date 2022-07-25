#pragma once
#include "CScene.h"

//���R�[�h��ʂ̃V�[���N���X
class CSceneRecord :public CScene {
private:
	//�t�F�[�h
	enum EFade {
		EFADE_STOP = 0,	//�t�F�[�h��~
		EFADE_IN,		//�t�F�[�h�C��
		EFADE_OUT		//�t�F�[�h�A�E�g
	};
	EFade mFade; //�t�F�[�h���f�p

	EScene mSceneTransitionKeep; //�V�[���̑J�ڐ��ۑ�����

	bool mIsButtonPush; //�{�^����������true

	float mRecord[6];	//�t�@�C������ǂݎ�����L�^������
public:
	CSceneRecord();	//�f�t�H���g�R���X�g���N�^

	void Init();	//����������
	void Update();	//�X�V����
	void Render();	//�`�揈��

	EScene GetNextScene(); //���̃V�[�����擾
};