#pragma once

#include "CScene.h"
#include <time.h>
#include "CShadowMap.h"

//�Q�[���̃V�[���N���X
class CSceneGame2 : public CScene {
private:
	clock_t mStartTime, mEndTime; //�v���J�n�����A�v���I������
	bool mCountStart; //�N���A���Ԍv���J�n�p

	//�V���h�E�}�b�v�̃C���X�^���X
	CShadowMap mShadowMap;

	//�t�F�[�Y
	enum EPhase {
		EPHASE_1 = 0,
		EPHASE_2,
		EPHASE_FINAL,
	};
	EPhase mPhase; //�t�F�[�Y���f�p

	//�t�F�[�h
	enum EFade {
		EFADE_IN = 0,	//�t�F�[�h�C��
		EFADE_OUT		//�t�F�[�h�A�E�g
	};
	EFade mFade; //�t�F�[�h���f�p

	EScene mSceneTransitionKeep; //�V�[���̑J�ڐ��ۑ�����
public:
	CSceneGame2();
	~CSceneGame2();
	//�����������̃I�[�o�[���C�h
	void Init();
	//�X�V�����̃I�[�o�[���C�h
	void Update();

	EScene GetNextScene(); //���̃V�[�����擾
};

