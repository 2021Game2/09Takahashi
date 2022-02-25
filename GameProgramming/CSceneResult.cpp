#include "CSceneResult.h"
#include "CKey.h"
#include "CUtil.h"
#include <stdio.h>
#include "CSceneGame.h"

#define DEF_TIME 600.00f //初期化用

#define FONT "Resource\\FontG.png" //フォント
#define IMAGE_BACKGROUND "Resource\\Result_Back.png" //背景画像

float CSceneResult::record[6] = { DEF_TIME,DEF_TIME,DEF_TIME,DEF_TIME,DEF_TIME,DEF_TIME };	//クリア時間の記録用

CSceneResult::CSceneResult()
:mNewRecord(0)
,count(0)
{
	mFont.LoadTexture(FONT, 1, 4096 / 64);
	mImageBackGround.Load(IMAGE_BACKGROUND);
}

void CSceneResult::Init()
{
	mScene = ERESULT;

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
	//左クリックorEnterキーでタイトルへ移行する
	if (CKey::Once(VK_LBUTTON)||CKey::Once(VK_RETURN)) {
		mScene = ETITLE;
	}

	CUtil::Start2D(0, 800, 0, 600);
	char buf[64];
	mImageBackGround.Draw(0, 800, 0, 600, 0, 599, 319, 0); //背景画像を表示

	mFont.DrawString("RESULT", 20, 570, 15, 15);

	mFont.DrawString("CLEARTIME", 270, 550, 15, 15);
	sprintf(buf, "%02d:%05.2f", (int)CSceneGame::mClearTime / 60, fmod(CSceneGame::mClearTime, 60));
	mFont.DrawString(buf, 290, 490, 15, 20);

	mFont.DrawString("RANKING", 120, 420, 15, 15);

	mFont.DrawString("RECORD", 490, 420, 15, 15);

	for (int i = 0; i < 5; i++) {
		//クリアタイムの上位5つを表示
		sprintf(buf, "%02d:%05.2f", (int)record[i] / 60, fmod(record[i], 60));
		mFont.DrawString(buf, 460, 350 - i * 70, 15, 20);

		//1から5の数字を表示
		sprintf(buf, "%d", i + 1);
		mFont.DrawString(buf, 210, 350 - i * 70, 15, 20);

		//記録が更新されたとき表示
		if (mNewRecord == i) {
			//点滅させる
			if (count % 30 < 15) {
				mFont.DrawString("NEW", 310, 350 - i * 70, 15, 15);
			}
		}
	}

	CUtil::End2D();
}

CScene::EScene CSceneResult::GetNextScene()
{
	return mScene;
}
