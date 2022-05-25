#pragma once
#include "CScene.h"

//リザルト画面のシーンクラス
class CSceneResult :public CScene {
private:
	int mNewRecord;	//新記録の順位を記録

	int count; //文字点滅用カウント
public:
	CSceneResult();

	void Init();
	void Update();

	EScene GetNextScene(); //次のシーンを取得

	static float record[6];	//クリア時間の記録用
};