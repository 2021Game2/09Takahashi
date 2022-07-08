#include "CTrapManager.h"
#include "CTaskManager.h"
#include "CXPlayer.h"

CTrapManager* CTrapManager::mInstance;

CTrapManager::CTrapManager()
	:mMapTrap(false)
{
}

CTrapManager::~CTrapManager()
{
	for (size_t i = 0; i < mTrapList.size(); i++) {
		delete mTrapList[i];
	}
	//�}�b�v���㩂��c���Ă�����
	if (mMapTrap == true) {
		//㩂̏����������Z
		CXPlayer::GetInstance()->SetTrapQuantity(1, true);
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
	/*
	for (size_t i = 0; i < mTrapList.size(); i++) {
		mTrapList[i]->Update();
	}
	*/
}

void CTrapManager::TrapGenerate(CVector pos, CVector rot)
{
	//㩐���
	CTrap* trap = new CTrap;
	trap->SetPos(pos);
	trap->SetRot(rot);
	trap->Update();
	mMapTrap = true;
	mTrapList.push_back(trap);
}

void CTrapManager::SetMapTrapFlag(bool flag)
{
	mMapTrap = flag;
}

bool CTrapManager::TrapAvailable()
{
	//㩂̏�������0�������Ă���Ƃ��A�}�b�v��Ƀg���b�v���u����Ă��Ȃ��Ƃ�true��Ԃ�
	return (CXPlayer::GetInstance()->GetTrapQuantity() > 0 && mMapTrap == false);
}
