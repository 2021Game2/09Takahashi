#ifndef CSCENEGAME_H
#define CSCENEGAME_H
#include "CScene.h"
#include "CXPlayer.h"
#include "CXEnemy.h"
#include "CMap.h"

/*
�Q�[���̃V�[��
*/
class CSceneGame : public CScene {
private:
	//�}�b�v�̃C���X�^���X
	CMap mMap;
	//�L�����N�^�̃C���X�^���X
	CXPlayer mPlayer;
	//�G�̃C���X�^���X
	CXEnemy mEnemy;

	int mTime;
	int mFrameCount;

public:
	~CSceneGame();
	//�����������̃I�[�o�[���C�h
	void Init();
	//�X�V�����̃I�[�o�[���C�h
	void Update();

	EScene GetNextScene();
};

#endif
