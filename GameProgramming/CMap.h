#ifndef CMAP_H
#define CMAP_H

#include "CCharacter.h"

//�}�b�v�N���X
class CMap : public CCharacter
{
private:
	CModel mModel;

	static CMap* mInstance;
public:
	CMap();
	~CMap();

	static void Generate();
	static void Release();
};

#endif