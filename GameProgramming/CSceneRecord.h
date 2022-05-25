#pragma once
#include "CScene.h"

//レコード画面のシーンクラス
class CSceneRecord :public CScene {
public:
	CSceneRecord();

	void Init();
	void Update();

	EScene GetNextScene(); //次のシーンを取得
};