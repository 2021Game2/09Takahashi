#include "CXEnemy.h"
#include "CXPlayer.h"

#define HP_MAX 1000	//体力
#define DAMAGE_BODY 10	//ダメージ(体)
#define DAMAGE_HEAD 20	//ダメージ(頭)
#define MARGIN 0.01f
#define ATTACK_DISTANCE 3.0f	//下回ると攻撃が可能になる距離
#define COUNT 90
#define FRONTVEC CVector(0.0f, 0.0f, -1.0f) 
#define SPEED_MOVE 0.05f	//通常移動のスピード
#define SPEED_CHASE 0.1f	//追跡中の移動速度

extern int EHp;	//敵の体力確認用、後で削除

CXEnemy::CXEnemy()
	: mColSphereBody(this, nullptr, CVector(0.5f, -1.0f, 0.0f), 1.0f)
	, mColSphereHead(this, nullptr, CVector(0.0f, 1.f, 0.0f), 1.0f)
	, mColSphereSword0(this, nullptr, CVector(0.7f, 3.5f, -0.2f), 0.5f)
	, mColSphereSword1(this, nullptr, CVector(0.5f, 2.5f, -0.2f), 0.5f)
	, mColSphereSword2(this, nullptr, CVector(0.3f, 1.5f, -0.2f), 0.5f)
	, mColSearch(this, &mMatrix, CVector(0.0f, 0.0f, 0.0f), 15.0f)
	, mHp(HP_MAX)
	, mPlayerPosition(0.0f, 0.0f, 0.0f)
	,mPoint(0.0f, 0.0f, 0.0f)
	,mCount(COUNT)
{
	mTag = EENEMY;

	mColSphereBody.mTag = CCollider::EBODY;
	mColSphereHead.mTag = CCollider::EHEAD;
	mColSphereSword0.mTag = CCollider::ESWORD;
	mColSphereSword1.mTag = CCollider::ESWORD;
	mColSphereSword2.mTag = CCollider::ESWORD;

	mColSearch.mTag = CCollider::ESEARCH;

	mState = EIDLE;	//待機

	mPoint = mPosition * mMatrixRotate;
}

void CXEnemy::Init(CModelX* model)
{
	CXCharacter::Init(model);
	//合成行列の設定
	mColSphereBody.mpMatrix = &mpCombinedMatrix[1];
	//頭
	mColSphereHead.mpMatrix = &mpCombinedMatrix[1];
	//剣
	mColSphereSword0.mpMatrix = &mpCombinedMatrix[26];
	mColSphereSword1.mpMatrix = &mpCombinedMatrix[26];
	mColSphereSword2.mpMatrix = &mpCombinedMatrix[26];

	/*
	mColSphereBody.mpMatrix = &mpCombinedMatrix[8];
	//頭
	mColSphereHead.mpMatrix = &mpCombinedMatrix[11];
	//剣
	mColSphereSword.mpMatrix = &mpCombinedMatrix[21];
	*/
}

void CXEnemy::Update()
{
	switch (mState) {
	case EIDLE:	//待機状態
		Idle();	//待機処理を呼ぶ
		break;

	case EMOVE:	//移動状態
		Move();	//移動処理を呼ぶ
		break;

	case ECHASE: //追跡状態
		Chase(); //追跡処理を呼ぶ
		break;

	case EATTACK_1:	//攻撃状態
		Attack_1();	//攻撃1処理を呼ぶ
		if (mAttackFlag_1 == false) {
			mState = EIDLE;
		}
		break;

	case EDEATH: //死亡状態
		Death(); //死亡処理を呼ぶ
		break;
	}

	mCount--;

	if (mHp <= 0) {
		mState = EDEATH;
	}

	if (mHp < 0) mHp = 0;

	EHp = mHp;

	CXCharacter::Update();
}

