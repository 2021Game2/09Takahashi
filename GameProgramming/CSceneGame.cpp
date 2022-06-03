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
#include "CMap2.h"
//
#include "CTrapManager.h"
//
#include "CEffect.h"
//
#include "CEffect2.h"
//
#include "CInput.h"
//
#include "CEnemyManager.h"
//

//画像系
#define EFFECT_ATTACK_HIT "Resource\\Effect_Attack_Hit.png"		//攻撃ヒット時のエフェクト画像
#define EFFECT_PORTION_USE "Resource\\Effect_Portion_Use.png"	//回復アイテム使用時のエフェクト画像
#define TEXWIDTH  8192	//テクスチャ幅
#define TEXHEIGHT  6144	//テクスチャ高さ

float CSceneGame::mClearTime = 0.0f; //クリアまでにかかった時間

CSceneGame::CSceneGame()
	:mStartTime(0)
	, mEndTime(0)
	, mCountStart(false)
	, mPhase(EPHASE_1)
{
}

CSceneGame::~CSceneGame() {
	CXPlayer::Release();		//プレイヤー解放
	CMap::Release();			//マップ解放
	CMap2::Release();			//マップ2解放
	CEnemyManager::Release();	//敵管理解放
	CTrapManager::Release();	//罠管理解放
}

void CSceneGame::Init() {
	mScene = EGAME; //シーンゲーム

	mCountStart = false;
	mClearTime = 0.0f;

	//プレイヤー生成
	CXPlayer::Generate();
	//プレイヤーの初期化
	CXPlayer::GetInstance()->Init(&CRes::sModelXPlayer);

	//敵管理生成
	CEnemyManager::Generate();
	//敵を生成する
	CEnemyManager::GetInstance()->EnemyGenerate(ENEMY_GENERATE_NUM, 1);

	//マップ生成
	CMap::Generate();
	//マップ2生成
	CMap2::Generate();

	//トラップ管理生成
	CTrapManager::Generate();

	//カメラ初期化
	Camera.Init();

	ShowCursor(false); //カーソル非表示

	//影の設定
	float shadowColor[] = { 0.4f, 0.4f, 0.4f, 0.2f };	//影の色
	float lightPos[] = { 50.0f, 160.0f, 50.0f };		//光源の位置
	mShadowMap.Init(TEXWIDTH, TEXHEIGHT, Render, shadowColor, lightPos); //シャドウマップ初期化

	//エフェクト画像読み込み
	if (CEffect::sMaterial.mTexture.mId == 0) {
		CEffect::sMaterial.mTexture.Load(EFFECT_ATTACK_HIT); //攻撃ヒット時のエフェクト
		CEffect::sMaterial.mDiffuse[0] = CEffect::sMaterial.mDiffuse[1] =
			CEffect::sMaterial.mDiffuse[2] = CEffect::sMaterial.mDiffuse[3] = 1.0f;
	}
	if (CEffect2::sMaterial.mTexture.mId == 0) {
		CEffect2::sMaterial.mTexture.Load(EFFECT_PORTION_USE); //回復アイテム使用時のエフェクト
		CEffect2::sMaterial.mDiffuse[0] = CEffect2::sMaterial.mDiffuse[1] =
			CEffect2::sMaterial.mDiffuse[2] = CEffect2::sMaterial.mDiffuse[3] = 1.0f;
	}
}

