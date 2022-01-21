#pragma once
#include "CScene.h"

class CSceneTitle :public CScene {
private:
	enum ESelect {
		EBACKGROUND = 0,//�w�i
		EGAMESTART,		//�Q�[���X�^�[�g
		ERECORD,		//���R�[�h
	};
	int mSelect;

	CTexture mTexture;

public:
	CSceneTitle();
	void Init();
	void Update();

	EScene GetNextScene();
};