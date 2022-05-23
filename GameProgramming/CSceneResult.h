#pragma once
#include "CScene.h"

//リザルト画面のシーンクラス
class CSceneResult :public CScene {
private:
	int mNewRecord;	//新記録の順位を記録

	CTexture mImageBackGround; //背景画像

	int count;

public:
	CSceneResult();
	void Init();
	void Update();

	EScene GetNextScene();

	static float record[6];	//クリア時間の記録用
};