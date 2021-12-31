#pragma once
#include "CScene.h"

class CSceneResult :public CScene {
public:
	void Init();
	void Update();

	EScene GetNextScene();
};