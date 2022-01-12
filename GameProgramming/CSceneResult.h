#pragma once
#include "CScene.h"

class CSceneResult :public CScene {
private:
	int mNewRecord;	//新記録の順位を記録

	CTexture mBackGroundTexture;

	int count;

public:
	CSceneResult();
	void Init();
	void Update();

	EScene GetNextScene();

	static float record[6];	//クリア時間の記録用
};