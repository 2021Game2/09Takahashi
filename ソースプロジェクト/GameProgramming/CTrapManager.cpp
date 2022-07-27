#include "CTrapManager.h"
#include "CTaskManager.h"
#include "CXPlayer.h"

#define MAP_TRAP_MAX 3 //�}�b�v��ɐݒu�ł���㩂̏��

CTrapManager* CTrapManager::mInstance;

CTrapManager::CTrapManager()
	:mMapTrapNum(0)
{
}

CTrapManager::~CTrapManager()
{
	for (size_t i = 0; i < mTrapList.size(); i++) {
		delete mTrapList[i];
	}

	//�v���C���[�̃C���X�^���X������Ƃ�
	if (CXPlayer::GetInstance()) {
		//�}�b�v��ɗL����㩂�����Ƃ�
		if (mMapTrapNum > 0) {
			//㩂̏����������Z
			CXPlayer::GetInstance()->SetTrapQuantity(mMapTrapNum, true);
		}
	}
}

void CTrapManager::Generate()
{
	mInstance = new CTrapManager;
}

void CTrapManager::Release()
{
	if (mInstance) {
		delete mInstance;
		mInstance = NULL;
	}
}

CTrapManager* CTrapManager::GetInstance()
{
	return mInstance;
}

void CTrapManager::Update()
{
	//���Z�b�g
	mMapTrapNum = 0;

	for (size_t i = 0; i < mTrapList.size(); i++) {
		//�G�ƏՓ˂��������f����t���O��false
		if (mTrapList[i]->GetIsEnemyCol() == false) {
			//�}�b�v��̗L����㩂̐����J�E���g�A�b�v
			mMapTrapNum++;
		}
	}
}

void CTrapManager::TrapGenerate(CVector pos, CVector rot)
{
	//㩐���
	CTrap* trap = new CTrap;
	trap->SetPos(pos);
	trap->SetRot(rot);
	trap->Update();
	mTrapList.push_back(trap);
}

bool CTrapManager::TrapAvailable()
{
	//㩂̏�������0�������Ă���Ƃ��A�}�b�v��̗L����㩂̐����ݒu�ł������𒴂��Ă��Ȃ��Ƃ�
	return (CXPlayer::GetInstance()->GetTrapQuantity() > 0 && mMapTrapNum < MAP_TRAP_MAX);
}
