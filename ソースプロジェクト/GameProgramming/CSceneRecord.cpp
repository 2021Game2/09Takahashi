#include "CSceneRecord.h"
#include "CKey.h"
#include "CUtil.h"
#include <stdio.h>
#include <corecrt_math.h>
#include "CSceneResult.h"
#include "CRes.h"

CSceneRecord::CSceneRecord()
	:mFade(EFADE_STOP)
	,mSceneTransitionKeep(ERECORD)
	,mIsButtonPush(false)
{
}

void CSceneRecord::Init()
{
	mScene = ERECORD; //�V�[�����R�[�h

	mFade = EFADE_IN; //�t�F�[�h�C��
}

void CSceneRecord::Update()
{
	//���N���b�NorEnter�L�[���������Ƃ�
	if (CKey::Once(VK_LBUTTON) || CKey::Once(VK_RETURN)) {
		//�{�^���������Ă��Ȃ��Ƃ��A�t�F�[�h�C�����ł͂Ȃ��Ƃ�
		if (mIsButtonPush == false && mFade != EFADE_IN) {
			mIsButtonPush = true;	//�{�^����������
			mFade = EFADE_OUT;		//�t�F�[�h�A�E�g�J�n
			mSceneTransitionKeep = ETITLE;	//�V�[���̑J�ڐ��ۑ�
			CRes::sSESelectBack.Play();//���ʉ����Đ�����
		}
	}

	//�t�F�[�h�𔻒f
	switch (mFade) {
	case EFADE_STOP: //�t�F�[�h��~
		break;

	case EFADE_IN: //�t�F�[�h�C��
		if (CRes::sImageBlack.mAlpha > 0.0f) {
			//�����摜�̃A���t�@�l��������
			CRes::sImageBlack.mAlpha -= 0.02f;
		}
		else if (CRes::sImageBlack.mAlpha == 0.0f) {
			//�t�F�[�h�C�����~����
			mFade = EFADE_STOP;
		}
		break;

	case EFADE_OUT: //�t�F�[�h�A�E�g
		if (CRes::sImageBlack.mAlpha < 1.0f) {
			//�����摜�̃A���t�@�l���グ��
			CRes::sImageBlack.mAlpha += 0.02f;
		}
		else if (CRes::sImageBlack.mAlpha == 1.0f) {
			//�ۑ����ꂽ�J�ڐ�փV�[�����ڍs����
			mScene = mSceneTransitionKeep;
		}
		break;
	}
}

void CSceneRecord::Render()
{
	CUtil::Start2D(0, 800, 0, 600);
	char buf[64];
	CRes::sImageBackGround.Draw(0, 800, 0, 600, 0, 599, 319, 0); //�w�i�摜��\��

	CRes::sFont.DrawString("RECORD", 20, 570, 15, 15);

	CRes::sFont.DrawString("RANKING", 120, 450, 15, 15);

	CRes::sFont.DrawString("RECORD", 490, 450, 15, 15);

	for (int i = 0; i < 5; i++) {
		//�N���A�^�C���̏��5��\��
		sprintf(buf, "%02d:%05.2f", (int)CSceneResult::sRecord[i] / 60, fmod(CSceneResult::sRecord[i], 60));
		CRes::sFont.DrawString(buf, 460, 380 - i * 70, 15, 20);

		//1����5�̐�����\��
		sprintf(buf, "%d", i + 1);
		CRes::sFont.DrawString(buf, 210, 380 - i * 70, 15, 20);
	}

	//�����摜��\��
	CRes::sImageBlack.Draw(0, 800, 0, 600, 1, 1, 1, 1);

	CUtil::End2D();
}

CScene::EScene CSceneRecord::GetNextScene()
{
	return mScene;
}
