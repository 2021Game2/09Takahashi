#include "CSceneTitle.h"
#include "CKey.h"
#include "CUtil.h"
#include "CKey.h"
#include <stdio.h>

#include "CInput.h"

#define FONT "Resource\\FontG.png" //フォント
#define IMAGE_BUTTONBACK "Resource\\Gauge.png" //ボタン背景画像

CSceneTitle::CSceneTitle()
	:mSelect(EBACKGROUND)
{
	mFont.LoadTexture(FONT, 1, 4096 / 64);
	mImageButtonBack.Load(IMAGE_BUTTONBACK);
}

void CSceneTitle::Init()
{
	mScene = ETITLE;
}

void CSceneTitle::Update()
{
	int mouseX, mouseY;
	CInput::GetMousePosW(&mouseX, &mouseY);
	mouseY = 600 - mouseY;

	mSelect = EBACKGROUND;

	if ((mouseX >= 240 - 20 && mouseX <= (240 - 20) + (40 * 9)) &&
		(mouseY >= 220 - 20 && mouseY <= 220 + 20)) {
		mSelect = EGAMESTART;
	}

	if ((mouseX >= 300 - 20 && mouseX <= (300 - 20) + (40 * 6)) &&
		(mouseY >= 120 - 20 && mouseY <= 120 + 20)) {
		mSelect = CSceneTitle::ERECORD;
	}

	if (CKey::Once(VK_LBUTTON)) {
		switch (mSelect) {
		case EGAMESTART:
			mScene = EGAME;
			break;

		case CSceneTitle::ERECORD:
			mScene = CScene::ERECORD;
			break;

		default:
			break;
		}
	}

	CUtil::Start2D(0, 800, 0, 600);

	mFont.DrawString("BATTLE", 200, 500, 40, 40);
	mFont.DrawString("COLISEUM", 120, 400, 40, 40);

	switch (mSelect) {
	case EGAMESTART:
		mImageButtonBack.Draw(220, 580, 200, 240, 338, 338, 20, 20);
		break;

	case CSceneTitle::ERECORD:
		mImageButtonBack.Draw(280, 520, 100, 140, 338, 338, 20, 20);
		break;

	default:
		break;
	}

	mFont.DrawString("GAMESTART", 240, 220, 20, 20);

	mFont.DrawString("RECORD", 300, 120, 20, 20);

	CUtil::End2D();
}

CScene::EScene CSceneTitle::GetNextScene() {
	return mScene;
}