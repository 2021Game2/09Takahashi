#pragma once
#include "CTrap.h"

//罠管理クラス
class CTrapManager {
private:
	static CTrapManager* mInstance; //罠管理のインスタンス

	std::vector<CTrap*> mTrapList; //罠格納

	bool mMapTrap;			//マップ上に罠があるとtrue
public:
	CTrapManager();		//デフォルトコンストラクタ
	~CTrapManager();	//デストラクタ

	static void Generate();	//生成
	static void Release();	//解放
	static CTrapManager* GetInstance();	//インスタンス取得

	void Update();	//更新

	void TrapGenerate(CVector pos, CVector rot); //罠生成

	void SetMapTrapFlag(bool flag); //マップ上に罠があるか判断するフラグを設定

	bool TrapAvailable(); //罠が使用可能な時trueを返す
};