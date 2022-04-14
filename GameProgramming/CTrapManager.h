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

	void TrapGenerate(CVector pos, CVector rot); //罠生成

	int mTrapQuantity;		//罠の所持数

	bool mMapTrap;			//マップ上に罠があるとtrue

	bool TrapAvailable(); //罠が使用可能な時trueを返す
};