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

	CTexture mImageButtonBack; //�{�^���w�i�摜

public:
	CSceneTitle();
	void Init();
	void Update();

	EScene GetNextScene();
};