#ifndef CSCENEGAME_H
#define CSCENEGAME_H

#include "CScene.h"
#include <time.h>
#include "CShadowMap.h"

//�Q�[���̃V�[���N���X
class CSceneGame : public CScene {
private:
	clock_t mStartTime, mEndTime; //�v���J�n�����A�v���I������
	bool mCountStart; //�N���A���Ԍv���J�n�p

	//�V���h�E�}�b�v�̃C���X�^���X
	CShadowMap mShadowMap;

	//�t�F�[�Y
	enum EPhase {
		EPHASE_1 = 0,	//�t�F�[�Y1
		EPHASE_2,		//�t�F�[�Y2
		EPHASE_FINAL,	//�ŏI�t�F�[�Y
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
	CSceneGame();	//�f�t�H���g�R���X�g���N�^
	~CSceneGame();	//�f�X�g���N�^
	//�����������̃I�[�o�[���C�h
	void Init();
	//�X�V�����̃I�[�o�[���C�h
	void Update();
	//�`�揈���̃I�[�o�[���C�h
	void Render();

	EScene GetNextScene(); //���̃V�[�����擾

	static float sClearTime;	//�N���A�܂łɂ�����������
};

//�`��p�O���֐�
void GlobalRender();

#endif
