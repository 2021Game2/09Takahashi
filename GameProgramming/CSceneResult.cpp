#include "CSceneResult.h"
#include "CKey.h"
#include "CUtil.h"
#include <stdio.h>
#include "CSceneGame.h"
#include "CRes.h"

#define DEF_TIME 600.00f //�������p

float CSceneResult::record[6] = { DEF_TIME,DEF_TIME,DEF_TIME,DEF_TIME,DEF_TIME,DEF_TIME };	//�N���A���Ԃ̋L�^�p

CSceneResult::CSceneResult()
	:mNewRecord(0)
	, count(0)
	, mFade(EFADE_STOP)
	, mSceneTransitionKeep(ERESULT)
	,mIsButtonPush(false)
{
}

void CSceneResult::Init()
{
	mScene = ERESULT; //���U���g

	mFade = EFADE_IN; //�t�F�[�h�C��

	mNewRecord = 6;

	record[5] = CSceneGame::mClearTime; //�N���A���Ԃ�����
	float tmp = 0; //�ޔ�p

	//�N���A�^�C�����������ɂȂ�悤�ɓ���ւ���
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

	//���N���b�NorEnter�L�[���������Ƃ�
	if (CKey::Once(VK_LBUTTON)||CKey::Once(VK_RETURN)) {
		//�{�^���������Ă��Ȃ��Ƃ��A�t�F�[�h�C�����ł͂Ȃ��Ƃ�
		if (mIsButtonPush == false && mFade != EFADE_IN) {
			mIsButtonPush = true;	//�{�^����������
			mFade = EFADE_OUT;		//�t�F�[�h�A�E�g�J�n
			mSceneTransitionKeep = ETITLE; //�V�[���̑J�ڐ��ۑ�����
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

	CUtil::Start2D(0, 800, 0, 600);
	char buf[64];
	CRes::sImageBackGround.Draw(0, 800, 0, 600, 0, 599, 319, 0); //�w�i�摜��\��

	CRes::sFont.DrawString("RESULT", 20, 570, 15, 15);

	CRes::sFont.DrawString("CLEARTIME", 270, 550, 15, 15);
	sprintf(buf, "%02d:%05.2f", (int)CSceneGame::mClearTime / 60, fmod(CSceneGame::mClearTime, 60));
	CRes::sFont.DrawString(buf, 290, 490, 15, 20);

	CRes::sFont.DrawString("RANKING", 120, 420, 15, 15);

	CRes::sFont.DrawString("RECORD", 490, 420, 15, 15);

	for (int i = 0; i < 5; i++) {
		//�N���A�^�C���̏��5��\��
		sprintf(buf, "%02d:%05.2f", (int)record[i] / 60, fmod(record[i], 60));
		CRes::sFont.DrawString(buf, 460, 350 - i * 70, 15, 20);

		//1����5�̐�����\��
		sprintf(buf, "%d", i + 1);
		CRes::sFont.DrawString(buf, 210, 350 - i * 70, 15, 20);

		//�L�^���X�V���ꂽ�Ƃ��\��
		if (mNewRecord == i) {
			//�_�ł�����
			if (count % 30 < 15) {
				CRes::sFont.DrawString("NEW", 310, 350 - i * 70, 15, 15);
			}
		}
	}

	//�����摜��\��
	CRes::sImageBlack.Draw(0, 800, 0, 600, 1, 1, 1, 1);

	CUtil::End2D();
}

CScene::EScene CSceneResult::GetNextScene()
{
	return mScene;
}
