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
//CMatrix Matrix;

//画像系
#define FONT "Resource\\FontG.png" //フォント
#define EFFECT_ATTACK_HIT "Resource\\Effect_Attack_Hit.png"		//攻撃ヒット時のエフェクト画像
#define EFFECT_PORTION_USE "Resource\\Effect_Portion_Use.png"	//回復アイテム使用時のエフェクト画像
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

CSceneGame::~CSceneGame() {
	CTrapManager::Release();
	CMap2::Release();
}

void CSceneGame::Init() {
	mScene = EGAME;

	mCountStart = false;
	mClearTime = 0.0f;

	//テキストフォントの読み込みと設定
	mFont.LoadTexture(FONT, 1, 4096 / 64);

	CRes::sModelX.Load(MODEL_FILE);
	CRes::sKnight.Load(MODEL_ENEMY);
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
	mPlayer.mPosition = CVector(0.0f, 0.0f, 20.0f);
	mPlayer.mRotation = CVector(0.0f, 180.0f, 0.0f);

	//敵の初期設定
	mEnemy.Init(&CRes::sKnight);
	mEnemy.mAnimationFrameSize = 1024;
	//敵の配置
	mEnemy.mPosition = CVector(0.0f, 0.0f, -10.0f);
	//mEnemy.ChangeAnimation(2, true, 200);

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
	CTaskManager::Get()->Update();

	//衝突処理
	//CCollisionManager::Get()->Collision();
	CTaskManager::Get()->TaskCollision();
	/*
	Camera.Update();

	Camera.Render();
	*/
	//タスクリスト削除
	CTaskManager::Get()->Delete();
	//タスク描画
	//CTaskManager::Get()->Render();
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
		//Enterキーを押すとリザルトに移行する
		if (CKey::Once(VK_RETURN)) {
			mScene = ERESULT;
			ShowCursor(true); //カーソル表示
		}
	}

	//プレイヤーが死亡状態のとき
	if (CXPlayer::GetInstance()->mState == CXPlayer::EPlayerState::EDEATH) {
		//Enterキーでタイトルに移行する
		if (CKey::Once(VK_RETURN)) {
			mScene = ETITLE;
			ShowCursor(true); //カーソル表示
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

	//効果音のテスト
	if (CKey::Once('1')) {
		SE_Attack_Hit_1.Play();
	}
	else if (CKey::Once('2')) {
		SE_Attack_Hit_2.Play();
	}
	else if (CKey::Once('3')) {
		SE_Trap_Active.Play();
	}
#endif

	if (CXPlayer::GetInstance()->mState == CXPlayer::EPlayerState::EDEATH) {
		mFont.DrawString("GAMEOVER", 120, 300, 40, 40);
		mFont.DrawString("PUSH ENTER", 30, 30, 20, 20);
	}
	else if (CXEnemy::GetInstance()->DeathFlag() == true) {
		mFont.DrawString("CLEAR", 230, 300, 40, 40);
		mFont.DrawString("PUSH ENTER", 30, 30, 20, 20);
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
	

