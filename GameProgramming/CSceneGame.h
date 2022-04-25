#ifndef CSCENEGAME_H
#define CSCENEGAME_H
#include "CScene.h"
#include "CXPlayer.h"
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

	clock_t mStartTime, mEndTime; //�v���J�n�����A�v���I������
	bool mCountStart; //�N���A���Ԍv���J�n�p

	//�V���h�E�}�b�v�̃C���X�^���X
	CShadowMap mShadowMap;

	CTexture mImagePlayerRun;		//�v���C���[�̑���������p�摜
	CTexture mImagePlayerAttack;	//�v���C���[�̍U�����@�����p�摜
	CTexture mImagePlayerAvoid;		//�v���C���[�̉����@�����p�摜
	CTexture mImageMouse;			//�}�E�X��������p�摜

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
