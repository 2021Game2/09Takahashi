#pragma once
#include "CTrap.h"

//罠管理クラス
class CTrapManager {
private:
	static CTrapManager* mInstance; //罠管理のインスタンス

	std::vector<CTrap*> mTrapList; //罠格納
public:
	CTrapManager();
	~CTrapManager();

	static void Generate();	//生成
	static void Release();	//解放
	static CTrapManager* GetInstance();	//インスタンス取得

	void Update();

	void TrapGenerate(CVector pos, CVector rot); //罠生成

	int mTrapQuantity;		//罠の所持数

	bool mMapTrap;			//マップ上に罠があるとtrue

	bool TrapAvailable(); //罠が使用可能な時trueを返す
};