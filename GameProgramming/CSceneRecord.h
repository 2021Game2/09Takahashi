#pragma once
#include "CScene.h"

//レコード画面のシーンクラス
class CSceneRecord :public CScene {
private:
	//フェード
	enum EFade {
		EFADE_STOP = 0,	//フェード停止
		EFADE_IN,		//フェードイン
		EFADE_OUT		//フェードアウト
	};
	EFade mFade; //フェード判断用

	EScene mSceneTransitionKeep; //シーンの遷移先を保存する

	bool mIsButtonPush; //ボタンを押すとtrue
public:
	CSceneRecord();

	void Init();
	void Update();

	EScene GetNextScene(); //次のシーンを取得
};