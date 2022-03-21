#include "CCharacter.h"
#include "CTaskManager.h"

//描画処理
void CCharacter::Render()
{
	mpModel->Render(mMatrix);
}

CCharacter::~CCharacter() {
	//タスクリストから削除
	CTaskManager::Get()->Remove(this);
}

CCharacter::CCharacter()
:mTag(EZERO)
{
	//タスクリストに追加
	CTaskManager::Get()->Add(this);
}

void CCharacter::ChangePriority(int priority)
{
	mPriority = priority;
	CTaskManager::Get()->Remove(this);
	CTaskManager::Get()->Add(this);
}

