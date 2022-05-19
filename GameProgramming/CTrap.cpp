#include "CTrap.h"
#include "CXPlayer.h"
#include "CXEnemy.h"
#include "CRes.h"
#include "CTrapManager.h"
#include "CSound.h"
#include "CCollisionManager.h"

extern CSound SE_Trap_Active;	//罠アイテム作動時の効果音

CTrap* CTrap::mInstance;

CTrap::CTrap()
	:mCollider(this, &mMatrix, CVector(0.0f, 0.0f, 0.0f), 1.0f)
	,mEnemyCol(false)
{
	mTag = ETRAP;	//罠
	mCollider.mTag = CCollider::EBODY;	//本体
	mScale = CVector(7.0f, 7.0f, 7.0f);

	mpModel = &CTrapManager::sTrap;

	mInstance = this;
}

void CTrap::Update()
{
	//当たったらY座標を下げる
	if (mEnemyCol == true) {
		mPosition.mY = -10.0f;
	}
	CTransform::Update();
}

void CTrap::Collision(CCollider* m, CCollider* o)
{
	//自分と相手が球コライダの時
	if (m->mType == CCollider::ESPHERE && o->mType == CCollider::ESPHERE)
	{
		//相手の親のタグが敵の時
		if (o->mpParent->mTag == EENEMY)
		{
			//敵が死亡状態だとリターンする
			if (((CXEnemy*)(o->mpParent))->mState == CXEnemy::EDEATH)return;
			//コライダのタグがボディの時
			if (o->mTag == CCollider::EBODY)
			{
				//敵の状態がスタン状態では無いとき
				if (((CXEnemy*)(o->mpParent))->mState != CXEnemy::ESTUN)
				{
					//球コライダ同士の衝突判定
					if (CCollider::Collision(m, o))
					{
						mEnemyCol = true; //敵に当たったことを返す
						CTrapManager::GetInstance()->mMapTrap = false; //マップ上に罠がない判定にする
						SE_Trap_Active.Play(); //罠アイテム作動時の効果音を再生する
					}
				}
			}
		}
	}
}

void CTrap::TaskCollision()
{
	CCollisionManager::Get()->Collision(&mCollider, COLLISIONRANGE);
}

CTrap* CTrap::GetInstance()
{
	return mInstance;
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

