#pragma once
#include "CCharacter.h"
#include "CCollider.h"

//罠(アイテム)クラス
class CTrap :public CCharacter {
private:
	CCollider mCollider; //コライダ

	bool mIsEnemyCol; //敵と衝突したか判断するフラグ、敵と衝突しているとtrue
public:
	CTrap();

	void Update(); //更新

	void SetPos(CVector hpos);	//位置を設定
	void SetRot(CVector hrot);	//回転を設定

	void SetIsEnemyCol(bool enemycol);	//敵と衝突したか判断するフラグを設定
	bool GetIsEnemyCol();				//敵と衝突したか判断するフラグを取得
};