#pragma once
#include "CScene.h"

class CSceneTitle :public CScene {
private:
	enum EMenu {
		HEAD = 0,
		EGAMESTART,	//�Q�[���X�^�[�g
		ERECORD,	//���R�[�h
		EMANUAL,	//�}�j���A��
		TAIL,
	};
	int mSelectMenu;

	void MenuSelect();
public:
	CSceneTitle();
	void Init();
	void Update();

	EScene GetNextScene();
};