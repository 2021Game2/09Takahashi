#ifndef CSCENEGAME_H
#define CSCENEGAME_H
#include "CScene.h"
#include "CXPlayer.h"
#include "CMap.h"
#include <time.h>
#include "CShadowMap.h"

//�Q�[���̃V�[���N���X
class CSceneGame : public CScene {
private:
	clock_t mStartTime, mEndTime; //�v���J�n�����A�v���I������
	bool mCountStart; //�N���A���Ԍv���J�n�p

	//�V���h�E�}�b�v�̃C���X�^���X
	CShadowMap mShadowMap;



public:
	CSceneGame();
	~CSceneGame();
	//�����������̃I�[�o�[���C�h
	void Init();
	//�X�V�����̃I�[�o�[���C�h
	void Update();

	EScene GetNextScene();

	static float mClearTime;	//�N���A�܂łɂ�����������
};

//�O���֐�
void Render();

#endif
