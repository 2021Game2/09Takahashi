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
//
//CMatrix Matrix;

float CSceneGame::mClearTime = 0.0f; //クリアまでにかかった時間

CSceneGame::~CSceneGame() {

}

void CSceneGame::Init() {
	mScene = EGAME;

	mCountStart = false;
	mClearTime = 0.0f;

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
	CRes::sKnight.SeparateAnimationSet(0, 440, 520, "attack1_1");//7:Attack1
	CRes::sKnight.SeparateAnimationSet(0, 520, 615, "attack2");//8:Attack2
	CRes::sKnight.SeparateAnimationSet(0, 10, 80, "walk");//9:ダミー
	CRes::sKnight.SeparateAnimationSet(0, 10, 80, "walk");//10:ダミー
	CRes::sKnight.SeparateAnimationSet(0, 1160, 1260, "death1");//11:ダウン
	CRes::sKnight.SeparateAnimationSet(0, 90, 160, "knockback");//12:ノックバック
	CRes::sKnight.SeparateAnimationSet(0, 1120, 1160, "stun");//13:スタン
	CRes::sKnight.SeparateAnimationSet(0, 170, 220, "Dash");//14:ダッシュ

	CRes::sKnight.SeparateAnimationSet(0, 380, 430, "Jump");//15:ジャンプ

	//キャラクターにモデルを設定
	mPlayer.Init(&CRes::sModelX);

	//敵の初期設定
	mEnemy.Init(&CRes::sKnight);
	mEnemy.mAnimationFrameSize = 1024;
	//敵の配置
	mEnemy.mPosition = CVector(20.0f, 0.0f, 0.0f);
	//mEnemy.ChangeAnimation(2, true, 200);

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

	//タスクリスト削除
	CTaskManager::Get()->Delete();
	//タスク描画
	CTaskManager::Get()->Render();
	CTaskManager::Get()->Render2D();

#ifdef _DEBUG
	//コライダの描画
	CCollisionManager::Get()->Render();
#endif

	//時間計測開始
	if (mCountStart == false) {
		start = clock();
		mCountStart = true;
	}

	//敵が死亡状態になると時間計測終了
	if (CXEnemy::GetInstance()->DeathFlag() != true) {
		end = clock();
	}
	else {
		//クリア時間を記録
		mClearTime = (float)(end - start) / 1000;
		//Enterキーを押すとリザルト画面に移行する
		if (CKey::Once(VK_RETURN)) {
			mScene = ERESULT;
		}
	}

	//2D描画開始
	CUtil::Start2D(0, 800, 0, 600);

#ifdef _DEBUG
	//確認用、後で削除
	char buf[64];
	//タイム
	sprintf(buf, "TIME:%06.2f", (float)(end - start) / 1000);
	mFont.DrawString(buf, 50, 100, 10, 12);

	if (CXEnemy::GetInstance()->DeathFlag() == true) {
		mFont.DrawString("PUSH ENTER", 50, 50, 20, 20);
	}
#endif

	//2Dの描画終了
	CUtil::End2D();

	return;
}


CScene::EScene CSceneGame::GetNextScene()
{
	return mScene;
}
	

