#include "CSceneTitle.h"
#include "CKey.h"
#include "CUtil.h"
#include "CKey.h"
#include <stdio.h>

#include "CInput.h"

CSceneTitle::CSceneTitle()
	:mSelect(EBACKGROUND)
{
	mTexture.Load("Gauge.png");
}

void CSceneTitle::Init()
{
	mScene = ETITLE;
	mFont.LoadTexture("FontG.png", 1, 4096 / 64);
}

void CSceneTitle::Update()
{
	int mouseX, mouseY;
	CInput::GetMousePosW(&mouseX, &mouseY);
	mouseY = 600 - mouseY;

	mSelect = EBACKGROUND;

	if ((mouseX >= 240 - 20 && mouseX <= (240 - 20) + (40 * 9)) &&
		(mouseY >= 230 - 20 && mouseY <= 230 + 20)) {
		mSelect = EGAMESTART;
	}

	if ((mouseX >= 300 - 20 && mouseX <= (300 - 20) + (40 * 6)) &&
		(mouseY >= 160 - 20 && mouseY <= 160 + 20)) {
		mSelect = CSceneTitle::ERECORD;
	}

	if ((mouseX >= 300 - 20 && mouseX <= (300 - 20) + (40 * 6)) &&
		(mouseY >= 90 - 20 && mouseY <= 90 + 20)) {
		mSelect = EMANUAL;
	}

	if (CKey::Once(VK_LBUTTON)) {
		switch (mSelect) {
		case EGAMESTART:
			mScene = EGAME;
			break;

		case CSceneTitle::ERECORD:
			mScene = CScene::ERECORD;
			break;

		case EMANUAL:
			break;

		default:
			break;
		}
	}

	CUtil::Start2D(0, 800, 0, 600);

	mFont.DrawString("TITLE", 200, 450, 50, 50);

	switch (mSelect) {
	case EGAMESTART:
		mTexture.Draw(220, 580, 210, 250, 338, 338, 20, 20);
		break;

	case CSceneTitle::ERECORD:
		mTexture.Draw(280, 520, 140, 180, 338, 338, 20, 20);
		break;

	case EMANUAL:
		mTexture.Draw(280, 520, 70, 110, 338, 338, 20, 20);
		break;

	default:
		break;
	}

	mFont.DrawString("GAMESTART", 240, 230, 20, 20);

	mFont.DrawString("RECORD", 300, 160, 20, 20);

	mFont.DrawString("MANUAL", 300, 90, 20, 20);

	CUtil::End2D();
}

CScene::EScene CSceneTitle::GetNextScene() {
	return mScene;
}