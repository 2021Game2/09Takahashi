#pragma once
#include "CCharacter.h"
#include "CCollider.h"

//�(�A�C�e��)�N���X
class CTrap :public CCharacter {
private:
	CCollider mCollider; //�R���C�_

	bool mIsEnemyCol; //�G�ƏՓ˂��������f����t���O�A�G�ƏՓ˂��Ă����true
public:
	CTrap();

	void Update(); //�X�V

	void SetPos(CVector hpos);	//�ʒu��ݒ�
	void SetRot(CVector hrot);	//��]��ݒ�

	void SetIsEnemyCol(bool enemycol);	//�G�ƏՓ˂��������f����t���O��ݒ�
	bool GetIsEnemyCol();				//�G�ƏՓ˂��������f����t���O���擾
};