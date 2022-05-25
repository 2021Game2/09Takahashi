#include "CSceneRecord.h"
#include "CKey.h"
#include "CUtil.h"
#include <stdio.h>
#include <corecrt_math.h>
#include "CSceneResult.h"
#include "CRes.h"

CSceneRecord::CSceneRecord()
{
}

void CSceneRecord::Init()
{
	mScene = ERECORD; //�V�[�����R�[�h
}

void CSceneRecord::Update()
{
	//���N���b�NorEnter�L�[�Ń^�C�g���ֈڍs����
	if (CKey::Once(VK_LBUTTON) || CKey::Once(VK_RETURN)) {
		mScene = ETITLE;
		CRes::sSETransitionTitle.Play(); //���ʉ����Đ�����
	}

	CUtil::Start2D(0, 800, 0, 600);
	char buf[64];
	CRes::sImageBackGround.Draw(0, 800, 0, 600, 0, 599, 319, 0); //�w�i�摜��\��

	CRes::sFont.DrawString("RECORD", 20, 570, 15, 15);

	CRes::sFont.DrawString("RANKING", 120, 450, 15, 15);

	CRes::sFont.DrawString("RECORD", 490, 450, 15, 15);

	for (int i = 0; i < 5; i++) {
		//�N���A�^�C���̏��5��\��
		sprintf(buf, "%02d:%05.2f", (int)CSceneResult::record[i] / 60, fmod(CSceneResult::record[i], 60));
		CRes::sFont.DrawString(buf, 460, 380 - i * 70, 15, 20);

		//1����5�̐�����\��
		sprintf(buf, "%d", i + 1);
		CRes::sFont.DrawString(buf, 210, 380 - i * 70, 15, 20);
	}

	CUtil::End2D();
}

CScene::EScene CSceneRecord::GetNextScene()
{
	return mScene;
}
