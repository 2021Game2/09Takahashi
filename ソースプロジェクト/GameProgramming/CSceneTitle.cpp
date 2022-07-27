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
#define RESULTBUTTON_LEFT 270	//���U���g�{�^�������W
#define RESULTBUTTON_RIGHT 530	//���U���g�{�^���E���W
#define RESULTBUTTON_DOWN 80	//���U���g�{�^�������W
#define RESULTBUTTON_UP 140		//���U���g�{�^������W

#define BUTTONBACK_ALPHA_UP 0.2f	//�{�^���w�i�̃A���t�@�l���グ��ۂ̒l
#define BUTTONBACK_ALPHA_DOWN 0.1f	//�{�^���w�i�̃A���t�@�l���グ��ۂ̒l

CSceneTitle::CSceneTitle()
	:mSelect(EBACKGROUND)
	, mOldSelect(EBACKGROUND)
	, mFade(EFADE_STOP)
	, mSceneTransitionKeep(ETITLE)
	, mIsButtonPush(false)
	,mButtonBackAlpha1(0.0f)
	,mButtonBackAlpha2(0.0f)
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
	int mouseX = 0;
	int mouseY = 0;
	//�{�^���������Ă��Ȃ��Ƃ��A�t�F�[�h�C�����ł͂Ȃ��Ƃ�
	if (mIsButtonPush == false && mFade != EFADE_IN) {
		//�}�E�X�̍��W���擾
		CInput::GetMousePosW(&mouseX, &mouseY);
		mouseY = 600 - mouseY;
	}

	//�Q�[���X�^�[�g�{�^����Ƀ}�E�X�|�C���^������Ƃ�
	if ((mouseX >= STARTBUTTON_LEFT && mouseX <= STARTBUTTON_RIGHT) &&
		(mouseY >= STARTBUTTON_DOWN && mouseY <= STARTBUTTON_UP)) {
		//�I�����Ă���ꏊ���Q�[���X�^�[�g�ɐݒ�
		mSelect = EGAMESTART;
		//���ݑI�����Ă���ꏊ��1�t���[���O�ɑI�����Ă����ꏊ���Ⴄ�Ƃ�
		if (mSelect != mOldSelect) {
			CRes::sSEButtonCursor.Play(); //���ʉ����Đ�
		}
		//�Q�[���X�^�[�g�{�^���w�i�̃A���t�@�l���グ��
		if (mButtonBackAlpha1 < 1.0f)mButtonBackAlpha1 += BUTTONBACK_ALPHA_UP;
		//���R�[�h�{�^���w�i�̃A���t�@�l��������
		if (mButtonBackAlpha2 > 0.0f)mButtonBackAlpha2 -= BUTTONBACK_ALPHA_DOWN;
	}
	//���R�[�h�{�^����Ƀ}�E�X�|�C���^������Ƃ�
	else if ((mouseX >= RESULTBUTTON_LEFT && mouseX <= RESULTBUTTON_RIGHT) &&
		(mouseY >= RESULTBUTTON_DOWN && mouseY <= RESULTBUTTON_UP)) {
		//�I�����Ă���ꏊ�����R�[�h�ɐݒ�
		mSelect = CSceneTitle::ERECORD;
		//���ݑI�����Ă���ꏊ��1�t���[���O�ɑI�����Ă����ꏊ���Ⴄ�Ƃ�
		if (mSelect != mOldSelect) {
			CRes::sSEButtonCursor.Play(); //���ʉ����Đ�
		}
		//���R�[�h�{�^���w�i�̃A���t�@�l���グ��
		if (mButtonBackAlpha2 < 1.0f)mButtonBackAlpha2 += BUTTONBACK_ALPHA_UP;
		//�Q�[���X�^�[�g�{�^���w�i�̃A���t�@�l��������
		if (mButtonBackAlpha1 > 0.0f)mButtonBackAlpha1 -= BUTTONBACK_ALPHA_DOWN;
	}
	//��L�ȊO�͔w�i
	else {
		//�I�����Ă���ꏊ��w�i�ɐݒ�
		mSelect = EBACKGROUND;
		//�Q�[���X�^�[�g�{�^���w�i�̃A���t�@�l��������
		if (mButtonBackAlpha1 > 0.0f)mButtonBackAlpha1 -= BUTTONBACK_ALPHA_DOWN;
		//���R�[�h�{�^���w�i�̃A���t�@�l��������
		if (mButtonBackAlpha2 > 0.0f)mButtonBackAlpha2 -= BUTTONBACK_ALPHA_DOWN;
	}

	//�I�����Ă����ꏊ�ۑ�
	mOldSelect = mSelect;

	//���N���b�N�����Ƃ�
	if (CKey::Once(VK_LBUTTON)) {
		//�I�����Ă���ꏊ�𔻒f����
		switch (mSelect) {
		case EGAMESTART: //�Q�[���X�^�[�g
				//�{�^���������Ă��Ȃ��Ƃ��A�t�F�[�h�C�����ł͂Ȃ��Ƃ�
			if (mIsButtonPush == false && mFade != EFADE_IN) {
				mIsButtonPush = true;	//�{�^����������
				mFade = EFADE_OUT;		//�t�F�[�h�A�E�g�J�n
				mSceneTransitionKeep = EGAME; //�V�[���̑J�ڐ��ۑ�
				CRes::sSESelect.Play(); //���ʉ����Đ�����
				CRes::sBGMTitle.Stop(); //BGM��~
			}
			break;

		case CSceneTitle::ERECORD: //���R�[�h���
				//�{�^���������Ă��Ȃ��Ƃ��A�t�F�[�h�C�����ł͂Ȃ��Ƃ�
			if (mIsButtonPush == false && mFade != EFADE_IN) {
				mIsButtonPush = true;	//�{�^����������
				mFade = EFADE_OUT;		//�t�F�[�h�A�E�g�J�n
				mSceneTransitionKeep = CScene::ERECORD; //�V�[���̑J�ڐ��ۑ�
				CRes::sSESelect.Play(); //���ʉ����Đ�����
				CRes::sBGMTitle.Stop(); //BGM��~
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
		if (CRes::sImageBlack.GetAlpha() > 0.0f) {
			//�����摜�̃A���t�@�l��������
			CRes::sImageBlack.SetAlpha(-0.02f, true);
		}
		else if (CRes::sImageBlack.GetAlpha() == 0.0f) {
			//�t�F�[�h�C�����~����
			mFade = EFADE_STOP;
		}
		break;

	case EFADE_OUT: //�t�F�[�h�A�E�g
		if (CRes::sImageBlack.GetAlpha() < 1.0f) {
			//�����摜�̃A���t�@�l���グ��
			CRes::sImageBlack.SetAlpha(0.02f, true);
		}
		else if (CRes::sImageBlack.GetAlpha() == 1.0f) {
			//�ۑ����ꂽ�J�ڐ�փV�[�����ڍs����
			mScene = mSceneTransitionKeep;
		}
		break;
	}
}

void CSceneTitle::Render()
{
	//2D�`��J�n
	CUtil::Start2D(0, 800, 0, 600);

	CRes::sImageTitleBack.Draw(0, 800, 0, 600, 0, 800, 600, 0);	//�^�C�g���w�i�摜

	//�Q�[���X�^�[�g�{�^���w�i�̃A���t�@�l��ݒ�
	CRes::sImageButtonBack.SetAlpha(mButtonBackAlpha1);
	//�Q�[���X�^�[�g�{�^���w�i�`��
	CRes::sImageButtonBack.Draw(STARTBUTTON_LEFT, STARTBUTTON_RIGHT, STARTBUTTON_DOWN, STARTBUTTON_UP, 510, 510, 510, 510);

	//���R�[�h�{�^���w�i�̃A���t�@�l��ݒ�
	CRes::sImageButtonBack.SetAlpha(mButtonBackAlpha2);
	//���R�[�h�{�^���w�i�`��
	CRes::sImageButtonBack.Draw(RESULTBUTTON_LEFT, RESULTBUTTON_RIGHT, RESULTBUTTON_DOWN, RESULTBUTTON_UP, 510, 510, 510, 510);

	//�{�^���w�i�摜�̃A���t�@�l��߂�
	CRes::sImageButtonBack.SetAlpha(1.0f);

	CRes::sImageTitleText.Draw(0, 800, 0, 600, 0, 800, 600, 0);	//�^�C�g���e�L�X�g�摜

	//�����摜��\��
	CRes::sImageBlack.Draw(0, 800, 0, 600, 1, 1, 1, 1);

	//2D�`��I��
	CUtil::End2D();
}

CScene::EScene CSceneTitle::GetNextScene() {
	return mScene;
}