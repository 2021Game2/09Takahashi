#pragma once
#include "CScene.h"

//���R�[�h��ʂ̃V�[���N���X
class CSceneRecord :public CScene {
private:
	CTexture mImageBackGround; //�w�i�摜

public:
	CSceneRecord();
	void Init();
	void Update();

	EScene GetNextScene();
};