#pragma once
#include "CTrap.h"
#include "CModel.h"

class CTrapManager {
private:
	static CTrapManager* mInstance;

	std::vector<CTrap*> mTrapList;
public:
	CTrapManager();
	~CTrapManager();

	static void Generate();
	static void Release();
	static CTrapManager* GetInstance();

	void Update();

	static CModel sTrap;

	void TrapGenerate(CVector pos, CVector rot); //ã©¶¬

	int mTrapQuantity;		//ã©‚ÌŠ”
	bool mMapTrap;			//ƒ}ƒbƒvã‚Éã©‚ª‚ ‚é‚Ætrue
};