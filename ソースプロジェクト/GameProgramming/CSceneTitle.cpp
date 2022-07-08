#include "CSceneTitle.h"
#include "CKey.h"
#include "CUtil.h"
#include "CKey.h"
#include <stdio.h>
#include "CInput.h"
#include "CRes.h"
#include "CSceneGame.h"

//スタートボタン座標
#define STARTBUTTON_LEFT 210	//スタートボタン左座標
#define STARTBUTTON_RIGHT 590	//スタートボタン右座標
#define STARTBUTTON_DOWN 180	//スタートボタン下座標
#define STARTBUTTON_UP 240		//スタートボタン上座標

//リザルトボタン座標
#define RESULTBUTTON_LEFT 290	//リザルトボタン左座標
#define RESULTBUTTON_RIGHT 510	//リザルトボタン右座標
#define RESULTBUTTON_DOWN 80	//リザルトボタン下座標
#define RESULTBUTTON_UP 140		//リザルトボタン上座標

CSceneTitle::CSceneTitle()
	:mSelect(EBACKGROUND)
	,mFade(EFADE_STOP)
	,mSceneTransitionKeep(ETITLE)
	,mIsButtonPush(false)
{
}

void CSceneTitle::Init()
{
	mScene = ETITLE; //シーンタイトル
	mFade = EFADE_IN;
	CRes::sBGMTitle.Repeat(); //BGM再生
}

void CSceneTitle::Update()
{
	int mouseX, mouseY;
	CInput::GetMousePosW(&mouseX, &mouseY);
	mouseY = 600 - mouseY;

	//ゲームスタートボタン上にマウスポインタがあるとき
	if ((mouseX >= STARTBUTTON_LEFT && mouseX <= STARTBUTTON_RIGHT) &&
		(mouseY >= STARTBUTTON_DOWN && mouseY <= STARTBUTTON_UP)) {
		mSelect = EGAMESTART;
	}
	//レコードボタン上にマウスポインタがあるとき
	else if ((mouseX >= RESULTBUTTON_LEFT && mouseX <= RESULTBUTTON_RIGHT) &&
		(mouseY >= RESULTBUTTON_DOWN && mouseY <= RESULTBUTTON_UP)) {
		mSelect = CSceneTitle::ERECORD;
	}
	//上記以外は背景
	else {
		mSelect = EBACKGROUND;
	}

	//左クリックしたとき
	if (CKey::Once(VK_LBUTTON)) {
		//選んだボタンを判断する
		switch (mSelect) {
		case EGAMESTART: //ゲームスタート
			//ボタンを押していないとき、フェードイン中ではないとき
			if (mIsButtonPush == false && mFade != EFADE_IN) {
				mIsButtonPush = true;	//ボタンを押した
				mFade = EFADE_OUT;		//フェードアウト開始
				mSceneTransitionKeep = EGAME; //シーンの遷移先を保存
				CRes::sSESelect.Play(); //効果音を再生する
			}
			break;

		case CSceneTitle::ERECORD: //レコード画面
			//ボタンを押していないとき、フェードイン中ではないとき
			if (mIsButtonPush == false && mFade != EFADE_IN) {
				mIsButtonPush = true;	//ボタンを押した
				mFade = EFADE_OUT;		//フェードアウト開始
				mSceneTransitionKeep = CScene::ERECORD; //シーンの遷移先を保存
				CRes::sSESelect.Play(); //効果音を再生する
			}
			break;

		default:
			break;
		}
	}

	//フェードを判断
	switch (mFade) {
	case EFADE_STOP: //フェード停止
		break;

	case EFADE_IN: //フェードイン
		if (CRes::sImageBlack.mAlpha > 0.0f) {
			//黒い画像のアルファ値を下げる
			CRes::sImageBlack.mAlpha -= 0.02f;
		}
		else if(CRes::sImageBlack.mAlpha == 0.0f){
			//フェードインを停止する
			mFade = EFADE_STOP;
		}
		break;

	case EFADE_OUT: //フェードアウト
		if (CRes::sImageBlack.mAlpha < 1.0f) {
			//黒い画像のアルファ値を上げる
			CRes::sImageBlack.mAlpha += 0.02f;
		}
		else if (CRes::sImageBlack.mAlpha == 1.0f) {
			//保存された遷移先へシーンを移行する
			mScene = mSceneTransitionKeep;
			CRes::sBGMTitle.Stop(); //BGM停止
		}
		break;
	}
}

void CSceneTitle::Render()
{
	//2D描画開始
	CUtil::Start2D(0, 800, 0, 600);

	CRes::sImageTitleBack.Draw(0, 800, 0, 600, 0, 800, 600, 0);	//タイトル背景画像
	
	//選択中の場所
	switch (mSelect) {
	case EGAMESTART: //ゲームスタートボタン
		CRes::sImageButtonBack.Draw(STARTBUTTON_LEFT, STARTBUTTON_RIGHT, STARTBUTTON_DOWN, STARTBUTTON_UP, 510, 510, 510, 510); //ボタン背景描画
		break;

	case CSceneTitle::ERECORD: //レコードボタン
		CRes::sImageButtonBack.Draw(RESULTBUTTON_LEFT, RESULTBUTTON_RIGHT, RESULTBUTTON_DOWN, RESULTBUTTON_UP, 510, 510, 510, 510); //ボタン背景描画
		break;

	default:
		break;
	}

	CRes::sImageTitleText.Draw(0, 800, 0, 600, 0, 800, 600, 0);	//タイトルテキスト画像

	//黒い画像を表示
	CRes::sImageBlack.Draw(0, 800, 0, 600, 1, 1, 1, 1);

	//2D描画終了
	CUtil::End2D();
}

CScene::EScene CSceneTitle::GetNextScene() {
	return mScene;
}