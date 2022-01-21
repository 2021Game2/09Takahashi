#pragma once
#include "CScene.h"

class CSceneTitle :public CScene {
private:
	enum ESelect {
		EBACKGROUND = 0,//背景
		EGAMESTART,		//ゲームスタート
		ERECORD,		//レコード
	};
	int mSelect;

	CTexture mTexture;

public:
	CSceneTitle();
	void Init();
	void Update();

	EScene GetNextScene();
};