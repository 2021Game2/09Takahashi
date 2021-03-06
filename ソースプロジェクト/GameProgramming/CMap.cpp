#include "CMap.h"
#include "CRes.h"

CMap* CMap::mInstance;

CMap::CMap()
{
	mPosition = CVector(0.0f, 1.0f, 0.0f);
	mScale = CVector(1.0f, 1.0f, 1.0f);
	CTransform::Update();

	mpModel = &CRes::sModelMap;
}

CMap::~CMap()
{
}

void CMap::Generate()
{
	mInstance = new CMap;
}

void CMap::Release()
{
	if (mInstance) {
		delete mInstance;
		mInstance = nullptr;
	}
}
