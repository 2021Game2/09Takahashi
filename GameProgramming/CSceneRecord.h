#pragma once
#include "CScene.h"

//レコード画面のシーンクラス
class CSceneRecord :public CScene {
private:
	CTexture mImageBackGround; //背景画像

public:
	CSceneRecord();
	void Init();
	void Update();

	EScene GetNextScene();
};