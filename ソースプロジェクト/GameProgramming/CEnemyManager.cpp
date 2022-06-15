#include "CEnemyManager.h"
#include "CRes.h"
#include "CXPlayer.h"
#include "CKey.h"
#include "CXEnemy1.h"
#include "CXEnemy2.h"

#define ATTACK_NUM_MAX 1 //�����ɍU���ł���G�̐�

CEnemyManager* CEnemyManager::mInstance;

CEnemyManager::CEnemyManager()
	:mNearTarget(nullptr)
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

CXEnemy* CEnemyManager::GetNearEnemy()
{
	return mNearTarget;
}

void CEnemyManager::Update()
{
	AIUpdate();

#ifdef _DEBUG
	//�e�X�g�p�A�G����̐�������
	if (CKey::Once('P')) {
		//EnemyGenerate(1, 1, 10);
	}
#endif
}

void CEnemyManager::Render()
{
}

void CEnemyManager::AIUpdate()
{
	//���Z�b�g
	mEnemyDeathNum = 0;
	mEnemyAttackNum = 0;
	float len1 = 10000.0f;

	for (size_t i = 0; i < mEnemyList.size(); i++) {
		if (i == 0) {
			mNearTarget = nullptr;
		}
		mEnemyList[i]->mIsTarget = false;
		//���S��Ԃ�������
		if (mEnemyList[i]->mIsDeath()) {
			mEnemyDeathNum++; //���S��Ԃ̓G�̃J�E���g���Z
			continue; //�ǂݔ�΂�
		}
		//�U����Ԃ������Ƃ��U����Ԃ̓G�̐������Z
		if (mEnemyList[i]->mIsAttack())mEnemyAttackNum++;
		//�v���C���[�Ƃ̋�������ԋ߂��G�����߂�
		CVector pos = mEnemyList[i]->mPosition - CXPlayer::GetInstance()->mPosition;
		float len2 = pos.Length();
		if (len1 > len2) {
			len1 = len2;
			mNearTarget = mEnemyList[i]; //�v���C���[�Ƃ̋�������ԋ߂��G���i�[����
		}
	}

	if (mNearTarget) {
		mNearTarget->mIsTarget = true; //�v���C���[�Ɉ�ԋ߂��G�̍U���Ώۃt���O��true�ɂ���
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



