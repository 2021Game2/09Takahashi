#include "CEnemyManager.h"
#include "CRes.h"
#include "CXPlayer.h"
#include "CKey.h"
#include "CXEnemy1.h"
#include "CXEnemy2.h"

#define ATTACK_NUM_MAX 1 //�����ɍU���ł���G�̐�

CEnemyManager* CEnemyManager::mInstance;

CEnemyManager::CEnemyManager()
	:mTargetEnemy(nullptr)
	,mEnemyDeathNum(0)
	,mEnemyAttackNum(0)
{
}

CEnemyManager::~CEnemyManager()
{
	for (size_t i = 0; i < mEnemyList.size(); i++) {
		delete mEnemyList[i];
	}
}

void CEnemyManager::Generate()
{
	mInstance = new CEnemyManager;
}

void CEnemyManager::Release()
{
	if (mInstance) {
		delete mInstance;
		mInstance = nullptr;
	}
}

CEnemyManager* CEnemyManager::GetInstance()
{
	return mInstance;
}

//�G�̐�������
//�����Ő������A��ށA�̗͂�ݒ肷��
void CEnemyManager::EnemyGenerate(int num, int type, int hp,CVector pos[])
{
	for (int i = 0; i < num; i++) {
		CVector tPos;
		tPos.Set(0, 0, 0);
		tPos.mX = pos[i].mX;
		tPos.mZ = pos[i].mZ;

		//��������G�̎�ނ𔻕�
		switch (type) {
		case CXEnemy::ETYPE_1:	//�^�C�v1
		{
			CXEnemy1* tmp = new CXEnemy1;
			tmp->SetPos(tPos);	//���W�ݒ�
			tmp->SetHp(hp);		//�̗͂�ݒ�
			tmp->Update();		//�X�V
			mEnemyList.push_back(tmp);	//���X�g�ɒǉ�
		}
		break;

		case CXEnemy::ETYPE_2:	//�^�C�v2
		{
			CXEnemy2* tmp = new CXEnemy2;
			tmp->SetPos(tPos);	//���W�ݒ�
			tmp->SetHp(hp);		//�̗͂�ݒ�
			tmp->Update();		//�X�V
			mEnemyList.push_back(tmp);	//���X�g�ɒǉ�
		}
		break;
		}
	}
}

CXEnemy* CEnemyManager::GetTargetEnemy()
{
	return mTargetEnemy;
}

void CEnemyManager::Update()
{
	AIUpdate();
}

void CEnemyManager::Render()
{
}

void CEnemyManager::AIUpdate()
{
	//���Z�b�g
	mEnemyDeathNum = 0;
	mEnemyAttackNum = 0;
	float score1 = -10000.0f;

	for (size_t i = 0; i < mEnemyList.size(); i++) {
		if (i == 0) {
			mTargetEnemy = nullptr;
		}
		mEnemyList[i]->SetIsTarget(false);
		//���S��Ԃ�������
		if (mEnemyList[i]->mIsDeath()) {
			mEnemyDeathNum++; //���S��Ԃ̓G�̃J�E���g���Z
			continue; //�ǂݔ�΂�
		}
		//�U����Ԃ������Ƃ��U����Ԃ̓G�̐������Z
		if (mEnemyList[i]->mIsAttack())mEnemyAttackNum++;

		float score2 = mEnemyList[i]->GetScore();
		if (score1 < score2) {
			score1 = score2;
			mTargetEnemy = mEnemyList[i]; //�X�R�A�������G���i�[����
		}
	}

	if (mTargetEnemy) {
		mTargetEnemy->SetIsTarget(true); //�X�R�A����ԍ����G�̍U���Ώۃt���O��true�ɂ���
	}
}

//�G���S�Ď��S��Ԃ̂Ƃ�true��Ԃ�
bool CEnemyManager::mIsEnemyAllDeath()
{
	return(mEnemyList.size() == mEnemyDeathNum);
}

//�U����Ԃ̓G�̐��������ɍU���ł���G�̐��𒴂��Ă��Ȃ����true��Ԃ�
bool CEnemyManager::mIsEnemyAttack()
{
	return (mEnemyAttackNum < ATTACK_NUM_MAX);
}



