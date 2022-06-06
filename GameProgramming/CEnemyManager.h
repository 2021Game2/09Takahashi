#pragma once
#include "CXEnemy.h"

#define ENEMY_GENERATE_NUM 3 //�G�̐�����

#define ENEMY_START_POS CVector(0.0f, 0.0f, -10.0f),CVector(10.0f, 0.0f, 0.0f),CVector(-10.0f, 0.0f, 0.0f) //�G�̏������W1

//�G���Ǘ�����N���X
class CEnemyManager
{
private:
	static CEnemyManager* mInstance; //�G�Ǘ��̃C���X�^���X

	std::vector<CXEnemy*> mEnemyList; //�G�i�[

	CXEnemy* mNearTarget; //��ԋ߂��G���i�[

	int mEnemyDeathNum; //���S��Ԃ̓G�̐�

	int mEnemyAttackNum; //�U����Ԃ̓G�̐�

	CVector mEnemyStartPos[ENEMY_GENERATE_NUM] = { ENEMY_START_POS }; //�G�̏������W
public:
	CEnemyManager();
	~CEnemyManager();

	static void Generate(); //����
	static void Release(); //�j��
	static CEnemyManager* GetInstance(); //�C���X�^���X�擾

	void EnemyGenerate(int num, int type, int hp); //�G�����A�����Ő������ƓG�̎�ނƏ����̗�(�ȗ���)���w�肷��
	CXEnemy* GetNearEnemy(); //��ԋ߂��G���擾����

	void Update();
	void Render();

	void AIUpdate(); //AI�X�V

	bool mIsEnemyAllDeath(); //�G���S�Ď��S��ԂɂȂ��true��Ԃ�

	bool mIsEnemyAttack(); //�G���U���\�ȂƂ�true��Ԃ�
};