#pragma once
#include "CScene.h"

//���R�[�h��ʂ̃V�[���N���X
class CSceneRecord :public CScene {
public:
	CSceneRecord();

	void Init();
	void Update();

	EScene GetNextScene(); //���̃V�[�����擾
};