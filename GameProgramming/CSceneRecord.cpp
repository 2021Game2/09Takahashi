#include "CSceneRecord.h"
#include "CKey.h"
#include "CUtil.h"
#include <stdio.h>
#include <corecrt_math.h>

#include "CSceneResult.h"

#define FONT "Resource\\FontG.png" //フォント
#define IMAGE_BACKGROUND "Resource\\Result_Back.png" //背景画像

CSceneRecord::CSceneRecord()
{
	mFont.LoadTexture(FONT, 1, 4096 / 64);
	mImageBackGround.Load(IMAGE_BACKGROUND);
}

void CSceneRecord::Init()
{
	mScene = ERECORD;
}

void CSceneRecord::Update()
{
	//左クリックorEnterキーでタイトルへ移行する
	if (CKey::Once(VK_LBUTTON) || CKey::Once(VK_RETURN)) {
		mScene = ETITLE;
	}

	CUtil::Start2D(0, 800, 0, 600);
	char buf[64];
	mImageBackGround.Draw(0, 800, 0, 600, 0, 599, 319, 0); //背景画像を表示

	mFont.DrawString("RECORD", 20, 570, 15, 15);

	mFont.DrawString("RANKING", 120, 450, 15, 15);

	mFont.DrawString("RECORD", 490, 450, 15, 15);

	for (int i = 0; i < 5; i++) {
		//クリアタイムの上位5つを表示
		sprintf(buf, "%02d:%05.2f", (int)CSceneResult::record[i] / 60, fmod(CSceneResult::record[i], 60));
		mFont.DrawString(buf, 460, 380 - i * 70, 15, 20);

		//1から5の数字を表示
		sprintf(buf, "%d", i + 1);
		mFont.DrawString(buf, 210, 380 - i * 70, 15, 20);
	}

	CUtil::End2D();
}

CScene::EScene CSceneRecord::GetNextScene()
{
	return mScene;
}
