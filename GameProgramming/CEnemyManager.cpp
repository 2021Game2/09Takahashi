#include "CEnemyManager.h"
#include "CRes.h"
#include "CXPlayer.h"
#include "CKey.h"

#define ATTACK_NUM_MAX 1 //�����ɍU���ł���G�̐�

CEnemyManager* CEnemyManager::mInstance;

CEnemyManager::CEnemyManager()
	:mNearTarget(nullptr)
	,mEnemyDeathNum(0)
	,mEnemyAttackNum(0)
{
	CRes::sModelXEnemy.SeparateAnimationSet(0, 10, 80, "walk");//1:�ړ�
	CRes::sModelXEnemy.SeparateAnimationSet(0, 1530, 1830, "idle1");//2:�ҋ@
	CRes::sModelXEnemy.SeparateAnimationSet(0, 10, 80, "walk");//3:�_�~�[
	CRes::sModelXEnemy.SeparateAnimationSet(0, 10, 80, "walk");//4:�_�~�[
	CRes::sModelXEnemy.SeparateAnimationSet(0, 10, 80, "walk");//5:�_�~�[
	CRes::sModelXEnemy.SeparateAnimationSet(0, 10, 80, "walk");//6:�_�~�[
	CRes::sModelXEnemy.SeparateAnimationSet(0, 440, 520, "attack1_1");//7:Attack1
	CRes::sModelXEnemy.SeparateAnimationSet(0, 520, 615, "attack2");//8:Attack2
	CRes::sModelXEnemy.SeparateAnimationSet(0, 10, 80, "walk");//9:�_�~�[
	CRes::sModelXEnemy.SeparateAnimationSet(0, 10, 80, "walk");//10:�_�~�[
	CRes::sModelXEnemy.SeparateAnimationSet(0, 1160, 1260, "death1");//11:�_�E��
	CRes::sModelXEnemy.SeparateAnimationSet(0, 90, 160, "knockback");//12:�m�b�N�o�b�N
	CRes::sModelXEnemy.SeparateAnimationSet(0, 1120, 1160, "stun");//13:�X�^��
	CRes::sModelXEnemy.SeparateAnimationSet(0, 170, 220, "dash");//14:�_�b�V��
	CRes::sModelXEnemy.SeparateAnimationSet(0, 380, 430, "jump");//15:�W�����v
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

void CEnemyManager::EnemyGenerate(int num)
{
	for (int i = 0; i < num; i++) {
		CVector tPos;
		tPos.Set(0, 0, 0);
		tPos.mX = mEnemyStartPos[i].mX;
		tPos.mZ = mEnemyStartPos[i].mZ;

		CXEnemy* tmp = new CXEnemy;
		tmp->SetPos(tPos);
		tmp->Update();
		mEnemyList.push_back(tmp);
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
	//�e�X�g�p�A�G����̐��������
	if (CKey::Once('P')) {
		EnemyGenerate(1);
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



