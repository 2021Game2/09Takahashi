#ifndef CSCENE_H
#define CSCENE_H

#include "CText.h"
#include "CRes.h"

/*
�V�[���̊��N���X
*/
class CScene {
public:
	enum EScene {
		ETITLE = 0,	//�^�C�g��
		EGAME,		//�Q�[��
		ERESULT,	//���U���g
		ERECORD,	//���R�[�h
	};
	EScene mScene;

	CRes mResource; //���\�[�X
	//CText mFont; //�t�H���g
	//virtual ���z�֐�
	//�|�����[�t�B�Y���̎���
	//=0 �������z�֐��̐ݒ�
	virtual void Init() = 0;	//�������z�֐� ����������
	virtual void Update() = 0; //�������z�֐� �X�V����
	//���̃V�[���̎擾
	virtual EScene GetNextScene() = 0;
	virtual ~CScene() {}	//�f�X�g���N�^�̒�`
};


#endif
