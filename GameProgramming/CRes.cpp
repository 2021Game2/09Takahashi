#include "CRes.h"

bool CRes::sIsResLoad = false; //ファイル読み込み判断用

//モデル系
#define MODEL_PLAYER "Resource\\ラグナ.x"				//プレイヤーモデル
#define MODEL_ENEMY "Resource\\knight\\knight_low.x"	//敵モデル

#define MODEL_MAP "Resource\\sky.obj", "Resource\\sky.mtl"				//マップモデル
#define MODEL_MAP2 "Resource\\Colosseum.obj", "Resource\\Colosseum.mtl" //マップ2モデル
#define MODEL_TRAP "Resource\\trap.obj", "Resource\\trap.mtl"			//罠モデル

//画像系
#define FONT "Resource\\FontG.png"	//フォント画像
#define FONT_ROW 1					//フォント行数
#define FONT_COL 4096 / 64			//フォント列数

#define IMAGE_GAUGE "Resource\\Gauge.png"			//ゲージ画像
#define IMAGE_TARGET "Resource\\Image_Target.png"	//ターゲット画像

#define IMAGE_PORTION "Resource\\Image_Portion.png"	//回復薬画像
#define IMAGE_TRAP "Resource\\Image_Trap.png"		//罠画像
#define IMAGE_NIXSIGN "Resource\\Image_Nixsign.png"	//禁止マーク画像

#define IMAGE_PLAYER_RUN "Resource\\Image_Player_Run.png"		//プレイヤーの走り方説明用画像
#define IMAGE_PLAYER_ATTACK "Resource\\Image_Player_Attack.png" //プレイヤーの攻撃方法説明用画像
#define IMAGE_PLAYER_AVOID "Resource\\Image_Player_Avoid.png"	//プレイヤーの回避方法説明用画像
#define IMAGE_MOUSE "Resource\\Image_Mouse.png"					//マウス操作説明用画像

#define IMAGE_BUTTONBACK "Resource\\Gauge.png"			//ボタン背景画像
#define IMAGE_BACKGROUND "Resource\\Result_Back.png"	//背景画像

//サウンド系
#define SE_PLAYER_WALK "Resource\\SE_Player_Walk.wav"	//プレイヤーの歩行時の効果音
#define SE_PLAYER_RUN "Resource\\SE_Player_Run.wav"		//プレイヤーの走行時の効果音
#define SE_PLAYER_AVOID "Resource\\SE_Player_Avoid.wav"	//プレイヤーの回避時の効果音
#define SE_ATTACK_HIT_1 "Resource\\SE_Attack_Hit_1.wav"	//攻撃ヒット時の効果音1
#define SE_ATTACK_HIT_2 "Resource\\SE_Attack_Hit_2.wav"	//攻撃ヒット時の効果音2
#define SE_PORTION_USE "Resource\\SE_Portion_Use.wav"	//回復アイテム使用時の効果音
#define SE_TRAP_USE "Resource\\SE_Trap_Use.wav"			//罠アイテム使用時の効果音
#define SE_TRAP_ACTIVE "Resource\\SE_Trap_Active.wav"	//罠アイテム作動時の効果音
#define SE_TRANSITION_TITLE "Resource\\SE_Transition_Title.wav" //シーンがタイトルに遷移する時の効果音
#define SE_TRANSITION_RECORD "Resource\\SE_Transition_Record.wav" //シーンがレコードに遷移する時の効果音

//モデル系
CModelX CRes::sModelXPlayer;	//プレイヤーモデル
CModelX CRes::sModelXEnemy;		//敵モデル

CModel CRes::sModelMap;			//マップモデル
CModel CRes::sModelMap2;		//マップ2モデル
CModel CRes::sModelTrap;		//罠モデル

//画像系
CText CRes::sFont;				//フォント画像

CTexture CRes::sImageGauge;		//ゲージ画像
CTexture CRes::sImageTarget;	//ターゲット画像

CTexture CRes::sImagePortion;	//回復薬画像
CTexture CRes::sImageTrap;		//罠画像
CTexture CRes::sImageNixsign;	//禁止マーク画像

CTexture CRes::sImagePlayerRun;		//プレイヤーの走り方説明用画像
CTexture CRes::sImagePlayerAttack;	//プレイヤーの攻撃方法説明用画像
CTexture CRes::sImagePlayerAvoid;	//プレイヤーの回避方法説明用画像
CTexture CRes::sImageMouse;			//マウス操作説明用画像

