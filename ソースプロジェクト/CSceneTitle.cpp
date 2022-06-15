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
	,mFade(EFADE_STOP)
	,mSceneTransitionKeep(ETITLE)
	,mIsButtonPush(false)
{
}

void CSceneTitle::Init()
{
	mScene = ETITLE; //�V�[���^�C�g��
	mFade = EFADE_IN;
}

void CSceneTitle::Update()
{
	int mouseX, mouseY;
	CInput::GetMousePosW(&mouseX, &mouseY);
	mouseY = 600 - mouseY;

	//�Q�[���X�^�[�g�{�^����Ƀ}�E�X�|�C���^������Ƃ�
	if ((mouseX >= 240 - 20 && mouseX <= (240 - 20) + (40 * 9)) &&
		(mouseY >= 220 - 20 && mouseY <= 220 + 20)) {
		mSelect = EGAMESTART;
	}
	//���R�[�h�{�^����Ƀ}�E�X�|�C���^������Ƃ�
	else if ((mouseX >= 300 - 20 && mouseX <= (300 - 20) + (40 * 6)) &&
		(mouseY >= 120 - 20 && mouseY <= 120 + 20)) {
		mSelect = CSceneTitle::ERECORD;
	}
	//��L�ȊO�͔w�i
	else {
		mSelect = EBACKGROUND;
	}

	//���N���b�N�����Ƃ�
	if (CKey::Once(VK_LBUTTON)) {
		//�I�񂾃{�^���𔻒f����
		switch (mSelect) {
		case EGAMESTART: //�Q�[���X�^�[�g
			//�{�^���������Ă��Ȃ��Ƃ��A�t�F�[�h�C�����ł͂Ȃ��Ƃ�
			if (mIsButtonPush == false && mFade != EFADE_IN) {
				mIsButtonPush = true;	//�{�^����������
				mFade = EFADE_OUT;		//�t�F�[�h�A�E�g�J�n
				mSceneTransitionKeep = EGAME; //�V�[���̑J�ڐ��ۑ�
				/*
				�Q�[���X�^�[�g���̌��ʉ���ǉ�����
				*/
			}
			break;

		case CSceneTitle::ERECORD: //���R�[�h���
			//�{�^���������Ă��Ȃ��Ƃ��A�t�F�[�h�C�����ł͂Ȃ��Ƃ�
			if (mIsButtonPush == false && mFade != EFADE_IN) {
				mIsButtonPush = true;	//�{�^����������
				mFade = EFADE_OUT;		//�t�F�[�h�A�E�g�J�n
				mSceneTransitionKeep = CScene::ERECORD; //�V�[���̑J�ڐ��ۑ�
				CRes::sSETransitionRecord.Play(); //���ʉ����Đ�����
			}
			break;

		default:
			break;
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
		else if(CRes::sImageBlack.mAlpha == 0.0f){
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

void CSceneTitle::Render()
{
	CUtil::Start2D(0, 800, 0, 600);

	CRes::sFont.DrawString("BATTLE", 200, 500, 40, 40);
	CRes::sFont.DrawString("COLISEUM", 120, 400, 40, 40);

	//�I�𒆂̏ꏊ
	switch (mSelect) {
	case EGAMESTART: //�Q�[���X�^�[�g�{�^��
		CRes::sImageButtonBack.Draw(220, 580, 200, 240, 338, 338, 20, 20); //�{�^���w�i�`��
		break;

	case CSceneTitle::ERECORD: //���R�[�h�{�^��
		CRes::sImageButtonBack.Draw(280, 520, 100, 140, 338, 338, 20, 20); //�{�^���w�i�`��
		break;

	default:
		break;
	}

	CRes::sFont.DrawString("GAMESTART", 240, 220, 20, 20);

	CRes::sFont.DrawString("RECORD", 300, 120, 20, 20);

	//�����摜��\��
	CRes::sImageBlack.Draw(0, 800, 0, 600, 1, 1, 1, 1);

	CUtil::End2D();
}

CScene::EScene CSceneTitle::GetNextScene() {
	return mScene;
}