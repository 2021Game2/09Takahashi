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

public:
	CSceneTitle();

	void Init();
	void Update();

	EScene GetNextScene(); //���̃V�[�����擾
};