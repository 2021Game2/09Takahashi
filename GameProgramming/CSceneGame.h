#ifndef CSCENEGAME_H
#define CSCENEGAME_H
#include "CScene.h"
#include "CXPlayer.h"
#include "CXEnemy.h"
#include "CMap.h"
#include <time.h>
#include "CShadowMap.h"

/*
�Q�[���̃V�[��
*/
class CSceneGame : public CScene {
private:
	//�}�b�v�̃C���X�^���X
	CMap mMap;

	CModel mMap2;

	//�L�����N�^�̃C���X�^���X
	CXPlayer mPlayer;
	//�G�̃C���X�^���X
	CXEnemy mEnemy;

	clock_t start, end;
	bool mCountStart; //�N���A���Ԍv���J�n�p

	//�V���h�E�}�b�v�̃C���X�^���X
	CShadowMap mShadowMap;

public:
	~CSceneGame();
	//�����������̃I�[�o�[���C�h
	void Init();
	//�X�V�����̃I�[�o�[���C�h
	void Update();

	EScene GetNextScene();

	static float mClearTime;	//�N���A�܂łɂ�����������
};

void Render();

#endif
