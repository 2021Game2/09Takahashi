#include "CSceneResult.h"
#include "CKey.h"
#include "CUtil.h"
#include <stdio.h>
#include "CSceneGame.h"
#include "CRes.h"
#include "CCamera.h"

#include <fstream>
#include <string>
using namespace std;
#define FILE_RECORD_SAVE "Resource\\Record_Save.txt" //���R�[�h���ۑ�����Ă���e�L�X�g�t�@�C��

#define COUNT_UP_FRAME 1.0f/120.0f //�J�E���g�A�b�v����������܂ł̃t���[����
#define STAY_FRAME 240 //��~�t���[����

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
	, mSceneStayCount(STAY_FRAME)
{
	//������
	for (int i = 0; i < 6; i++) {
		mRecord[i] = 0.0f;
	}

	//���R�[�h���ۑ�����Ă���e�L�X�g�t�@�C����ǂݍ���
	std::ifstream inputfile(FILE_RECORD_SAVE);  //�ǂݍ��ރt�@�C���̃p�X���w��
	std::string line;
	for (int i = 0; i < 6; i++) {
		std::getline(inputfile, line);
		mRecord[i] = std::stod(line);	//�ǂݍ��񂾃��R�[�h��������
	}

	mRecord[5] = CSceneGame::mClearTime; //�N���A���Ԃ�����
	float tmp = 0; //�ޔ�p

	for (int i = 0; i < 6; i++) {
		//�Ō���̂�
		if (i == 5) {
			mRecordTmp[i] = mRecord[i];			//�Q�[���v���C�ŏo���L�^��ݒ�
			mClearTimePosX[i] = 460;			//�N���A�^�C���̕\���ʒuX��ݒ�
			mClearTimePosY[i] = -100;			//�N���A�^�C���̕\���ʒuY��ݒ�(��ʊO)
			mRankingPosY[i] = mClearTimePosY[i];//���ʂ��Ƃ̕\���ʒu��ݒ�(�N���A�^�C���̕\���ʒuY�Ɠ���)
		}
		else {
			mRecordTmp[i] = mRecord[i];			//�\�[�g���s���O�̃��R�[�h���L�^
			mClearTimePosX[i] = 460;			//�N���A�^�C���̕\���ʒuX��ݒ�
			mClearTimePosY[i] = 350 - i * 70;	//�N���A�^�C���̕\���ʒuY��ݒ�
			mRankingPosY[i] = mClearTimePosY[i];//���ʂ��Ƃ̕\���ʒu��ݒ�(�N���A�^�C���̕\���ʒuY�Ɠ���)
		}
	}

	//�N���A�^�C�����������ɂȂ�悤�ɓ���ւ���
	for (int i = 5; i > 0; i--) {
		//���̏��ʂ̋L�^��菬������
		if (mRecord[i - 1] > mRecord[i]) {
			//�L�^����Ă���l�����ւ���
			tmp = mRecord[i];
			mRecord[i] = mRecord[i - 1];
			mRecord[i - 1] = tmp;
			mNewRecord = i - 1;		//�V�L�^�̏��ʂ�ݒ�
			mIsRecordSort = true;	//�\�[�g���s���������f����t���O��true�ɂ���
		}
	}

	//���R�[�h���ۑ�����Ă���e�L�X�g�t�@�C���֏�������
	ofstream outputfile(FILE_RECORD_SAVE);	//�������ރt�@�C���̃p�X���w��
	for (int i = 0; i < 6; i++) {
		outputfile << mRecord[i] << "\n"; //�t�@�C���֏�������
	}
	outputfile.close(); //�t�@�C�������

	CRes::sBGMResult.Repeat(); //BGM���Đ�����
}

void CSceneResult::Init()
{
	mScene = ERESULT; //���U���g

	mFade = EFADE_IN; //�t�F�[�h�C��
}

