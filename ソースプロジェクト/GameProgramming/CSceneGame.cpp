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
#include "CXPlayer.h"
//
#include "CMap.h"

//敵の生成数
#define ENEMY_GENERATE_NUM_PHASE1 1 //フェーズ1
#define ENEMY_GENERATE_NUM_PHASE2 2 //フェーズ2
#define ENEMY_GENERATE_NUM_FINALPHASE 3 //最終フェーズ
//敵の体力
#define ENEMY_HP_PHASE1 200
#define ENEMY_HP_PHASE2 150
#define ENEMY_HP_FINALPHASE 150
//敵の初期位置
#define ENEMY_START_POS_PHASE1 CVector(0.0f,0.0f,-10.0f) //フェーズ1
#define ENEMY_START_POS_PHASE2 CVector(20.0f,0.0f,0.0f),CVector(-20.0f,0.0f,0.0f) //フェーズ2
#define ENEMY_START_POS_FINALPHASE CVector(0.0f,0.0f,20.0f),CVector(0.0f,0.0f,0.0f),CVector(0.0f,0.0f,-20.0f) //最終フェーズ

//画像系
#define EFFECT_ATTACK_HIT "Resource\\Effect_Attack_Hit.png"		//攻撃ヒット時のエフェクト画像
#define EFFECT_PORTION_USE "Resource\\Effect_Portion_Use.png"	//回復アイテム使用時のエフェクト画像
#define TEXWIDTH  8192	//テクスチャ幅
#define TEXHEIGHT  6144	//テクスチャ高さ

float CSceneGame::sClearTime = 0.0f; //クリアまでにかかった時間

CSceneGame::CSceneGame()
	:mStartTime(0)
	, mEndTime(0)
	, mCountStart(false)
	, mPhase(EPHASE_1)
	, mFade(EFADE_IN)
	, mSceneTransitionKeep(EGAME)
	,mShadowMap()
{
}

CSceneGame::~CSceneGame() {
	//タイトルに戻るとき
	if (mScene == ETITLE) {
		CXPlayer::Release();		//プレイヤー解放
		CMap::Release();			//マップ解放
		CMap2::Release();			//マップ2解放
		CEnemyManager::Release();	//敵管理解放
		CTrapManager::Release();	//罠管理解放
		ShowCursor(true);			//カーソル表示
		Camera.SetCameraMode(CCamera::TARGET_LOOK);	//カメラのモードを通常モードに設定
	}
	//次のステージへ進んだとき
	else {
		CEnemyManager::Release();	//敵管理解放
		CTrapManager::Release();	//罠管理解放
	}
}

void CSceneGame::Init() {
	mScene = EGAME; //シーンゲーム

	mCountStart = false;
	sClearTime = 0.0f;

	//プレイヤー生成
	CXPlayer::Generate();
	//プレイヤーの初期化
	CXPlayer::GetInstance()->Init(&CRes::sModelXPlayer);

	//敵管理生成
	CEnemyManager::Generate();
	//敵を生成する
	CVector pos[ENEMY_GENERATE_NUM_PHASE1] = { ENEMY_START_POS_PHASE1 }; //敵の初期位置
	CEnemyManager::GetInstance()->EnemyGenerate(ENEMY_GENERATE_NUM_PHASE1, CXEnemy::ETYPE_1, ENEMY_HP_PHASE1, pos);

	//マップ生成
	CMap::Generate();
	//マップ2生成
	CMap2::Generate();

	//トラップ管理生成
	CTrapManager::Generate();

	//カメラ初期化
	Camera.Init();
	//カメラモードを設定
	Camera.SetCameraMode(CCamera::TARGET_LOOK);

	ShowCursor(false); //カーソル非表示

	//影の設定
	float shadowColor[] = { 0.4f, 0.4f, 0.4f, 0.2f };	//影の色
	float lightPos[] = { 50.0f, 160.0f, 50.0f };		//光源の位置
	mShadowMap.Init(TEXWIDTH, TEXHEIGHT, GlobalRender, shadowColor, lightPos); //シャドウマップ初期化

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

	//BGM再生
	CRes::sBGMBattle.Repeat();
}