CTexture CRes::sImageButtonBack;	//ボタン背景画像
CTexture CRes::sImageBackGround;	//背景画像

//サウンド系
CSound CRes::sSEPlayerWalk;		//プレイヤーの歩行時の効果音
CSound CRes::sSEPlayerRun;		//プレイヤーの走行時の効果音
CSound CRes::sSEPlayerAvoid;	//プレイヤーの回避時の効果音
CSound CRes::sSEAttackHit1;		//攻撃ヒット時の効果音1
CSound CRes::sSEAttackHit2;		//攻撃ヒット時の効果音2
CSound CRes::sSEPortionUse;		//回復アイテム使用時の効果音
CSound CRes::sSETrapUse;		//罠アイテム使用時の効果音
CSound CRes::sSETrapActive;		//罠アイテム作動時の効果音
CSound CRes::sSETransitionTitle;	//シーンがタイトルに遷移する時の効果音
CSound CRes::sSETransitionRecord;	//シーンがレコードに遷移する時の効果音

CRes::CRes()
{
	//リソースファイル読み込み、一度だけ通る
	if (sIsResLoad == false) {
		sIsResLoad = true;
		//モデル系
		CRes::sModelXPlayer.Load(MODEL_PLAYER);		//プレイヤーモデル
		CRes::sModelXEnemy.Load(MODEL_ENEMY);		//敵モデル

		CRes::sModelMap.Load(MODEL_MAP);			//マップモデル
		CRes::sModelMap2.Load(MODEL_MAP2);			//マップ2モデル
		CRes::sModelTrap.Load(MODEL_TRAP);			//罠モデル

		//画像系
		CRes::sFont.LoadTexture(FONT, FONT_ROW, FONT_COL);	//フォント画像
		CRes::sImageGauge.Load(IMAGE_GAUGE);	//ゲージ画像
		CRes::sImageTarget.Load(IMAGE_TARGET);	//ターゲット画像

		CRes::sImagePortion.Load(IMAGE_PORTION);	//回復アイテム画像
		CRes::sImageTrap.Load(IMAGE_TRAP);			//罠アイテム画像
		CRes::sImageNixsign.Load(IMAGE_NIXSIGN);	//禁止マーク画像

		CRes::sImagePlayerRun.Load(IMAGE_PLAYER_RUN);		//プレイヤーの走り方説明用画像
		CRes::sImagePlayerAttack.Load(IMAGE_PLAYER_ATTACK);	//プレイヤーの攻撃方法説明用画像
		CRes::sImagePlayerAvoid.Load(IMAGE_PLAYER_AVOID);	//プレイヤーの回避方法説明用画像
		CRes::sImageMouse.Load(IMAGE_MOUSE);				//マウス操作説明用画像

		CRes::sImageButtonBack.Load(IMAGE_BUTTONBACK);	//ボタン背景画像
		CRes::sImageBackGround.Load(IMAGE_BACKGROUND);	//背景画像

		//サウンド系
		CRes::sSEPlayerWalk.Load(SE_PLAYER_WALK);	//プレイヤーの歩行時の効果音
		CRes::sSEPlayerRun.Load(SE_PLAYER_RUN);		//プレイヤーの走行時の効果音
		CRes::sSEPlayerAvoid.Load(SE_PLAYER_AVOID);	//プレイヤーの回避時の効果音
		CRes::sSEAttackHit1.Load(SE_ATTACK_HIT_1);	//攻撃ヒット時の効果音1
		CRes::sSEAttackHit2.Load(SE_ATTACK_HIT_2);	//攻撃ヒット時の効果音2
		CRes::sSEPortionUse.Load(SE_PORTION_USE);	//回復アイテム使用時の効果音
		CRes::sSETrapUse.Load(SE_TRAP_USE);			//罠アイテム使用時の効果音
		CRes::sSETrapActive.Load(SE_TRAP_ACTIVE);	//罠アイテム作動時の効果音
		CRes::sSETransitionTitle.Load(SE_TRANSITION_TITLE);		//シーンがタイトルに遷移する時の効果音
		CRes::sSETransitionRecord.Load(SE_TRANSITION_RECORD);	//シーンがレコードに遷移する時の効果音
	}
}
