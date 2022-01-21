#pragma once
#include "CCharacter.h"
#include "CCollider.h"

class CTrap :public CCharacter {
private:
	CCollider mCollider;
	CModel mModel;

	bool mEnemyCol; //“G‚Æ“–‚½‚é‚Ætrue
public:
	CTrap();

	void Update();

	void Collision(CCollider* m, CCollider* o);

	static CTrap* mInstance;
	static CTrap* GetInstance();

	void SetPos(CVector hpos);	//ˆÊ’u‚ğİ’è
	void SetRot(CVector hrot);	//‰ñ“]‚ğİ’è
};