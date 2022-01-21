#pragma once
#include "CCharacter.h"
#include "CCollider.h"

class CTrap :public CCharacter {
private:
	static CTrap* mInstance;
	CCollider mCollider;
	CModel mModel;
public:
	CTrap();

	void Update();

	void Collision(CCollider* m, CCollider* o);

	static CTrap* GetInstance();

	void SetPos(CVector hpos);	//ˆÊ’u‚ğİ’è
	void SetRot(CVector hrot);	//‰ñ“]‚ğİ’è
};