#pragma once
#include "CScene.h"

//タイトル画面のシーンクラス
class CSceneTitle :public CScene {
private:
	enum ESelect {
		EBACKGROUND = 0,//背景
		EGAMESTART,		//ゲームスタート
		ERECORD,		//レコード
	};
	int mSelect;

public:
	CSceneTitle();
	void Init();
	void Update();

	EScene GetNextScene();
};