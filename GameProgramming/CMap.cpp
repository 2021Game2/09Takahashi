#include "CMap.h"

CMap::CMap()
{
	mPosition = CVector(0.0f, 1.0f, 0.0f);
	mScale = CVector(1.0f, 1.0f, 1.0f);
	CTransform::Update();

	mModel.Load("sky.obj", "sky.mtl");
	mpModel = &mModel;
}
