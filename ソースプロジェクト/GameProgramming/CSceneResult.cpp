#include "CSceneResult.h"
#include "CKey.h"
#include "CUtil.h"
#include <stdio.h>
#include "CSceneGame.h"
#include "CRes.h"
#include "CCamera.h"

#define COUNT_UP_FRAME 1.0f/120.0f //�J�E���g�A�b�v����������܂ł̃t���[����
#define DEF_TIME 600.00f //�������p

float CSceneResult::sRecord[6] = { DEF_TIME,DEF_TIME,DEF_TIME,DEF_TIME,DEF_TIME,DEF_TIME };	//�N���A���Ԃ̋L�^�p

CSceneResult::CSceneResult()
	:mNewRecord(6)
	, count(0)
	, mFade(EFADE_STOP)
	, mSceneTransitionKeep(ERESULT)
	, mIsButtonPush(false)
	, mResultState(CLEARTIME_DISPLAY)
	, mClearTimeCountUp(0.0f)
	, mIsRecordSort(false)
	, mIsRankingChange(false)
{
	sRecord[5] = CSceneGame::mClearTime; //�N���A���Ԃ�����
	float tmp = 0; //�ޔ�p

	for (int i = 0; i < 6; i++) {
		//�Ō���̂�
		if (i == 5) {
			mRecordTmp[i] = sRecord[i];			//�Q�[���v���C�ŏo���L�^��ݒ�
			mClearTimePosX[i] = 460;			//�N���A�^�C���̕\���ʒuX��ݒ�
			mClearTimePosY[i] = -100;			//�N���A�^�C���̕\���ʒuY��ݒ�(��ʊO)
			mRankingPosY[i] = mClearTimePosY[i];//���ʂ��Ƃ̕\���ʒu��ݒ�(�N���A�^�C���̕\���ʒuY�Ɠ���)
		}
		else {
			mRecordTmp[i] = sRecord[i];			//�\�[�g���s���O�̃��R�[�h���L�^
			mClearTimePosX[i] = 460;			//�N���A�^�C���̕\���ʒuX��ݒ�
			mClearTimePosY[i] = 350 - i * 70;	//�N���A�^�C���̕\���ʒuY��ݒ�
			mRankingPosY[i] = mClearTimePosY[i];//���ʂ��Ƃ̕\���ʒu��ݒ�(�N���A�^�C���̕\���ʒuY�Ɠ���)
		}
	}

	//�N���A�^�C�����������ɂȂ�悤�ɓ���ւ���
	for (int i = 5; i > 0; i--) {
		//���̏��ʂ̋L�^��菬������
		if (sRecord[i - 1] > sRecord[i]) {
			//�L�^����Ă���l�����ւ���
			tmp = sRecord[i];
			sRecord[i] = sRecord[i - 1];
			sRecord[i - 1] = tmp;
			mNewRecord = i - 1;		//�V�L�^�̏��ʂ�ݒ�
			mIsRecordSort = true;	//�\�[�g���s���������f����t���O��true�ɂ���
		}
	}
}

void CSceneResult::Init()
{
	mScene = ERESULT; //���U���g

	mFade = EFADE_IN; //�t�F�[�h�C��
}

