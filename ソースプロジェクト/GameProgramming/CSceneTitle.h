#pragma once
#include "CScene.h"

//タイトル画面のシーンクラス
class CSceneTitle :public CScene {
private:
	//選択している場所
	enum ESelect {
		EBACKGROUND = 0,//背景
		EGAMESTART,		//ゲームスタート
		ERECORD,		//レコード
	};
	ESelect mSelect; //選択している場所の判断用
	ESelect mOldSelect; //1フレーム前に選択していた場所

	//フェード
	enum EFade {
		EFADE_STOP = 0,	//フェード停止
		EFADE_IN,		//フェードイン
		EFADE_OUT		//フェードアウト
	};
	EFade mFade; //フェード判断用

	EScene mSceneTransitionKeep; //シーンの遷移先を保存する

	bool mIsButtonPush; //ボタンを押すとtrue

	float mButtonBackAlpha1;	//ボタン背景画像のアルファ値変更に使用する、ゲームスタートボタン用
	float mButtonBackAlpha2;	//ボタン背景画像のアルファ値変更に使用する、レコードボタン用
public:
	CSceneTitle();	//デフォルトコンストラクタ

	void Init();	//初期化処理
	void Update();	//更新処理
	void Render();	//描画処理

	EScene GetNextScene(); //次のシーンを取得
};