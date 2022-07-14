#include "CSceneRecord.h"
#include "CKey.h"
#include "CUtil.h"
#include <stdio.h>
#include <corecrt_math.h>
#include "CSceneResult.h"
#include "CRes.h"

CSceneRecord::CSceneRecord()
	:mFade(EFADE_STOP)
	,mSceneTransitionKeep(ERECORD)
	,mIsButtonPush(false)
{
}

void CSceneRecord::Init()
{
	mScene = ERECORD; //シーンレコード

	mFade = EFADE_IN; //フェードイン
}

void CSceneRecord::Update()
{
	//左クリックorEnterキーを押したとき
	if (CKey::Once(VK_LBUTTON) || CKey::Once(VK_RETURN)) {
		//ボタンを押していないとき、フェードイン中ではないとき
		if (mIsButtonPush == false && mFade != EFADE_IN) {
			mIsButtonPush = true;	//ボタンを押した
			mFade = EFADE_OUT;		//フェードアウト開始
			mSceneTransitionKeep = ETITLE;	//シーンの遷移先を保存
			CRes::sSESelectBack.Play();//効果音を再生する
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
		else if (CRes::sImageBlack.mAlpha == 0.0f) {
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

void CSceneRecord::Render()
{
	CUtil::Start2D(0, 800, 0, 600);
	char buf[64];
	CRes::sImageBackGround.Draw(0, 800, 0, 600, 0, 599, 319, 0); //背景画像を表示

	CRes::sFont.DrawString("RECORD", 20, 570, 15, 15);

	CRes::sFont.DrawString("RANKING", 120, 450, 15, 15);

	CRes::sFont.DrawString("RECORD", 490, 450, 15, 15);

	for (int i = 0; i < 5; i++) {
		//クリアタイムの上位5つを表示
		sprintf(buf, "%02d:%05.2f", (int)CSceneResult::sRecord[i] / 60, fmod(CSceneResult::sRecord[i], 60));
		CRes::sFont.DrawString(buf, 460, 380 - i * 70, 15, 20);

		//1から5の数字を表示
		sprintf(buf, "%d", i + 1);
		CRes::sFont.DrawString(buf, 210, 380 - i * 70, 15, 20);
	}

	//黒い画像を表示
	CRes::sImageBlack.Draw(0, 800, 0, 600, 1, 1, 1, 1);

	CUtil::End2D();
}

CScene::EScene CSceneRecord::GetNextScene()
{
	return mScene;
}
