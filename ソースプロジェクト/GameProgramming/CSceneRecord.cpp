#include "CSceneRecord.h"
#include "CKey.h"
#include "CUtil.h"
#include <stdio.h>
#include <corecrt_math.h>
#include "CSceneResult.h"
#include "CRes.h"

#include <fstream>
#include <string>
using namespace std;

#define FILE_RECORD_SAVE "Resource\\Record_Save.txt" //���R�[�h���ۑ�����Ă���e�L�X�g�t�@�C��
#define DEF_RECORD 600.0f //�f�[�^�����݂��Ȃ��������ɐݒ肷��l

CSceneRecord::CSceneRecord()
	:mFade(EFADE_STOP)
	, mSceneTransitionKeep(ERECORD)
	, mIsButtonPush(false)
	, mIsNoData(false)
{
	//������
	for (int i = 0; i < 6; i++) {
		mRecord[i] = 0.0f;
	}

	//���R�[�h���ۑ�����Ă���e�L�X�g�t�@�C����ǂݍ���
	std::ifstream inputfile(FILE_RECORD_SAVE);  // �ǂݍ��ރt�@�C���̃p�X���w��
	std::string line;
	for (int i = 0; i < 6; i++) {
		//�ǂݍ���
		if (std::getline(inputfile, line)) {
			mRecord[i] = std::stod(line);	//�ǂݍ��񂾃��R�[�h��������
		}
		//�ǂݍ��߂Ȃ������Ƃ�
		else {
			mIsNoData = true;	//�f�[�^�����݂��Ȃ��̂�true
			break;				//for���𔲂���
		}
	}

	//�ǂݍ��񂾃t�@�C���Ƀf�[�^�����݂��Ȃ������ꍇ
	if (mIsNoData == true) {
		//���R�[�h���ۑ�����Ă���e�L�X�g�t�@�C���֏�������
		ofstream outputfile(FILE_RECORD_SAVE);	//�������ރt�@�C���̃p�X���w��
		for (int i = 0; i < 6; i++) {
			mRecord[i] = DEF_RECORD;			//�l��ݒ肷��
			outputfile << mRecord[i] << "\n";	//�t�@�C���֏�������
		}
		outputfile.close(); //�t�@�C�������
	}
}

void CSceneRecord::Init()
{
	mScene = ERECORD; //�V�[�����R�[�h

	mFade = EFADE_IN; //�t�F�[�h�C��

	CRes::sBGMRecord.Repeat(); //BGM���Đ�����
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
			CRes::sSESelectBack.Play();	//���ʉ����Đ�����
			CRes::sBGMRecord.Stop();	//BGM���~����
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

void CSceneRecord::Render()
{
	CUtil::Start2D(0, 800, 0, 600);
	char buf[64];
	CRes::sImageRecordBack.Draw(0, 800, 0, 600, 0, 799, 599, 0); //�w�i�摜��\��

	CRes::sFont.DrawString("RECORD", 20, 570, 15, 15);

	for (int i = 0; i < 5; i++) {
		//�N���A�^�C���̏��5��\��
		sprintf(buf, "%02d:%05.2f", (int)mRecord[i] / 60, fmod(mRecord[i], 60));
		CRes::sFont.DrawString(buf, 460, 380 - i * 70, 15, 20);
	}

	//�����摜��\��
	CRes::sImageBlack.Draw(0, 800, 0, 600, 1, 1, 1, 1);

	CUtil::End2D();
}

CScene::EScene CSceneRecord::GetNextScene()
{
	return mScene;
}
