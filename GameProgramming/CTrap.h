#pragma once
#include "CCharacter.h"
#include "CCollider.h"

class CTrap :public CCharacter {
private:
	CCollider mCollider;
	CModel mModel;

	bool mEnemyCol; //敵と当たるとtrue
public:
	CTrap();

	void Update();

	void CTrap::Collision(CCollider* m, CCollider* o);
	void CTrap::TaskCollision();

	static CTrap* mInstance;
	static CTrap* GetInstance();

	void SetPos(CVector hpos);	//位置を設定
	void SetRot(CVector hrot);	//回転を設定
};