#ifndef CMAP_H
#define CMAP_H

#include "CCharacter.h"

//�}�b�v�N���X
class CMap : public CCharacter
{
private:
	static CMap* mInstance; //�}�b�v�̃C���X�^���X
public:
	CMap();		//�f�t�H���g�R���X�g���N�^
	~CMap();	//�f�X�g���N�^

	static void Generate();	//����
	static void Release();	//���
};

#endif