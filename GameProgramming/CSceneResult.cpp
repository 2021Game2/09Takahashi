#include "CSceneResult.h"
#include "CKey.h"
#include "CUtil.h"
#include <stdio.h>
#include "CSceneGame.h"
#include "CRes.h"

#define DEF_TIME 600.00f //初期化用

float CSceneResult::record[6] = { DEF_TIME,DEF_TIME,DEF_TIME,DEF_TIME,DEF_TIME,DEF_TIME };	//クリア時間の記録用

CSceneResult::CSceneResult()
	:mNewRecord(0)
	, count(0)
	, mFade(EFADE_STOP)
	, mSceneTransitionKeep(ERESULT)
	,mIsButtonPush(false)
{
}

void CSceneResult::Init()
{
	mScene = ERESULT; //リザルト

	mFade = EFADE_IN; //フェードイン

	mNewRecord = 6;

	record[5] = CSceneGame::mClearTime; //クリア時間を入れる
	float tmp = 0; //退避用

	//クリアタイムが早い順になるように入れ替える
	for (int i = 5; i > 0; i--) {
		if (record[i - 1] > record[i]) {
			tmp = record[i];
			record[i] = record[i - 1];
			record[i - 1] = tmp;
			mNewRecord = i - 1;
		}
	}

	count = 0;
}

void CSceneResult::Update()
{
	count++;

	//左クリックorEnterキーを押したとき
	if (CKey::Once(VK_LBUTTON)||CKey::Once(VK_RETURN)) {
		//ボタンを押していないとき、フェードイン中ではないとき
		if (mIsButtonPush == false && mFade != EFADE_IN) {
			mIsButtonPush = true;	//ボタンを押した
			mFade = EFADE_OUT;		//フェードアウト開始
			mSceneTransitionKeep = ETITLE; //シーンの遷移先を保存する
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

	CUtil::Start2D(0, 800, 0, 600);
	char buf[64];
	CRes::sImageBackGround.Draw(0, 800, 0, 600, 0, 599, 319, 0); //背景画像を表示

	CRes::sFont.DrawString("RESULT", 20, 570, 15, 15);

	CRes::sFont.DrawString("CLEARTIME", 270, 550, 15, 15);
	sprintf(buf, "%02d:%05.2f", (int)CSceneGame::mClearTime / 60, fmod(CSceneGame::mClearTime, 60));
	CRes::sFont.DrawString(buf, 290, 490, 15, 20);

	CRes::sFont.DrawString("RANKING", 120, 420, 15, 15);

	CRes::sFont.DrawString("RECORD", 490, 420, 15, 15);

	for (int i = 0; i < 5; i++) {
		//クリアタイムの上位5つを表示
		sprintf(buf, "%02d:%05.2f", (int)record[i] / 60, fmod(record[i], 60));
		CRes::sFont.DrawString(buf, 460, 350 - i * 70, 15, 20);

		//1から5の数字を表示
		sprintf(buf, "%d", i + 1);
		CRes::sFont.DrawString(buf, 210, 350 - i * 70, 15, 20);

		//記録が更新されたとき表示
		if (mNewRecord == i) {
			//点滅させる
			if (count % 30 < 15) {
				CRes::sFont.DrawString("NEW", 310, 350 - i * 70, 15, 15);
			}
		}
	}

	//黒い画像を表示
	CRes::sImageBlack.Draw(0, 800, 0, 600, 1, 1, 1, 1);

	CUtil::End2D();
}

CScene::EScene CSceneResult::GetNextScene()
{
	return mScene;
}
