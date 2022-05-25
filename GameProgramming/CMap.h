#ifndef CMAP_H
#define CMAP_H

#include "CCharacter.h"

//マップクラス
class CMap : public CCharacter
{
private:
	static CMap* mInstance; //マップのインスタンス
public:
	CMap();
	~CMap();

	static void Generate();	//生成
	static void Release();	//解放
};

#endif