#pragma once
#include "CScene.h"

//タイトル画面のシーンクラス
class CSceneTitle :public CScene {
private:
	//選択している場所
	enum ESelect {
		EBACKGROUND = 0,//背景
		EGAMESTART,		//ゲームスタート
		ERECORD,		//レコード
	};
	ESelect mSelect; //選択している場所の判断用

public:
	CSceneTitle();

	void Init();
	void Update();

	EScene GetNextScene(); //次のシーンを取得
};