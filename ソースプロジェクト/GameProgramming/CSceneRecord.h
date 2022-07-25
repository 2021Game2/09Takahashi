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

	float mRecord[6];	//ファイルから読み取った記録を入れる
public:
	CSceneRecord();	//デフォルトコンストラクタ

	void Init();	//初期化処理
	void Update();	//更新処理
	void Render();	//描画処理

	EScene GetNextScene(); //次のシーンを取得
};