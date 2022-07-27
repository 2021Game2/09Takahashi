#pragma once
#include "CScene.h"

//���U���g��ʂ̃V�[���N���X
class CSceneResult :public CScene {
private:
	int mNewRecord;	//�V�L�^�̏��ʂ��L�^

	int count; //�����_�ŗp�J�E���g

	//�t�F�[�h
	enum EFade {
		EFADE_STOP = 0,	//�t�F�[�h��~
		EFADE_IN,		//�t�F�[�h�C��
		EFADE_OUT		//�t�F�[�h�A�E�g
	};
	EFade mFade; //�t�F�[�h���f�p

	EScene mSceneTransitionKeep; //�V�[���̑J�ڐ��ۑ�����

	bool mIsButtonPush; //�{�^���������Ă����true

	//���U���g��ʂ̏��
	enum EResultState {
		CLEARTIME_DISPLAY = 0,	//�N���A�^�C���\��
		RANKING_CHANGE			//�����L���O�\������ւ�
	};
	EResultState mResultState;	//���U���g��ʂ̏�Ԃ𔻒f

	void ClearTimeDisplay();	//�N���A�^�C���\����Ԃ̏���
	void RankingChange();		//�����L���O�\������ւ���Ԃ̏���

	float mClearTimeCountUp;	//�N���A�^�C�����J�E���g�A�b�v����p

	bool mIsRecordSort;			//���R�[�h�̃\�[�g���s���Ă�����true

	float mRecordTmp[6];		//�����L���O�\���p�A�\�[�g���s���O�̃��R�[�h��ێ�
	float mClearTimePosX[6];	//�N���A�^�C����\������ʒuX
	float mClearTimePosY[6];	//�N���A�^�C����\������ʒuY
	float mRankingPosY[6];		//���ʂ��Ƃ̕\��������WY�A�\������ւ��̍ۂɈړ��̖ڕW�n�_�Ƃ��Ďg�p
	bool mIsRankingChange;		//�����L���O�̕\�������ւ��邩���f����t���O�Atrue�œ���ւ�����

	int mSceneStayCount;		//�V�[���ړ���x�点��p�̃J�E���g

	float mRecord[6];			//�t�@�C������ǂݎ�������R�[�h��ێ�����
	bool mIsNoData;				//���R�[�h���ۑ����ꂽ�t�@�C����ǂݍ��񂾍ہA�f�[�^�����݂��Ȃ����true

public:
	CSceneResult();	//�f�t�H���g�R���X�g���N�^

	void Init();	//����������
	void Update();	//�X�V����
	void Render();	//�`�揈��

	EScene GetNextScene(); //���̃V�[�����擾
};