void CSceneResult::Update()
{
	count++;

	//���U���g��ʂ̏�Ԃ𔻒f
	switch (mResultState) {
	case CLEARTIME_DISPLAY: //�N���A�^�C���\�����
		ClearTimeDisplay();	//�N���A�^�C���\���������Ă�
		break;

	case RANKING_CHANGE: //�����L���O�\������ւ����
		RankingChange(); //�����L���O�\������ւ��������Ă�
		break;
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

void CSceneResult::Render()
{
	CUtil::Start2D(0, 800, 0, 600);
	char buf[64];
	CRes::sImageBackGround.Draw(0, 800, 0, 600, 0, 599, 319, 0); //�w�i�摜��\��

	CRes::sFont.DrawString("RESULT", 20, 570, 15, 15);

	CRes::sFont.DrawString("CLEARTIME", 270, 550, 15, 15);
	//�N���A�^�C�����J�E���g�A�b�v�ŕ\��
	sprintf(buf, "%02d:%05.2f", (int)mClearTimeCountUp / 60, fmod(mClearTimeCountUp, 60));
	CRes::sFont.DrawString(buf, 290, 490, 15, 20);

	CRes::sFont.DrawString("RANKING", 120, 420, 15, 15);

	CRes::sFont.DrawString("RECORD", 490, 420, 15, 15);

	for (int i = 0; i < 5; i++) {
		//1����5�̐�����\��
		sprintf(buf, "%d", i + 1);
		CRes::sFont.DrawString(buf, 210, 350 - i * 70, 15, 20);
	}

	for (int i = 0; i < 6; i++) {
		//�N���A�^�C���̏��5��\������
		sprintf(buf, "%02d:%05.2f", (int)mRecordTmp[i] / 60, fmod(mRecordTmp[i], 60));
		//�����L���O�\������ւ��t���O��true�̂Ƃ�
		if (mIsRankingChange == true) {
			//�V�L�^�ȉ��̋L�^�̕\���ʒu��ύX
			if (mNewRecord <= i) {
				//�Q�[�����v���C���ďo���L�^
				if (i == 5) {
					//�V�L�^�̏��ʂ̕\���ʒu�ֈړ�������
					mClearTimePosY[i] = Camera.mLerp(mClearTimePosY[i], mRankingPosY[mNewRecord], 0.1f);
				}
				//���ʂ�5�ʂ̋L�^
				else if (i == 4) {
					//��ʊO�ֈړ�������
					mClearTimePosX[i] = Camera.mLerp(mClearTimePosX[i], 900, 0.1f);
				}
				//��L�ȊO
				else {
					//1���̏��ʂ̕\���ʒu�ֈړ�������
					mClearTimePosY[i] = Camera.mLerp(mClearTimePosY[i], mRankingPosY[i + 1], 0.1f);
				}
			}
			//�V�L�^���o�����ʂ̕\���ʒu�ɕ�����\������
			if (mNewRecord == i) {
				//�_�ł�����
				if (count % 30 < 15) {
					CRes::sFont.DrawString("NEW", 310, mRankingPosY[mNewRecord], 15, 15);
				}
			}
		}
		//�N���A�^�C����\��
		CRes::sFont.DrawString(buf, mClearTimePosX[i], mClearTimePosY[i], 15, 20);
	}

	//�����摜��\��
	CRes::sImageBlack.Draw(0, 800, 0, 600, 1, 1, 1, 1);

	CUtil::End2D();
}

//�N���A�^�C���\����Ԃ̏���
void CSceneResult::ClearTimeDisplay()
{
	//�t�F�[�h�C�����ł͂Ȃ��Ƃ�
	if (mFade != EFADE_IN) {
		//�J�E���g�A�b�v���N���A�^�C�����Ⴂ��
		if (mClearTimeCountUp < CSceneGame::mClearTime) {
			//�J�E���g�A�b�v����
			mClearTimeCountUp += Camera.mLerp(0.0f, CSceneGame::mClearTime, COUNT_UP_FRAME);
			//�J�E���g�A�b�v���N���A�^�C���𒴂�����
			if (mClearTimeCountUp > CSceneGame::mClearTime) {
				//�J�E���g�A�b�v�ɃN���A�^�C����������
				mClearTimeCountUp = CSceneGame::mClearTime;
			}
		}
	}

	//���N���b�NorEnter�L�[���������Ƃ�
	if (CKey::Once(VK_LBUTTON) || CKey::Once(VK_RETURN)) {
		//�J�E���g�A�b�v���N���A�^�C�����Ⴂ��
		if (mClearTimeCountUp < CSceneGame::mClearTime) {
			//�J�E���g�A�b�v�ɃN���A�^�C����������
			mClearTimeCountUp = CSceneGame::mClearTime;
		}
		//���R�[�h�̃\�[�g���s���Ă����Ƃ�
		else if (mIsRecordSort == true) {
			//�����L���O�\������ւ���Ԃֈڍs
			mResultState = RANKING_CHANGE;
		}
		//��L�ȊO�̓^�C�g���ֈڍs
		else {
			//�{�^���������Ă��Ȃ��Ƃ��A�t�F�[�h�C�����ł͂Ȃ��Ƃ�
			if (mIsButtonPush == false && mFade != EFADE_IN) {
				mIsButtonPush = true;			//�{�^����������
				mFade = EFADE_OUT;				//�t�F�[�h�A�E�g�J�n
				mSceneTransitionKeep = ETITLE;	//�V�[���̑J�ڐ��ۑ�����
				CRes::sSESelectBack.Play();		//���ʉ����Đ�����
			}
		}
	}
}

//�����L���O�\������ւ���Ԃ̏���
void CSceneResult::RankingChange()
{
	//�����L���O�\���ύX�t���O��true�ɂ���
	mIsRankingChange = true;

	//���N���b�NorEnter�L�[���������Ƃ�
	if (CKey::Once(VK_LBUTTON) || CKey::Once(VK_RETURN)) {
		//�{�^���������Ă��Ȃ��Ƃ��A�t�F�[�h�C�����ł͂Ȃ��Ƃ�
		if (mIsButtonPush == false && mFade != EFADE_IN) {
			mIsButtonPush = true;			//�{�^����������
			mFade = EFADE_OUT;				//�t�F�[�h�A�E�g�J�n
			mSceneTransitionKeep = ETITLE;	//�V�[���̑J�ڐ��ۑ�����
			CRes::sSESelectBack.Play();		//���ʉ����Đ�����
		}
	}
}

CScene::EScene CSceneResult::GetNextScene()
{
	return mScene;
}
