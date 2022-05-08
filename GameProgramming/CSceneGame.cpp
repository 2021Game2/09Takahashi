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
#include "CTrap.h"
//
#include "CTrapManager.h"
//
#include "CEffect.h"
//
#include "CEffect2.h"
//
#include "CSound.h"
//
#include "CInput.h"
//
#include "CEnemyManager.h"
//

#define ENEMY_GENERATE_NUM 3 //敵の生成数

//画像系
#define FONT "Resource\\FontG.png" //フォント
#define EFFECT_ATTACK_HIT "Resource\\Effect_Attack_Hit.png"		//攻撃ヒット時のエフェクト画像
#define EFFECT_PORTION_USE "Resource\\Effect_Portion_Use.png"	//回復アイテム使用時のエフェクト画像
#define IMAGE_PLAYER_RUN "Resource\\Image_Player_Run.png"		//プレイヤーの走り方説明用画像
#define IMAGE_PLAYER_ATTACK "Resource\\Image_Player_Attack.png" //プレイヤーの攻撃方法説明用画像
#define IMAGE_PLAYER_AVOID "Resource\\Image_Player_Avoid.png"	//プレイヤーの回避方法説明用画像
#define IMAGE_MOUSE "Resource\\Image_Mouse.png"					//マウス操作説明用画像
#define TEXWIDTH  8192	//テクスチャ幅
#define TEXHEIGHT  6144	//テクスチャ高さ

//モデル系
#define MODEL_ENEMY "Resource\\knight\\knight_low.x" //敵モデル
#define MODEL_MAP "Resource\\Colosseum.obj", "Resource\\Colosseum.mtl" //マップモデル

//サウンド系
#define SE_PLAYER_WALK "Resource\\SE_Player_Walk.wav"	//プレイヤーの歩行時の効果音
#define SE_PLAYER_RUN "Resource\\SE_Player_Run.wav"		//プレイヤーの走行時の効果音
#define SE_PLAYER_AVOID "Resource\\SE_Player_Avoid.wav"	//プレイヤーの回避時の効果音
#define SE_KNIGHT_WALK "Resource\\SE_Knight_Walk.wav"	//敵(ナイト)の歩行時の効果音
#define SE_KNIGHT_RUN "Resource\\SE_Knight_Run.wav"		//敵(ナイト)の走行時の効果音
#define SE_ATTACK_HIT_1 "Resource\\SE_Attack_Hit_1.wav"	//攻撃ヒット時の効果音1
#define SE_ATTACK_HIT_2 "Resource\\SE_Attack_Hit_2.wav"	//攻撃ヒット時の効果音2
#define SE_PORTION_USE "Resource\\SE_Portion_Use.wav"	//回復アイテム使用時の効果音
#define SE_TRAP_USE "Resource\\SE_Trap_Use.wav"			//罠アイテム使用時の効果音
#define SE_TRAP_ACTIVE "Resource\\SE_Trap_Active.wav"	//罠アイテム作動時の効果音

CSound SE_Player_Walk;	//プレイヤーの歩行時の効果音
CSound SE_Player_Run;	//プレイヤーの走行時の効果音
CSound SE_Player_Avoid;	//プレイヤーの回避時の効果音
CSound SE_Knight_Walk;	//敵(ナイト)の歩行時の効果音
CSound SE_Knight_Run;	//敵(ナイト)の走行時の効果音
CSound SE_Attack_Hit_1;	//攻撃ヒット時の効果音1
CSound SE_Attack_Hit_2;	//攻撃ヒット時の効果音2
CSound SE_Portion_Use;	//回復アイテム使用時の効果音
CSound SE_Trap_Use;		//罠アイテム使用時の効果音
CSound SE_Trap_Active;	//罠アイテム作動時の効果音

float CSceneGame::mClearTime = 0.0f; //クリアまでにかかった時間

CSceneGame::CSceneGame()
	:mStartTime(0)
	, mEndTime(0)
	, mCountStart(false)
{
}

CSceneGame::~CSceneGame() {
	CTrapManager::Release();
	CMap2::Release();
	CEnemyManager::Release();
}

