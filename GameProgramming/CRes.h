#ifndef CRES_H
#define CRES_H
#include "CModelX.h"
#include "CModel.h"
#include "CText.h"
#include "CTexture.h"
#include "CSound.h"

//リソースクラス
class CRes {
private:
	static bool sIsResLoad; //リソースファイルの読み込み判断用
public:
	CRes();
	//モデル系
	static CModelX sModelXPlayer;	//プレイヤーモデル
	static CModelX sModelXEnemy;	//敵モデル

	static CModel sModelMap;		//マップモデル
	static CModel sModelMap2;		//マップ2モデル
	static CModel sModelTrap;		//罠モデル
	static CModel sModelCylinder;	//シリンダーモデル

	//画像系
	static CText sFont;				//フォント画像
	static CTexture sImageGauge;	//ゲージ画像
	static CTexture sImageTarget;	//ターゲット画像

	static CTexture sImagePortion;	//回復薬画像
	static CTexture sImageTrap;		//罠画像
	static CTexture sImageNixsign;	//禁止マーク画像

	static CTexture sImagePlayerRun;		//プレイヤーの走り方説明用画像
	static CTexture sImagePlayerAttack;		//プレイヤーの攻撃方法説明用画像
	static CTexture sImagePlayerAvoid;		//プレイヤーの回避方法説明用画像
	static CTexture sImageMouse;			//マウス操作説明用画像

	static CTexture sImageButtonBack;		//ボタン背景画像
	static CTexture sImageBackGround;		//背景画像

	//サウンド系
	static CSound sSEPlayerWalk;	//プレイヤーの歩行時の効果音
	static CSound sSEPlayerRun;		//プレイヤーの走行時の効果音
	static CSound sSEPlayerAvoid;	//プレイヤーの回避時の効果音
	static CSound sSEAttackHit1;	//攻撃ヒット時の効果音1
	static CSound sSEAttackHit2;	//攻撃ヒット時の効果音2
	static CSound sSEPortionUse;	//回復アイテム使用時の効果音
	static CSound sSETrapUse;		//罠アイテム使用時の効果音
	static CSound sSETrapActive;	//罠アイテム作動時の効果音
	static CSound sSETransitionTitle;	//シーンがタイトルに遷移する時の効果音
	static CSound sSETransitionRecord;	//シーンがレコードに遷移する時の効果音
};

#endif
