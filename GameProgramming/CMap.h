#ifndef CMAP_H
#define CMAP_H

#include "CCharacter.h"

//�}�b�v�N���X
class CMap : public CCharacter
{
private:
	static CMap* mInstance; //�}�b�v�̃C���X�^���X
public:
	CMap();
	~CMap();

	static void Generate();	//����
	static void Release();	//���
};

#endif