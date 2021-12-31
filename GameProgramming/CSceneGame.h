#ifndef CSCENEGAME_H
#define CSCENEGAME_H
#include "CScene.h"
#include "CXPlayer.h"
#include "CXEnemy.h"
#include "CMap.h"

/*
ゲームのシーン
*/
class CSceneGame : public CScene {
private:
	//マップのインスタンス
	CMap mMap;
	//キャラクタのインスタンス
	CXPlayer mPlayer;
	//敵のインスタンス
	CXEnemy mEnemy;

	int mTime;
	int mFrameCount;

public:
	~CSceneGame();
	//初期化処理のオーバーライド
	void Init();
	//更新処理のオーバーライド
	void Update();

	EScene GetNextScene();
};

#endif