void CSceneGame::Init() {
	mScene = EGAME;

	mCountStart = false;
	mClearTime = 0.0f;

	//テキストフォントの読み込みと設定
	mFont.LoadTexture(FONT, 1, 4096 / 64);

	//画像読み込み
	mImagePlayerRun.Load(IMAGE_PLAYER_RUN);			//プレイヤーの走り方説明用画像
	mImagePlayerAttack.Load(IMAGE_PLAYER_ATTACK);	//プレイヤーの攻撃方法説明用画像
	mImagePlayerAvoid.Load(IMAGE_PLAYER_AVOID);		//プレイヤーの回避方法説明用画像
	mImageMouse.Load(IMAGE_MOUSE);					//マウス操作説明用画像

	CRes::sModelX.Load(MODEL_FILE);
	
	//キャラクターにモデルを設定
	mPlayer.Init(&CRes::sModelX);
	mPlayer.mPosition = CVector(0.0f, 0.0f, 20.0f);
	mPlayer.mRotation = CVector(0.0f, 180.0f, 0.0f);

	//敵管理生成
	CEnemyManager::Generate();
	CEnemyManager::GetInstance()->EnemyGenerate(ENEMY_GENERATE_NUM); //敵を生成

	//カメラ初期化
	Camera.Init();

	//マップモデルの読み込み、生成
	mMap2.Load(MODEL_MAP);
	new CMap2(&mMap2, CVector(0.0f, -5.0f, 0.0f),
		CVector(), CVector(4.0f, 3.0f, 4.0f));

	CTrapManager::Generate();

	ShowCursor(false); //カーソル非表示

	//影の設定
	float shadowColor[] = { 0.4f, 0.4f, 0.4f, 0.2f };  //影の色
	float lightPos[] = { 50.0f, 160.0f, 50.0f };  //光源の位置
	mShadowMap.Init(TEXWIDTH, TEXHEIGHT, Render, shadowColor, lightPos);

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

	//効果音読み込み
	SE_Player_Walk.Load(SE_PLAYER_WALK);	//プレイヤーの歩行時の効果音
	SE_Player_Run.Load(SE_PLAYER_RUN);		//プレイヤーの走行時の効果音
	SE_Player_Avoid.Load(SE_PLAYER_AVOID);	//プレイヤーの回避時の効果音
	SE_Knight_Walk.Load(SE_KNIGHT_WALK);	//敵(ナイト)の歩行時の効果音
	SE_Knight_Run.Load(SE_KNIGHT_RUN);		//敵(ナイト)の走行時の効果音
	SE_Attack_Hit_1.Load(SE_ATTACK_HIT_1);	//攻撃ヒット時の効果1
	SE_Attack_Hit_2.Load(SE_ATTACK_HIT_2);	//攻撃ヒット時の効果2
	SE_Portion_Use.Load(SE_PORTION_USE);	//回復アイテム使用時の効果音
	SE_Trap_Use.Load(SE_TRAP_USE);			//罠アイテム使用時の効果音
	SE_Trap_Active.Load(SE_TRAP_ACTIVE);	//罠アイテム作動時の効果音
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

	//時間計測開始
	if (mCountStart == false) {
		mStartTime = clock();
		mCountStart = true;
	}
	//全ての敵が死亡状態になるまで時間を計測する
	if (CEnemyManager::GetInstance()->mIsEnemyAllDeath()) {
		//クリア時間を記録
		mClearTime = (float)(mEndTime - mStartTime) / 1000;
		//Enterキーを押すとリザルトに移行する
		if (CKey::Once(VK_RETURN)) {
			mScene = ERESULT;
			ShowCursor(true); //カーソル表示
		}
	}
	else {
		mEndTime = clock();
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
	mImagePlayerRun.Draw(110, 190, 20, 100, 0, 255, 255, 0);	//プレイヤーの走り方
	mImagePlayerAttack.Draw(20, 100, 20, 100, 0, 255, 255, 0);	//プレイヤーの攻撃方法
	mImagePlayerAvoid.Draw(200, 280, 20, 100, 0, 255, 255, 0);	//プレイヤーの回避方法
	mImageMouse.Draw(590, 630, 70, 110, 0, 255, 255, 0);	//右クリック用
	mImageMouse.Draw(750, 790, 70, 110, 0, 255, 511, 256);	//ホイール用

	//プレイヤーが死亡状態になるとGAMEOVERと表示する
	if (CXPlayer::GetInstance()->mState == CXPlayer::EPlayerState::EDEATH) {
		mFont.DrawString("GAMEOVER", 120, 350, 40, 40);
		mFont.DrawString("PUSH ENTER", 125, 270, 30, 30);
	}
	//敵が全て死亡状態になるとGAMECLEARと表示する
	else if (CEnemyManager::GetInstance()->mIsEnemyAllDeath()) {
		mFont.DrawString("CLEAR", 230, 350, 40, 40);
		mFont.DrawString("PUSH ENTER", 125, 270, 30, 30);
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
	

