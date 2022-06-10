#include "CSceneTitle.h"
#include "CKey.h"
#include "CUtil.h"
#include "CKey.h"
#include <stdio.h>
#include "CInput.h"
#include "CRes.h"
#include "CSceneGame.h"

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
}

void CSceneTitle::Update()
{
	int mouseX, mouseY;
	CInput::GetMousePosW(&mouseX, &mouseY);
	mouseY = 600 - mouseY;

	//ゲームスタートボタン上にマウスポインタがあるとき
	if ((mouseX >= 240 - 20 && mouseX <= (240 - 20) + (40 * 9)) &&
		(mouseY >= 220 - 20 && mouseY <= 220 + 20)) {
		mSelect = EGAMESTART;
	}
	//レコードボタン上にマウスポインタがあるとき
	else if ((mouseX >= 300 - 20 && mouseX <= (300 - 20) + (40 * 6)) &&
		(mouseY >= 120 - 20 && mouseY <= 120 + 20)) {
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
				/*
				ゲームスタート時の効果音を追加する
				*/
			}
			break;

		case CSceneTitle::ERECORD: //レコード画面
			//ボタンを押していないとき、フェードイン中ではないとき
			if (mIsButtonPush == false && mFade != EFADE_IN) {
				mIsButtonPush = true;	//ボタンを押した
				mFade = EFADE_OUT;		//フェードアウト開始
				mSceneTransitionKeep = CScene::ERECORD; //シーンの遷移先を保存
				CRes::sSETransitionRecord.Play(); //効果音を再生する
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
		}
		break;
	}
}

void CSceneTitle::Render()
{
	CUtil::Start2D(0, 800, 0, 600);

	CRes::sFont.DrawString("BATTLE", 200, 500, 40, 40);
	CRes::sFont.DrawString("COLISEUM", 120, 400, 40, 40);

	//選択中の場所
	switch (mSelect) {
	case EGAMESTART: //ゲームスタートボタン
		CRes::sImageButtonBack.Draw(220, 580, 200, 240, 338, 338, 20, 20); //ボタン背景描画
		break;

	case CSceneTitle::ERECORD: //レコードボタン
		CRes::sImageButtonBack.Draw(280, 520, 100, 140, 338, 338, 20, 20); //ボタン背景描画
		break;

	default:
		break;
	}

	CRes::sFont.DrawString("GAMESTART", 240, 220, 20, 20);

	CRes::sFont.DrawString("RECORD", 300, 120, 20, 20);

	//黒い画像を表示
	CRes::sImageBlack.Draw(0, 800, 0, 600, 1, 1, 1, 1);

	CUtil::End2D();
}

CScene::EScene CSceneTitle::GetNextScene() {
	return mScene;
}