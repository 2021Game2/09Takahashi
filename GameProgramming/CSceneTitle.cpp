#include "CSceneTitle.h"
#include "CKey.h"
#include "CUtil.h"

void CSceneTitle::Init()
{
	mScene = ETITLE;
	mFont.LoadTexture("FontG.png", 1, 4096 / 64);
}

void CSceneTitle::Update()
{
	if (CKey::Once(VK_RETURN)) {
		mScene = EGAME;
	}

	CUtil::Start2D(0, 800, 0, 600);

	mFont.DrawString("TITLE", 400, 300, 10, 10);

	CUtil::End2D();
}

CScene::EScene CSceneTitle::GetNextScene() {
	return mScene;
}