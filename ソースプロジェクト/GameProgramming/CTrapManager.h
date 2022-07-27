#pragma once
#include "CTrap.h"

//罠管理クラス
class CTrapManager {
private:
	static CTrapManager* mInstance; //罠管理のインスタンス

	std::vector<CTrap*> mTrapList;	//罠格納

	int mMapTrapNum;	//マップ上にある有効な罠の数
public:
	CTrapManager();		//デフォルトコンストラクタ
	~CTrapManager();	//デストラクタ

	static void Generate();	//生成
	static void Release();	//解放
	static CTrapManager* GetInstance();	//インスタンス取得

	void Update();	//更新

	void TrapGenerate(CVector pos, CVector rot); //罠生成

	bool TrapAvailable(); //罠が使用可能な時trueを返す
};