void CSceneGame::Update() {
	//更新
	CEnemyManager::GetInstance()->Update(); //敵管理更新
	CTrapManager::GetInstance()->Update();	//罠管理更新
	CTaskManager::Get()->Update(); //タスク更新

	//衝突処理
	CTaskManager::Get()->TaskCollision();

	//タスクリスト削除
	CTaskManager::Get()->Delete();

	//一度だけ通る
	if (mCountStart == false) {
		mCountStart = true;
		mStartTime = clock(); //計測開始時刻を入れる
		//カメラ初期化
		Camera.Init();
	}

	//現在のフェーズを判断
	switch (mPhase) {
	case EPHASE_1: //フェーズ1
		//敵が全て死亡状態になったとき
		if (CEnemyManager::GetInstance()->mIsEnemyAllDeath()) {
			CVector pos[ENEMY_GENERATE_NUM_PHASE2] = { ENEMY_START_POS_PHASE2 }; //敵の初期位置
			CEnemyManager::GetInstance()->EnemyGenerate(ENEMY_GENERATE_NUM_PHASE2, CXEnemy::ETYPE_1, ENEMY_HP_PHASE2,pos); //敵を生成
			mPhase = EPHASE_2;	//フェーズ2へ移行
		}
		break;

	case EPHASE_2: //フェーズ2
		//敵が全て死亡状態になったとき
		if (CEnemyManager::GetInstance()->mIsEnemyAllDeath()) {
			CVector pos[ENEMY_GENERATE_NUM_FINALPHASE] = { ENEMY_START_POS_FINALPHASE }; //敵の初期位置
			CEnemyManager::GetInstance()->EnemyGenerate(ENEMY_GENERATE_NUM_FINALPHASE, CXEnemy::ETYPE_1, ENEMY_HP_FINALPHASE,pos); //敵を生成
			mPhase = EPHASE_FINAL;	//最終フェーズへ移行
		}
		break;

	case EPHASE_FINAL: //最終フェーズ
		//敵が全て死亡状態になったとき
		if (CEnemyManager::GetInstance()->mIsEnemyAllDeath()) {
			//カメラのモードを通常モードに設定する
			Camera.SetCameraMode(CCamera::NORMAL);
			//Enterキーを押したとき
			if (CKey::Once(VK_RETURN)) {
				//クリア時間を記録
				sClearTime = (float)(mEndTime - mStartTime) / 1000;
				mSceneTransitionKeep = EGAME2; //シーンの遷移先を保存
				mFade = EFADE_OUT; //フェードアウト開始
			}
		}
		break;
	}

	//全ての敵が死亡状態でないとき
	if (CEnemyManager::GetInstance()->mIsEnemyAllDeath() == false) {
		mEndTime = clock();	//計測終了時間を入れる
	}
	
	//プレイヤーが死亡状態のとき
	if (CXPlayer::GetInstance()->GetState() == CXPlayer::EPlayerState::EDEATH) {
		//Enterキーを押したとき
		if (CKey::Once(VK_RETURN)) {
			mSceneTransitionKeep = ETITLE; //シーンの遷移先を保存
			mFade = EFADE_OUT; //フェードアウト開始
		}
	}

	//フェードを判断
	switch (mFade) {
	case EFADE_IN: //フェードイン
		if (CRes::sImageBlack.GetAlpha() > 0.0f) {
			//黒い画像のアルファ値を下げる
			CRes::sImageBlack.SetAlpha(-0.02f, true);
		}
		break;

	case EFADE_OUT: //フェードアウト
		if (CRes::sImageBlack.GetAlpha() < 1.0f) {
			//黒い画像のアルファ値を上げる
			CRes::sImageBlack.SetAlpha(0.02f, true);
		}
		else if (CRes::sImageBlack.GetAlpha() == 1.0f) {
			//保存された遷移先へシーンを移行する
			mScene = mSceneTransitionKeep;
			CRes::sBGMBattle.Stop(); //BGM停止
		}
		break;
	}

	//リセット
	CInput::InputReset();

	return;
}

void CSceneGame::Render() {
	//カメラ描画
	Camera.Draw();

	//シャドウマップ描画
	mShadowMap.Render();

	//タスク2D描画
	CTaskManager::Get()->Render2D();

#ifdef _DEBUG
	//コライダの描画
	//CCollisionManager::Get()->Render();
#endif

	//2D描画開始
	CUtil::Start2D(0, 800, 0, 600);

	//操作方法の画像表示
	CRes::sImagePlayerRun.Draw(110, 190, 20, 100, 0, 255, 255, 0);		//プレイヤーの走り方
	CRes::sImagePlayerAttack.Draw(20, 100, 20, 100, 0, 255, 255, 0);	//プレイヤーの攻撃方法
	CRes::sImagePlayerAvoid.Draw(200, 280, 20, 100, 0, 255, 255, 0);	//プレイヤーの回避方法
	CRes::sImageMouse.Draw(590, 630, 70, 110, 0, 255, 255, 0);			//右クリック用
	CRes::sImageMouse.Draw(750, 790, 70, 110, 0, 255, 511, 256);		//ホイール用
	//一番近い敵の方向へカメラを向かせる方法
	//カメラモードを判断、ターゲット状態の敵の方へ向くモードだった時
	if (Camera.GetCameraMode() == CCamera::TARGET_LOOK) {
		CRes::sImageTargetLook.Draw(280, 360, 20, 100, 0, 255, 255, 0);	//ON状態用
	}
	else {
		CRes::sImageTargetLook.Draw(280, 360, 20, 100, 256, 511, 255, 0); //OFF状態用
	}

	//プレイヤーが死亡状態になるとGAMEOVERと表示する
	if (CXPlayer::GetInstance()->GetState() == CXPlayer::EPlayerState::EDEATH) {
		//ゲームオーバー時の画像を表示
		CRes::sImageGameOverText.Draw(0, 800, 0, 600, 0, 800, 600, 0);
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
			//ステージクリア時の画像を表示
			CRes::sImageStageClearText.Draw(0, 800, 0, 600, 0, 800, 600, 0);
		}
		break;
	}

	//黒い画像を表示
	CRes::sImageBlack.Draw(0, 800, 0, 600, 1, 1, 1, 1);

	//2Dの描画終了
	CUtil::End2D();
}

void GlobalRender()
{
	//タスク描画
	CTaskManager::Get()->Render();
}

CScene::EScene CSceneGame::GetNextScene()
{
	return mScene;
}
	

