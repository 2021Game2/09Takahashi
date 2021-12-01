#pragma once
#include "CXCharacter.h"
#include "CCollider.h"

class CTrap :public CXCharacter {
private:
	static CTrap* mInstance;
	CCollider mCollider;
public:
	CTrap();

	void Init(CModelX* model);

	void Update();

	void Collision(CCollider* m, CCollider* o);

	static CTrap* GetInstance();

	void SetPos(CVector hpos);	//ˆÊ’u‚ğİ’è
};