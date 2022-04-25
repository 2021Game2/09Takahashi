#ifndef CSCENEGAME_H
#define CSCENEGAME_H
#include "CScene.h"
#include "CXPlayer.h"
#include "CMap.h"
#include <time.h>
#include "CShadowMap.h"

/*
ゲームのシーン
*/
class CSceneGame : public CScene {
private:
	//マップのインスタンス
	CMap mMap;

	CModel mMap2;

	//キャラクタのインスタンス
	CXPlayer mPlayer;

	clock_t mStartTime, mEndTime; //計測開始時刻、計測終了時刻
	bool mCountStart; //クリア時間計測開始用

	//シャドウマップのインスタンス
	CShadowMap mShadowMap;

	CTexture mImagePlayerRun;		//プレイヤーの走り方説明用画像
	CTexture mImagePlayerAttack;	//プレイヤーの攻撃方法説明用画像
	CTexture mImagePlayerAvoid;		//プレイヤーの回避方法説明用画像
	CTexture mImageMouse;			//マウス操作説明用画像

public:
	CSceneGame();
	~CSceneGame();
	//初期化処理のオーバーライド
	void Init();
	//更新処理のオーバーライド
	void Update();

	EScene GetNextScene();

	static float mClearTime;	//クリアまでにかかった時間
};

//外部関数
void Render();

#endif
