#pragma once
#include "CCharacter.h"
#include "CCollider.h"

//罠(アイテム)クラス
class CTrap :public CCharacter {
private:
	CCollider mCollider; //コライダ

	bool mEnemyCol; //敵と当たるとtrue
public:
	CTrap();

	void Update();

	void CTrap::Collision(CCollider* m, CCollider* o);
	void CTrap::TaskCollision();

	void SetPos(CVector hpos);	//位置を設定
	void SetRot(CVector hrot);	//回転を設定
};