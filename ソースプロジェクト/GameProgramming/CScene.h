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
		EGAME2,		//�Q�[��2
		ERESULT,	//���U���g
		ERECORD,	//���R�[�h
	};
	EScene mScene = ETITLE;

	CRes mResource; //���\�[�X
	//CText mFont; //�t�H���g
	//virtual ���z�֐�
	//�|�����[�t�B�Y���̎���
	//=0 �������z�֐��̐ݒ�
	virtual void Init() = 0;	//�������z�֐� ����������
	virtual void Update() = 0; //�������z�֐� �X�V����
	virtual void Render() = 0; //�������z�֐� �`�揈��
	//���̃V�[���̎擾
	virtual EScene GetNextScene() = 0;
	virtual ~CScene() {}	//�f�X�g���N�^�̒�`
};


#endif
