#pragma once
#include "CScene.h"

class CSceneRecord :public CScene {
private:
	CTexture mBackGroundTexture;

public:
	CSceneRecord();
	void Init();
	void Update();

	EScene GetNextScene();
};