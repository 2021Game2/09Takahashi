#ifndef CSCENEGAME_H
#define CSCENEGAME_H
#include "CScene.h"
#include "CXPlayer.h"
#include "CXEnemy.h"
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
	//敵のインスタンス
	CXEnemy mEnemy;

	clock_t start, end;
	bool mCountStart; //クリア時間計測開始用

	//シャドウマップのインスタンス
	CShadowMap mShadowMap;

public:
	~CSceneGame();
	//初期化処理のオーバーライド
	void Init();
	//更新処理のオーバーライド
	void Update();

	EScene GetNextScene();

	static float mClearTime;	//クリアまでにかかった時間
};

void Render();

#endif
