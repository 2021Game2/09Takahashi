#include "CSceneTitle.h"
#include "CKey.h"
#include "CUtil.h"
#include "CKey.h"
#include <stdio.h>
#include "CInput.h"
#include "CRes.h"
#include "CSceneGame.h"

//�X�^�[�g�{�^�����W
#define STARTBUTTON_LEFT 210	//�X�^�[�g�{�^�������W
#define STARTBUTTON_RIGHT 590	//�X�^�[�g�{�^���E���W
#define STARTBUTTON_DOWN 180	//�X�^�[�g�{�^�������W
#define STARTBUTTON_UP 240		//�X�^�[�g�{�^������W

//���U���g�{�^�����W
#define RESULTBUTTON_LEFT 290	//���U���g�{�^�������W
#define RESULTBUTTON_RIGHT 510	//���U���g�{�^���E���W
#define RESULTBUTTON_DOWN 80	//���U���g�{�^�������W
#define RESULTBUTTON_UP 140		//���U���g�{�^������W

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
	CRes::sBGMTitle.Repeat(); //BGM�Đ�
}

void CSceneTitle::Update()
{
	int mouseX, mouseY;
	CInput::GetMousePosW(&mouseX, &mouseY);
	mouseY = 600 - mouseY;

	//�Q�[���X�^�[�g�{�^����Ƀ}�E�X�|�C���^������Ƃ�
	if ((mouseX >= STARTBUTTON_LEFT && mouseX <= STARTBUTTON_RIGHT) &&
		(mouseY >= STARTBUTTON_DOWN && mouseY <= STARTBUTTON_UP)) {
		mSelect = EGAMESTART;
	}
	//���R�[�h�{�^����Ƀ}�E�X�|�C���^������Ƃ�
	else if ((mouseX >= RESULTBUTTON_LEFT && mouseX <= RESULTBUTTON_RIGHT) &&
		(mouseY >= RESULTBUTTON_DOWN && mouseY <= RESULTBUTTON_UP)) {
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
				CRes::sSESelect.Play(); //���ʉ����Đ�����
			}
			break;

		case CSceneTitle::ERECORD: //���R�[�h���
			//�{�^���������Ă��Ȃ��Ƃ��A�t�F�[�h�C�����ł͂Ȃ��Ƃ�
			if (mIsButtonPush == false && mFade != EFADE_IN) {
				mIsButtonPush = true;	//�{�^����������
				mFade = EFADE_OUT;		//�t�F�[�h�A�E�g�J�n
				mSceneTransitionKeep = CScene::ERECORD; //�V�[���̑J�ڐ��ۑ�
				CRes::sSESelect.Play(); //���ʉ����Đ�����
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
			CRes::sBGMTitle.Stop(); //BGM��~
		}
		break;
	}
}

void CSceneTitle::Render()
{
	//2D�`��J�n
	CUtil::Start2D(0, 800, 0, 600);

	CRes::sImageTitleBack.Draw(0, 800, 0, 600, 0, 800, 600, 0);	//�^�C�g���w�i�摜
	
	//�I�𒆂̏ꏊ
	switch (mSelect) {
	case EGAMESTART: //�Q�[���X�^�[�g�{�^��
		CRes::sImageButtonBack.Draw(STARTBUTTON_LEFT, STARTBUTTON_RIGHT, STARTBUTTON_DOWN, STARTBUTTON_UP, 510, 510, 510, 510); //�{�^���w�i�`��
		break;

	case CSceneTitle::ERECORD: //���R�[�h�{�^��
		CRes::sImageButtonBack.Draw(RESULTBUTTON_LEFT, RESULTBUTTON_RIGHT, RESULTBUTTON_DOWN, RESULTBUTTON_UP, 510, 510, 510, 510); //�{�^���w�i�`��
		break;

	default:
		break;
	}

	CRes::sImageTitleText.Draw(0, 800, 0, 600, 0, 800, 600, 0);	//�^�C�g���e�L�X�g�摜

	//�����摜��\��
	CRes::sImageBlack.Draw(0, 800, 0, 600, 1, 1, 1, 1);

	//2D�`��I��
	CUtil::End2D();
}

CScene::EScene CSceneTitle::GetNextScene() {
	return mScene;
}