void CXEnemy::Collision(CCollider* m, CCollider* o)
{
	if (m->mType == CCollider::ESPHERE)
	{
		if (o->mType == CCollider::ESPHERE)
		{
			if (o->mpParent->mTag == EPLAYER)
			{
				if (o->mTag == CCollider::ESWORD)
				{
					if (CCollider::Collision(m, o))
					{
						//キャスト変換
						//プレイヤーの攻撃を受けた時
						if (((CXPlayer*)(o->mpParent))->mHit == true)
						{
							switch (m->mTag) {
							case CCollider::EBODY:
								mHp -= DAMAGE_BODY;
								break;
							case CCollider::EHEAD:
								mHp -= DAMAGE_HEAD;
								break;
							default:
								break;
							}
						}
					}
				}
				if (m->mTag == CCollider::ESEARCH) {
					if (o->mTag == CCollider::EHEAD || o->mTag == CCollider::EBODY) {
						if (CCollider::Collision(m, o)) {
							mPlayerPosition = o->mpParent->mPosition;
							//mState = ECHASE;
						}
					}
				}
			}
		}
	}
}

//待機処理
void CXEnemy::Idle()
{
	//待機アニメーション
	ChangeAnimation(2, true, 200);	

	if (mCount == 0) {
		int random = rand() % 2;
		switch (random) {
		case 0:
			//
			break;

		case 1:
			//移動状態へ移行する
			mState = EMOVE;
			mPoint = mPosition * CMatrix().RotateY(rand() % 360);
			break;
		}
		mCount = COUNT;
	}
}

//移動処理
void CXEnemy::Move()
{
	//移動アニメーション
	ChangeAnimation(1, true, 70);

	//左向き(X軸)のベクトルを求める
	CVector vx = CVector(1.0f, 0.0f, 0.0f) * mMatrixRotate;

	//目的地点までのベクトルを求める
	CVector vp = mPoint - mPosition;
	float dx = vp.Dot(vx); //左ベクトルとの内積を求める
	//左右方向へ回転
	if (dx > MARGIN)
	{
		mRotation.mY += 2.0f; //左へ回転
	}
	else if (dx < -MARGIN)
	{
		mRotation.mY -= 2.0f; //右へ回転
	}
	//移動する
	mPosition += (FRONTVEC * SPEED_MOVE) * mMatrixRotate;

	if (mCount == 0) {
		int random = rand() % 2;
		switch (random) {
		case 0:
			//目標地点更新
			mPoint = mPosition * CMatrix().RotateY(rand() % 360);
			break;

		case 1:
			//待機状態へ移行する
			mState = EIDLE;
			break;
		}
		mCount = COUNT;
	}
}

//追跡処理
void CXEnemy::Chase() 
{
	//移動アニメーション
	ChangeAnimation(1, true, 70);

	CVector SideVec = CVector(1.0f, 0.0f, 0.0f) * mMatrixRotate;
	SideVec.Normalize();

	CVector PlayerPoint = mPlayerPosition - mPosition;

	float DegreeX = PlayerPoint.Dot(SideVec);
	if (DegreeX > MARGIN)
	{
		mRotation.mY -= 2.0f;
	}
	else if (DegreeX < -MARGIN)
	{
		mRotation.mY += 2.0f;
	}

	float length = PlayerPoint.Length();
	if (length > ATTACK_DISTANCE) {
		mPosition += (FRONTVEC * SPEED_CHASE) * mMatrixRotate;
	}
	else{
		if (-1.5f < DegreeX && DegreeX < 1.5f)
		{
			mState = EATTACK_1;
		}
	}
}

//攻撃1処理
void CXEnemy::Attack_1()
{
	if (mAttackFlag_1 == false) {
		mAttackFlag_1 = true;
		ChangeAnimation(7, true, 80);
	}

	if (mAnimationIndex == 7)
	{
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mAttackFlag_1 = false;
		}
	}

	CVector SideVec = CVector(1.0f, 0.0f, 0.0f) * mMatrixRotate;
	SideVec.Normalize();

	CVector Point = mPlayerPosition - mPosition;

	float DegreeX = Point.Dot(SideVec);

	if (DegreeX > MARGIN)
	{
		mRotation.mY -= 1.0f;
	}
	else if (DegreeX < -MARGIN)
	{
		mRotation.mY += 1.0f;
	}
}

//死亡処理
void CXEnemy::Death()
{
	//死亡アニメーション
	ChangeAnimation(11, false, 60);
}