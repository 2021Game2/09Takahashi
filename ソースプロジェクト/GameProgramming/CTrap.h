#pragma once
#include "CCharacter.h"
#include "CCollider.h"

//�(�A�C�e��)�N���X
class CTrap :public CCharacter {
private:
	CCollider mCollider; //�R���C�_

	bool mEnemyCol; //�G�Ɠ������true
public:
	CTrap();

	void Update();

	void CTrap::Collision(CCollider* m, CCollider* o);
	void CTrap::TaskCollision();

	void SetPos(CVector hpos);	//�ʒu��ݒ�
	void SetRot(CVector hrot);	//��]��ݒ�
};