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
#define RESULTBUTTON_LEFT 270	//リザルトボタン左座標
#define RESULTBUTTON_RIGHT 530	//リザルトボタン右座標
#define RESULTBUTTON_DOWN 80	//リザルトボタン下座標
#define RESULTBUTTON_UP 140		//リザルトボタン上座標

#define BUTTONBACK_ALPHA_UP 0.2f	//ボタン背景のアルファ値を上げる際の値
#define BUTTONBACK_ALPHA_DOWN 0.1f	//ボタン背景のアルファ値を上げる際の値

CSceneTitle::CSceneTitle()
	:mSelect(EBACKGROUND)
	, mOldSelect(EBACKGROUND)
	, mFade(EFADE_STOP)
	, mSceneTransitionKeep(ETITLE)
	, mIsButtonPush(false)
	,mButtonBackAlpha1(0.0f)
	,mButtonBackAlpha2(0.0f)
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
	int mouseX = 0;
	int mouseY = 0;
	//ボタンを押していないとき、フェードイン中ではないとき
	if (mIsButtonPush == false && mFade != EFADE_IN) {
		//マウスの座標を取得
		CInput::GetMousePosW(&mouseX, &mouseY);
		mouseY = 600 - mouseY;
	}

	//ゲームスタートボタン上にマウスポインタがあるとき
	if ((mouseX >= STARTBUTTON_LEFT && mouseX <= STARTBUTTON_RIGHT) &&
		(mouseY >= STARTBUTTON_DOWN && mouseY <= STARTBUTTON_UP)) {
		//選択している場所をゲームスタートに設定
		mSelect = EGAMESTART;
		//現在選択している場所と1フレーム前に選択していた場所が違うとき
		if (mSelect != mOldSelect) {
			CRes::sSEButtonCursor.Play(); //効果音を再生
		}
		//ゲームスタートボタン背景のアルファ値を上げる
		if (mButtonBackAlpha1 < 1.0f)mButtonBackAlpha1 += BUTTONBACK_ALPHA_UP;
		//レコードボタン背景のアルファ値を下げる
		if (mButtonBackAlpha2 > 0.0f)mButtonBackAlpha2 -= BUTTONBACK_ALPHA_DOWN;
	}
	//レコードボタン上にマウスポインタがあるとき
	else if ((mouseX >= RESULTBUTTON_LEFT && mouseX <= RESULTBUTTON_RIGHT) &&
		(mouseY >= RESULTBUTTON_DOWN && mouseY <= RESULTBUTTON_UP)) {
		//選択している場所をレコードに設定
		mSelect = CSceneTitle::ERECORD;
		//現在選択している場所と1フレーム前に選択していた場所が違うとき
		if (mSelect != mOldSelect) {
			CRes::sSEButtonCursor.Play(); //効果音を再生
		}
		//レコードボタン背景のアルファ値を上げる
		if (mButtonBackAlpha2 < 1.0f)mButtonBackAlpha2 += BUTTONBACK_ALPHA_UP;
		//ゲームスタートボタン背景のアルファ値を下げる
		if (mButtonBackAlpha1 > 0.0f)mButtonBackAlpha1 -= BUTTONBACK_ALPHA_DOWN;
	}
	//上記以外は背景
	else {
		//選択している場所を背景に設定
		mSelect = EBACKGROUND;
		//ゲームスタートボタン背景のアルファ値を下げる
		if (mButtonBackAlpha1 > 0.0f)mButtonBackAlpha1 -= BUTTONBACK_ALPHA_DOWN;
		//レコードボタン背景のアルファ値を下げる
		if (mButtonBackAlpha2 > 0.0f)mButtonBackAlpha2 -= BUTTONBACK_ALPHA_DOWN;
	}

	//選択していた場所保存
	mOldSelect = mSelect;

	//左クリックしたとき
	if (CKey::Once(VK_LBUTTON)) {
		//選択している場所を判断する
		switch (mSelect) {
		case EGAMESTART: //ゲームスタート
				//ボタンを押していないとき、フェードイン中ではないとき
			if (mIsButtonPush == false && mFade != EFADE_IN) {
				mIsButtonPush = true;	//ボタンを押した
				mFade = EFADE_OUT;		//フェードアウト開始
				mSceneTransitionKeep = EGAME; //シーンの遷移先を保存
				CRes::sSESelect.Play(); //効果音を再生する
				CRes::sBGMTitle.Stop(); //BGM停止
			}
			break;

		case CSceneTitle::ERECORD: //レコード画面
				//ボタンを押していないとき、フェードイン中ではないとき
			if (mIsButtonPush == false && mFade != EFADE_IN) {
				mIsButtonPush = true;	//ボタンを押した
				mFade = EFADE_OUT;		//フェードアウト開始
				mSceneTransitionKeep = CScene::ERECORD; //シーンの遷移先を保存
				CRes::sSESelect.Play(); //効果音を再生する
				CRes::sBGMTitle.Stop(); //BGM停止
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
		if (CRes::sImageBlack.GetAlpha() > 0.0f) {
			//黒い画像のアルファ値を下げる
			CRes::sImageBlack.SetAlpha(-0.02f, true);
		}
		else if (CRes::sImageBlack.GetAlpha() == 0.0f) {
			//フェードインを停止する
			mFade = EFADE_STOP;
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
		}
		break;
	}
}

void CSceneTitle::Render()
{
	//2D描画開始
	CUtil::Start2D(0, 800, 0, 600);

	CRes::sImageTitleBack.Draw(0, 800, 0, 600, 0, 800, 600, 0);	//タイトル背景画像

	//ゲームスタートボタン背景のアルファ値を設定
	CRes::sImageButtonBack.SetAlpha(mButtonBackAlpha1);
	//ゲームスタートボタン背景描画
	CRes::sImageButtonBack.Draw(STARTBUTTON_LEFT, STARTBUTTON_RIGHT, STARTBUTTON_DOWN, STARTBUTTON_UP, 510, 510, 510, 510);

	//レコードボタン背景のアルファ値を設定
	CRes::sImageButtonBack.SetAlpha(mButtonBackAlpha2);
	//レコードボタン背景描画
	CRes::sImageButtonBack.Draw(RESULTBUTTON_LEFT, RESULTBUTTON_RIGHT, RESULTBUTTON_DOWN, RESULTBUTTON_UP, 510, 510, 510, 510);

	//ボタン背景画像のアルファ値を戻す
	CRes::sImageButtonBack.SetAlpha(1.0f);

	CRes::sImageTitleText.Draw(0, 800, 0, 600, 0, 800, 600, 0);	//タイトルテキスト画像

	//黒い画像を表示
	CRes::sImageBlack.Draw(0, 800, 0, 600, 1, 1, 1, 1);

	//2D描画終了
	CUtil::End2D();
}

CScene::EScene CSceneTitle::GetNextScene() {
	return mScene;
}