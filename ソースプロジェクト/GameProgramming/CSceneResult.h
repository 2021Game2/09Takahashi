#pragma once
#include "CScene.h"

//リザルト画面のシーンクラス
class CSceneResult :public CScene {
private:
	int mNewRecord;	//新記録の順位を記録

	int count; //文字点滅用カウント

	//フェード
	enum EFade {
		EFADE_STOP = 0,	//フェード停止
		EFADE_IN,		//フェードイン
		EFADE_OUT		//フェードアウト
	};
	EFade mFade; //フェード判断用

	EScene mSceneTransitionKeep; //シーンの遷移先を保存する

	bool mIsButtonPush; //ボタンを押しているとtrue
public:
	CSceneResult();	//デフォルトコンストラクタ

	void Init();	//初期化処理
	void Update();	//更新処理
	void Render();	//描画処理

	EScene GetNextScene(); //次のシーンを取得

	static float record[6];	//クリア時間の記録用
};