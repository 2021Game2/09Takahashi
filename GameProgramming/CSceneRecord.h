#pragma once
#include "CScene.h"

class CSceneRecord :public CScene {
private:
	CTexture mImageBackGround; //”wŒi‰æ‘œ

public:
	CSceneRecord();
	void Init();
	void Update();

	EScene GetNextScene();
};