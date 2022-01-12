#pragma once
#include "CScene.h"

class CSceneTitle :public CScene {
private:
	enum EMenu {
		HEAD = 0,
		EGAMESTART,	//ゲームスタート
		ERECORD,	//レコード
		EMANUAL,	//マニュアル
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