void CSceneGame::Update() {
	//更新
	CEnemyManager::GetInstance()->Update(); //敵管理更新
	CTaskManager::Get()->Update(); //タスク更新

	//衝突処理
	CTaskManager::Get()->TaskCollision();

	//タスクリスト削除
	CTaskManager::Get()->Delete();

	//カメラ描画
	Camera.Draw();

	//シャドウマップ描画
	mShadowMap.Render();

	//タスク2D描画
	CTaskManager::Get()->Render2D();

#ifdef _DEBUG
	//コライダの描画
	CCollisionManager::Get()->Render();
#endif

	//一度だけ通る
	if (mCountStart == false) {
		mStartTime = clock(); //計測開始時刻を入れる
		//カメラ初期化
		Camera.Init();
		mCountStart = true;
	}

	//現在のフェーズを判断
	switch (mPhase) {
	case EPHASE_1: //フェーズ1
		//敵が全て死亡状態になったとき
		if (CEnemyManager::GetInstance()->mIsEnemyAllDeath()) {
			CEnemyManager::GetInstance()->EnemyGenerate(ENEMY_GENERATE_NUM,0); //敵を生成
			mPhase = EPHASE_2;	//フェーズ2へ移行
		}
		break;

	case EPHASE_2: //フェーズ2
		//敵が全て死亡状態になったとき
		if (CEnemyManager::GetInstance()->mIsEnemyAllDeath()) {
			CEnemyManager::GetInstance()->EnemyGenerate(ENEMY_GENERATE_NUM,0); //敵を生成
			mPhase = EPHASE_FINAL;	//最終フェーズへ移行
		}
		break;

	case EPHASE_FINAL: //最終フェーズ
		//敵が全て死亡状態になったとき
		if (CEnemyManager::GetInstance()->mIsEnemyAllDeath()) {
			//Enterキーを押すとリザルトに移行する
			if (CKey::Once(VK_RETURN)) {
				//クリア時間を記録
				mClearTime = (float)(mEndTime - mStartTime) / 1000;
				mScene = ERESULT;
				ShowCursor(true); //カーソル表示
			}
		}
		break;
	}

	//全ての敵が死亡状態でないとき
	if (CEnemyManager::GetInstance()->mIsEnemyAllDeath() == false) {
		mEndTime = clock();	//計測終了時間を入れる
	}
	
	//プレイヤーが死亡状態のとき、Enterキーでタイトルに移行する
	if (CXPlayer::GetInstance()->mState == CXPlayer::EPlayerState::EDEATH) {
		if (CKey::Once(VK_RETURN)) {
			mScene = ETITLE;
			ShowCursor(true); //カーソル表示
		}
	}

	//2D描画開始
	CUtil::Start2D(0, 800, 0, 600);

	//操作方法の画像表示
	CRes::sImagePlayerRun.Draw(110, 190, 20, 100, 0, 255, 255, 0);		//プレイヤーの走り方
	CRes::sImagePlayerAttack.Draw(20, 100, 20, 100, 0, 255, 255, 0);	//プレイヤーの攻撃方法
	CRes::sImagePlayerAvoid.Draw(200, 280, 20, 100, 0, 255, 255, 0);	//プレイヤーの回避方法
	CRes::sImageMouse.Draw(590, 630, 70, 110, 0, 255, 255, 0);			//右クリック用
	CRes::sImageMouse.Draw(750, 790, 70, 110, 0, 255, 511, 256);		//ホイール用

	//プレイヤーが死亡状態になるとGAMEOVERと表示する
	if (CXPlayer::GetInstance()->mState == CXPlayer::EPlayerState::EDEATH) {
		CRes::sFont.DrawString("GAMEOVER", 120, 350, 40, 40);
		CRes::sFont.DrawString("PUSH ENTER", 125, 270, 30, 30);
	}

	//現在のフェーズを判断
	switch (mPhase) {
	case EPHASE_1: //フェーズ1
		CRes::sFont.DrawString("PHASE1", 660, 550, 10, 10);
		break;

	case EPHASE_2: //フェーズ2
		CRes::sFont.DrawString("PHASE2", 660, 550, 10, 10);
		break;

	case EPHASE_FINAL: //最終フェーズ
		CRes::sFont.DrawString("FINALPHASE", 580, 550, 10, 10);
		//敵が全て死亡状態のときCLEARとPUSHENTERを表示する
		if (CEnemyManager::GetInstance()->mIsEnemyAllDeath()) {
			CRes::sFont.DrawString("CLEAR", 230, 350, 40, 40);
			CRes::sFont.DrawString("PUSH ENTER", 125, 270, 30, 30);
		}
		break;
	}

	//2Dの描画終了
	CUtil::End2D();

	//リセット
	CInput::InputReset();

	return;
}

void Render()
{
	//タスク描画
	CTaskManager::Get()->Render();
}

CScene::EScene CSceneGame::GetNextScene()
{
	return mScene;
}
	

