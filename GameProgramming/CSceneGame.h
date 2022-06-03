#ifndef CSCENEGAME_H
#define CSCENEGAME_H
#include "CScene.h"
#include "CXPlayer.h"
#include "CMap.h"
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
		EPHASE_1 = 0,
		EPHASE_2,
		EPHASE_FINAL,
	};
	EPhase mPhase; //フェーズ判断用

public:
	CSceneGame();
	~CSceneGame();
	//初期化処理のオーバーライド
	void Init();
	//更新処理のオーバーライド
	void Update();

	EScene GetNextScene(); //次のシーンを取得

	static float mClearTime;	//クリアまでにかかった時間
};

//外部関数
void Render();

#endif