void CSceneResult::Update()
{
	//���U���g��ʂ̏�Ԃ𔻒f
	switch (mResultState) {
	case CLEARTIME_DISPLAY: //�N���A�^�C���\�����
		ClearTimeDisplay();	//�N���A�^�C���\���������Ă�
		break;

	case RANKING_CHANGE: //�����L���O�\������ւ����
		RankingChange(); //�����L���O�\������ւ��������Ă�
		break;
	}

	//�{�^����������Ă����Ƃ�
	if (mIsButtonPush == true) {
		//�V�[���ړ���x�点��J�E���g��0�ȉ��̂Ƃ�
		if (mSceneStayCount-- <= 0) {
			mFade = EFADE_OUT;				//�t�F�[�h�A�E�g�J�n
			mSceneTransitionKeep = ETITLE;	//�V�[���̑J�ڐ��ۑ�����
			CRes::sBGMResult.Stop(); //BGM���~����
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
			//���ʉ����Đ�����
			CRes::sSECountUp.Repeat();
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

void CSceneResult::Render()
{
	CUtil::Start2D(0, 800, 0, 600);
	char buf[64];

	//�w�i�摜��\��
	CRes::sImageResultBack.Draw(0, 800, 0, 600, 0, 799, 599, 0);

	CRes::sFont.DrawString("RESULT", 20, 570, 15, 15);

	//�N���A�^�C�����J�E���g�A�b�v�ŕ\��
	sprintf(buf, "%02d:%05.2f", (int)mClearTimeCountUp / 60, fmod(mClearTimeCountUp, 60));
	CRes::sFont.DrawString(buf, 290, 490, 15, 20);

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
					CRes::sFont.DrawString("NEW", 320, mRankingPosY[mNewRecord], 15, 15);
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
				//���ʉ����~����
				CRes::sSECountUp.Stop();
			}
		}
		//���N���b�NorEnter�L�[���������Ƃ�
		if (CKey::Once(VK_LBUTTON) || CKey::Once(VK_RETURN)) {
			//�J�E���g�A�b�v���N���A�^�C�����Ⴂ��
			if (mClearTimeCountUp < CSceneGame::mClearTime) {
				//�J�E���g�A�b�v�ɃN���A�^�C����������
				mClearTimeCountUp = CSceneGame::mClearTime;
				//���ʉ����~����
				CRes::sSECountUp.Stop();
			}
			//���R�[�h�̃\�[�g���s���Ă����Ƃ�
			else if (mIsRecordSort == true) {
				//�����L���O�\������ւ���Ԃֈڍs
				mResultState = RANKING_CHANGE;
				//���ʉ��Đ�
				CRes::sSENewRecord.Play();
			}
			//��L�ȊO�̓^�C�g���ֈڍs
			else {
				//�{�^���������Ă��Ȃ��Ƃ��A�t�F�[�h�C�����ł͂Ȃ��Ƃ�
				if (mIsButtonPush == false && mFade != EFADE_IN) {
					mIsButtonPush = true;			//�{�^����������
					CRes::sSEResultEnd.Play();		//���ʉ����Đ�����
				}
			}
		}
	}
}

//�����L���O�\������ւ���Ԃ̏���
void CSceneResult::RankingChange()
{
	//�����L���O�\���ύX�t���O��true�ɂ���
	mIsRankingChange = true;

	//�J�E���g���Z
	count++;

	//���N���b�NorEnter�L�[���������Ƃ�
	if (CKey::Once(VK_LBUTTON) || CKey::Once(VK_RETURN)) {
		//�{�^���������Ă��Ȃ��Ƃ��A�t�F�[�h�C�����ł͂Ȃ��Ƃ�
		if (mIsButtonPush == false && mFade != EFADE_IN) {
			mIsButtonPush = true;			//�{�^����������
			CRes::sSEResultEnd.Play();		//���ʉ����Đ�����
		}
	}
}

CScene::EScene CSceneResult::GetNextScene()
{
	return mScene;
}
