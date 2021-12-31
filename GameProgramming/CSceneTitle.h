#pragma once
#include "CScene.h"

class CSceneTitle :public CScene {
public:
	void Init();
	void Update();

	EScene GetNextScene();
};