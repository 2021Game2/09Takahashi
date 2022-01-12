#include "CSceneTitle.h"
#include "CKey.h"
#include "CUtil.h"
#include "CKey.h"
#include <stdio.h>

void CSceneTitle::MenuSelect()
{
	if (CKey::Once(VK_DOWN)) {
		mSelectMenu++;
	}
	else if (CKey::Once(VK_UP)) {
		mSelectMenu--;
	}

	if (mSelectMenu == TAIL) {
		mSelectMenu = HEAD + 1;
	}
	if (mSelectMenu == HEAD) {
		mSelectMenu = TAIL - 1;
	}
}

CSceneTitle::CSceneTitle()
	:mSelectMenu(HEAD+1)
{
}

void CSceneTitle::Init()
{
	mScene = ETITLE;
	mFont.LoadTexture("FontG.png", 1, 4096 / 64);
}

void CSceneTitle::Update()
{
	MenuSelect();

	if (CKey::Once(VK_RETURN)) {
		switch (mSelectMenu) {
		case EGAMESTART:
			mScene = CScene::EGAME;
			break;

		case ERECORD:
			mScene = CScene::ERECORD;
			break;

		case EMANUAL:
			break;
		}
	}

	CUtil::Start2D(0, 800, 0, 600);

	mFont.DrawString("TITLE", 200, 450, 50, 50);

	mFont.DrawString("GAMESTART", 240, 230, 20, 20);

	mFont.DrawString("RECORD", 300, 160, 20, 20);

	mFont.DrawString("MANUAL", 300, 90, 20, 20);

	CUtil::End2D();
}

CScene::EScene CSceneTitle::GetNextScene() {
	return mScene;
}