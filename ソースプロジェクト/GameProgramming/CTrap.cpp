#include "CTrap.h"
#include "CXPlayer.h"
#include "CXEnemy.h"
#include "CRes.h"
#include "CTrapManager.h"

#define SCALE CVector(7.0f,7.0f,7.0f) //スケール

CTrap::CTrap()
	:mCollider(this, &mMatrix, CVector(0.0f, 0.0f, 0.0f), 1.0f)
	,mIsEnemyCol(false)
{
	//タグを設定
	mTag = ETRAP;	//罠
	mCollider.mTag = CCollider::EBODY;	//本体

	mScale = SCALE; //スケールを設定

	mpModel = &CRes::sModelTrap; //モデルを設定
}

void CTrap::Update()
{
	//敵と当たっていたらY座標を下げる
	if (mIsEnemyCol == true) {
		mPosition.mY = -10.0f;
	}
	CTransform::Update();
}

//位置を設定
void CTrap::SetPos(CVector hpos)
{
	mPosition = hpos;
}

//回転を設定　
void CTrap::SetRot(CVector hrot)
{
	mRotation = hrot;
}

//敵と衝突したか判断するフラグを設定
void CTrap::SetIsEnemyCol(bool enemycol)
{
	mIsEnemyCol = enemycol;
}

//敵と衝突したか判断するフラグを取得
bool CTrap::GetIsEnemyCol()
{
	return mIsEnemyCol;
}

