#ifndef CSCENEGAME_H
#define CSCENEGAME_H

#include "CScene.h"
#include <time.h>
#include "CShadowMap.h"

//ゲームのシーンクラス
class CSceneGame : public CScene {
private:
	clock_t mStartTime, mEndTime; //計測開始時刻、計測終了時刻
	bool mCountStart; //クリア時間計測開始用

	//シャドウマップのインスタンス
	CShadowMap mShadowMap;

	//フェーズ
	enum EPhase {
		EPHASE_1 = 0,	//フェーズ1
		EPHASE_2,		//フェーズ2
		EPHASE_FINAL,	//最終フェーズ
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
	CSceneGame();	//デフォルトコンストラクタ
	~CSceneGame();	//デストラクタ
	//初期化処理のオーバーライド
	void Init();
	//更新処理のオーバーライド
	void Update();
	//描画処理のオーバーライド
	void Render();

	EScene GetNextScene(); //次のシーンを取得

	static float sClearTime;	//クリアまでにかかった時間
};

//描画用外部関数
void GlobalRender();

#endif
