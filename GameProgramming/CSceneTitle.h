#pragma once
#include "CScene.h"

//�^�C�g����ʂ̃V�[���N���X
class CSceneTitle :public CScene {
private:
	enum ESelect {
		EBACKGROUND = 0,//�w�i
		EGAMESTART,		//�Q�[���X�^�[�g
		ERECORD,		//���R�[�h
	};
	int mSelect;

public:
	CSceneTitle();
	void Init();
	void Update();

	EScene GetNextScene();
};