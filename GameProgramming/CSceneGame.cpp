#include "CSceneGame.h"
//
#include "CCamera.h"
//
#include "CUtil.h"
//
#include "CRes.h"

#include "Ckey.h"
//
#include "CMaterial.h"
//
#include "CCollisionManager.h"

//CMatrix Matrix;

int S;	//スタミナ確認用、後で削除
int EHp;	//敵の体力確認用、後で削除
int PHp;	//プレイヤーの体力確認用、後で削除

CSceneGame::~CSceneGame() {

}

void CSceneGame::Init() {
	//テキストフォントの読み込みと設定
	mFont.LoadTexture("FontG.png", 1, 4096 / 64);

	CRes::sModelX.Load(MODEL_FILE);
	CRes::sKnight.Load("knight\\knight_low.x");
	CRes::sKnight.SeparateAnimationSet(0, 10, 80, "walk");//1:移動
	CRes::sKnight.SeparateAnimationSet(0, 1530, 1830, "idle1");//2:待機
	CRes::sKnight.SeparateAnimationSet(0, 10, 80, "walk");//3:ダミー
	CRes::sKnight.SeparateAnimationSet(0, 10, 80, "walk");//4:ダミー
	CRes::sKnight.SeparateAnimationSet(0, 10, 80, "walk");//5:ダミー
	CRes::sKnight.SeparateAnimationSet(0, 10, 80, "walk");//6:ダミー
	CRes::sKnight.SeparateAnimationSet(0, 440, 520, "attack1");//7:Attack1
	CRes::sKnight.SeparateAnimationSet(0, 520, 615, "attack2");//8:Attack2
	CRes::sKnight.SeparateAnimationSet(0, 10, 80, "walk");//9:ダミー
	CRes::sKnight.SeparateAnimationSet(0, 10, 80, "walk");//10:ダミー
	CRes::sKnight.SeparateAnimationSet(0, 1160, 1260, "death1");//11:ダウン

	//キャラクターにモデルを設定
	mPlayer.Init(&CRes::sModelX);

	//敵の初期設定
	mEnemy.Init(&CRes::sKnight);
	mEnemy.mAnimationFrameSize = 1024;
	//敵の配置
	mEnemy.mPosition = CVector(7.0f, 0.0f, 0.0f);
	mEnemy.ChangeAnimation(2, true, 200);

	//カメラ初期化
	Camera.Init();
}


void CSceneGame::Update() {

	//更新
	CTaskManager::Get()->Update();

	//衝突処理
	CCollisionManager::Get()->Collision();

	Camera.Update();

	Camera.Render();

	//タスク描画
	CTaskManager::Get()->Render();

	//コライダの描画
	CCollisionManager::Get()->Render();

	//2D描画開始
	CUtil::Start2D(0, 800, 0, 600);

	//確認用、後で削除
	char buf[64];
	//スタミナ
	sprintf(buf, "STAMINA:%d", S);
	mFont.DrawString(buf, 50, 50, 10, 12);

	//敵の体力
	sprintf(buf, "EHP:%d", EHp);
	mFont.DrawString(buf, 50, 100, 10, 12);

	//プレイヤーの体力
	sprintf(buf, "PHP:%d", PHp);
	mFont.DrawString(buf, 50, 150, 10, 12);

	//2Dの描画終了
	CUtil::End2D();

	return;
}
	

