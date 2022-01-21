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

	void SetPos(CVector hpos);	//�ʒu��ݒ�
	void SetRot(CVector hrot);	//��]��ݒ�
};