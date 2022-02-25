#pragma once
#include "CScene.h"

class CSceneResult :public CScene {
private:
	int mNewRecord;	//V‹L˜^‚Ì‡ˆÊ‚ğ‹L˜^

	CTexture mImageBackGround; //”wŒi‰æ‘œ

	int count;

public:
	CSceneResult();
	void Init();
	void Update();

	EScene GetNextScene();

	static float record[6];	//ƒNƒŠƒAŠÔ‚Ì‹L˜^—p
};