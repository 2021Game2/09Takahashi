#pragma once
#include "CScene.h"

//���U���g��ʂ̃V�[���N���X
class CSceneResult :public CScene {
private:
	int mNewRecord;	//�V�L�^�̏��ʂ��L�^

	int count; //�����_�ŗp�J�E���g
public:
	CSceneResult();

	void Init();
	void Update();

	EScene GetNextScene(); //���̃V�[�����擾

	static float record[6];	//�N���A���Ԃ̋L�^�p
};