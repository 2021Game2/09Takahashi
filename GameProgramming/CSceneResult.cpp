#include "CSceneResult.h"
#include "CKey.h"
#include "CUtil.h"

void CSceneResult::Init()
{
	mScene = ERESULT;
	mFont.LoadTexture("FontG.png", 1, 4096 / 64);
}

void CSceneResult::Update()
{
	if (CKey::Once(VK_RETURN)) {
		mScene = ETITLE;
	}

	CUtil::Start2D(0, 800, 0, 600);

	mFont.DrawString("RESULT", 400, 300, 10, 10);

	CUtil::End2D();
}

CScene::EScene CSceneResult::GetNextScene()
{
	return mScene;
}
