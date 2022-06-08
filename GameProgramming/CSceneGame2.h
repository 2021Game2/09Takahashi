#pragma once

#include "CScene.h"
#include <time.h>
#include "CShadowMap.h"

//ゲームのシーンクラス
class CSceneGame2 : public CScene {
private:
	clock_t mStartTime, mEndTime; //計測開始時刻、計測終了時刻
	bool mCountStart; //クリア時間計測開始用

	//シャドウマップのインスタンス
	CShadowMap mShadowMap;

	//フェーズ
	enum EPhase {
		EPHASE_1 = 0,
		EPHASE_2,
		EPHASE_FINAL,
	};
	EPhase mPhase; //フェーズ判断用

	//フェード
	enum EFade {
		EFADE_IN = 0,	//フェードイン
		EFADE_OUT		//フェードアウト
	};
	EFade mFade; //フェード判断用

	EScene mSceneTransitionKeep; //シーンの遷移先を保存する
public:
	CSceneGame2();
	~CSceneGame2();
	//初期化処理のオーバーライド
	void Init();
	//更新処理のオーバーライド
	void Update();

	EScene GetNextScene(); //次のシーンを取得
};

