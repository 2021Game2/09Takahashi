#pragma once
#include "CCharacter.h"
#include "CCollider.h"

//�(�A�C�e��)�N���X
class CTrap :public CCharacter {
private:
	CCollider mCollider;
	CModel mModel;

	bool mEnemyCol; //�G�Ɠ������true
public:
	CTrap();

	void Update();

	void CTrap::Collision(CCollider* m, CCollider* o);
	void CTrap::TaskCollision();

	static CTrap* mInstance;
	static CTrap* GetInstance();

	void SetPos(CVector hpos);	//�ʒu��ݒ�
	void SetRot(CVector hrot);	//��]��ݒ�
};