#include "CSceneTitle.h"
#include "CKey.h"
#include "CUtil.h"
#include "CKey.h"
#include <stdio.h>
#include "CInput.h"
#include "CRes.h"
#include "CSceneGame.h"

CSceneTitle::CSceneTitle()
	:mSelect(EBACKGROUND)
{
}

void CSceneTitle::Init()
{
	mScene = ETITLE; //タイトル
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

	//左クリックしたとき
	if (CKey::Once(VK_LBUTTON)) {
		switch (mSelect) {
		case EGAMESTART:
			mScene = EGAME;
			/*
			ゲームスタート時の効果音を追加する
			*/
			break;

		case CSceneTitle::ERECORD:
			mScene = CScene::ERECORD;
			CRes::sSETransitionRecord.Play(); //効果音を再生する
			break;

		default:
			break;
		}
	}

	CUtil::Start2D(0, 800, 0, 600);

	CRes::sFont.DrawString("BATTLE", 200, 500, 40, 40);
	CRes::sFont.DrawString("COLISEUM", 120, 400, 40, 40);

	switch (mSelect) {
	case EGAMESTART:
		CRes::sImageButtonBack.Draw(220, 580, 200, 240, 338, 338, 20, 20);
		break;

	case CSceneTitle::ERECORD:
		CRes::sImageButtonBack.Draw(280, 520, 100, 140, 338, 338, 20, 20);
		break;

	default:
		break;
	}

	CRes::sFont.DrawString("GAMESTART", 240, 220, 20, 20);

	CRes::sFont.DrawString("RECORD", 300, 120, 20, 20);

	CUtil::End2D();
}

CScene::EScene CSceneTitle::GetNextScene() {
	return mScene;
}