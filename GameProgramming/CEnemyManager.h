#pragma once
#include "CXEnemy.h"

class CEnemyManager 
{
private:
	static CEnemyManager* mInstance; //�C���X�^���X

	std::vector<CXEnemy*> mEnemyList; //�i�[

	CXEnemy* mNearTarget; //��ԋ߂��G���i�[

	int mEnemyDeathNum; //���S��Ԃ̓G�̐�
public:
	CEnemyManager();
	~CEnemyManager();

	static void Generate(); //����
	static void Release(); //�j��
	static CEnemyManager* GetInstance(); //�C���X�^���X�擾

	void EnemyGenerate(int num); //�G�����A�����Ő��������w�肷��
	CXEnemy* GetNearEnemy(); //��ԋ߂��G���擾����

	void Update();
	void Render();

	void AIUpdate();

	bool mIsEnemyAllDeath(); //�G���S�Ď��S��ԂɂȂ��true��Ԃ�
};