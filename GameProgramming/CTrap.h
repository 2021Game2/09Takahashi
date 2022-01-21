#pragma once
#include "CCharacter.h"
#include "CCollider.h"

class CTrap :public CCharacter {
private:
	CCollider mCollider;
	CModel mModel;

	bool mEnemyCol; //�G�Ɠ������true
public:
	CTrap();

	void Update();

	void Collision(CCollider* m, CCollider* o);

	static CTrap* mInstance;
	static CTrap* GetInstance();

	void SetPos(CVector hpos);	//�ʒu��ݒ�
	void SetRot(CVector hrot);	//��]��ݒ�
};