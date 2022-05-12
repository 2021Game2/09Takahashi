#include "CXEnemy.h"
#include "CXPlayer.h"
#include "CUtil.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "CCamera.h"
#include <time.h>
#include "CEffect.h"
#include "CSound.h"
#include "CCollisionManager.h"
#include "CRes.h"
#include "CEnemyManager.h"

#define HP_MAX 150			//�̗͍ő�l
#define DAMAGE_BODY 10		//�_���[�W(��)
#define DAMAGE_HEAD 20		//�_���[�W(��)
#define ATTACK_DIS 4.0f		//�U���\�ɂȂ鋗��
#define SPEED_MOVE 0.05f	//�ʏ�ړ��̃X�s�[�h
#define SPEED_CHASE 0.1f	//�ǐՒ��̈ړ����x
#define SPEED_ATTACK_2 0.2f //�U��2��Ԃ̈ړ����x
#define CHASE_DIS_MAX 15.0f	//�ǐՉ\�ȍő勗��
#define SEARCH_DIS 15.0f	//�ǐՂ��J�n���鋗��
#define STUN_TIME 420		//㩂ɂ����������̃X�^������
#define AVOID_DIS 4.0f		//����\�ɂȂ鋗��
#define GAUGE_WID_MAX 50.0f	//�Q�[�W�̕��̍ő�l

#define FONT "Resource\\FontG.png" //�t�H���g
#define IMAGE_GAUGE "Resource\\Gauge.png" //�Q�[�W�摜
#define IMAGE_TARGET "Resource\\Image_Target.png" //�^�[�Q�b�g�摜

extern CSound SE_Attack_Hit_1;	//�U���q�b�g���̌��ʉ�
extern CSound SE_Knight_Walk;	//�G(�i�C�g)�̕��s���̌��ʉ�
extern CSound SE_Knight_Run;	//�G(�i�C�g)�̑��s���̌��ʉ�

CXEnemy::CXEnemy()
	: mColSphereBody(this, nullptr, CVector(0.5f, -1.0f, 0.0f), 1.2f)
	, mColSphereHead(this, nullptr, CVector(0.0f, 1.f, 0.0f), 1.2f)
	, mColSphereSword0(this, nullptr, CVector(0.7f, 3.5f, -0.2f), 0.8f)
	, mColSphereSword1(this, nullptr, CVector(0.5f, 2.5f, -0.2f), 0.8f)
	, mColSphereSword2(this, nullptr, CVector(0.3f, 1.5f, -0.2f), 0.8f)
	, mHp(HP_MAX)
	, mPoint(0.0f, 0.0f, 0.0f)
	, mPlayerPoint(0.0f, 0.0f, 0.0f)
	, mPlayerDis(0.0f)
	, mMoveDir(0.0f, 0.0f, 0.0f)
	, mRot(0.0f, 0.0f, 0.0f)
	, mSpeed(0.0f)
	, mDot(0.0f)
	, mStunTime(0)
	, mHit(false)
	, mIsTarget(false)
	, mIsInvincible(false)
{
	Init(&CRes::sKnight);

	mTag = EENEMY;	//�G

	mColSphereBody.mTag = CCollider::EBODY;		//�{��
	mColSphereHead.mTag = CCollider::EHEAD;		//��
	mColSphereSword0.mTag = CCollider::ESWORD;	//��
	mColSphereSword1.mTag = CCollider::ESWORD;	//��
	mColSphereSword2.mTag = CCollider::ESWORD;	//��

	mState = EIDLE;	//�ҋ@

	mFont.LoadTexture(FONT, 1, 4096 / 64);

	mImageGauge.Load(IMAGE_GAUGE);

	mImageTarget.Load(IMAGE_TARGET);

	srand((unsigned)time(NULL)); //�����p
}

void CXEnemy::Init(CModelX* model)
{
	CXCharacter::Init(model);
	//�����s��̐ݒ�
	mColSphereBody.mpMatrix = &mpCombinedMatrix[1];
	//��
	mColSphereHead.mpMatrix = &mpCombinedMatrix[1];
	//��
	mColSphereSword0.mpMatrix = &mpCombinedMatrix[26];
	mColSphereSword1.mpMatrix = &mpCombinedMatrix[26];
	mColSphereSword2.mpMatrix = &mpCombinedMatrix[26];
}

void CXEnemy::Update()
{
	//�v���C���[�̍U�����肪�����ɂȂ�ƓG�̖��G�������������
	if (CXPlayer::GetInstance()->mHit == false) {
		mIsInvincible = false;
	}

	//�v���C���[�����̃x�N�g�������߂�
	mPlayerPoint = CXPlayer::GetInstance()->mPosition - mPosition;
	//�v���C���[�܂ł̋��������߂�
	mPlayerDis = mPlayerPoint.Length();

	switch (mState) {
	case EIDLE:	//�ҋ@���
		Idle();	//�ҋ@�������Ă�
		break;

	case EAUTOMOVE:	//�ړ����
		AutoMove();	//�ړ��������Ă�
		break;

	case ECHASE: //�ǐՏ��
		Chase(); //�ǐՏ������Ă�
		break;

	case EATTACK_IDLE: //�U���ҋ@���
		Attack_Idle(); //�U���ҋ@�������Ă�
		break;

	case EATTACK_1:	//�U��1���
		Attack_1();	//�U��1�������Ă�
		break;

	case EATTACK_2: //�U��2���
		Attack_2(); //�U��2�������Ă�
		break;

	case EKNOCKBACK: //�m�b�N�o�b�N���
		KnockBack(); //�m�b�N�o�b�N�������Ă�
		break;

	case EDEATH: //���S���
		Death(); //���S�������Ă�
		break;

	case ESTUN:	//�X�^�����
		Stun();	//�X�^���������Ă�
		break;

	case EAVOID: //������
		Avoid(); //����������Ă�
		break;
	}

	Check tCheck = CUtil::GetCheck2D(mMoveDir.mX, mMoveDir.mZ, 0, 0, mRot.mY);
	float turnspeed = 0.5f;

	mDot = tCheck.dot;

	//�}�ȐU��Ԃ��}��
	if (tCheck.turn > 1.5f) tCheck.turn = 1.5f;

	//�ړ������փL��������������
	if (tCheck.cross > 0.0f) {
		mRot.mY += tCheck.turn * turnspeed;
	}
	if (tCheck.cross < 0.0f) {
		mRot.mY -= tCheck.turn * turnspeed;
	}

	//�ړ�����
	mPosition += mMoveDir * mSpeed;

	//�ړ��������Z�b�g
	mMoveDir = CVector(0.0f, 0.0f, 0.0f);
	//�ړ��X�s�[�h���Z�b�g
	mSpeed = 0.0f;

	//�\����180�x���]���Ă�̂Œ���
	CVector adjust_rot = mRot;
	adjust_rot.mY += M_PI;
	mRotation = (adjust_rot) * (180.0f / M_PI);


	//�̗͂�0�ɂȂ�Ǝ��S��Ԃֈڍs
	if (mHp <= 0) {
		mState = EDEATH;
		mHp = 0;
	}

	CXCharacter::Update();
}

void CXEnemy::Render2D()
{
	//2D�`��J�n
	CUtil::Start2D(0, 800, 0, 600);

	CVector ret;
	CVector tpos = mpCombinedMatrix[6].GetPos();
	Camera.WorldToScreen(&ret, tpos);

	float HpRate = (float)mHp / (float)HP_MAX; //�̗͍ő�l�ɑ΂���A���݂̗̑͂̊���
	float HpGaugeWid = GAUGE_WID_MAX * HpRate; //�̗̓Q�[�W�̕�
	
	//��ʊO�̎��ɕ\�����Ȃ�
	if (ret.mX > 0 && ret.mX < 800) {
		//�Q�[�W�w�i
		mImageGauge.Draw(ret.mX - GAUGE_WID_MAX, ret.mX + GAUGE_WID_MAX, ret.mY + 30.0f, ret.mY + 45.0f, 210, 290, 63, 0);
		//�̗̓Q�[�W
		mImageGauge.Draw(ret.mX - GAUGE_WID_MAX, (ret.mX - GAUGE_WID_MAX) + HpGaugeWid * 2.0f, ret.mY + 30.0f, ret.mY + 45.0f, 0, 0, 0, 0);
		//�v���C���[�̍U���ΏۂɂȂ��Ă���Ƃ�
		if (mIsTarget) {
			//�^�[�Q�b�g�摜�\��
			mImageTarget.Draw(ret.mX - 30.0f, ret.mX + 30.0f, ret.mY - 30.0f, ret.mY + 30.0f, 0, 255, 255, 0);
		}
	}
	//2D�̕`��I��
	CUtil::End2D();
}

void CXEnemy::Collision(CCollider* m, CCollider* o)
{
	//���������S��Ԃ̎��̓��^�[��
	if (mState == EDEATH)return;

	//�����Ƒ��肪���R���C�_�̎�
	if (m->mType == CCollider::ESPHERE && o->mType == CCollider::ESPHERE)
	{
		//����̐e�̃^�O���v���C���[
		if (o->mpParent->mTag == EPLAYER)
		{
			//����̃R���C�_�̃^�O����
			if (o->mTag == CCollider::ESWORD)
			{
				//�L���X�g�ϊ�
				//�v���C���[�̍U���̃q�b�g���肪�L���ȂƂ��A���G�t���O���L���łȂ��Ƃ�
				if (((CXPlayer*)(o->mpParent))->mHit == true && mIsInvincible == false)
				{
					//���R���C�_���m�̏Փ˔���
					if (CCollider::Collision(m, o))
					{
						//�U�����󂯂��ӏ�
						switch (m->mTag) {
						case CCollider::EBODY:	//��
							mHp -= DAMAGE_BODY;	//�_���[�W���󂯂�(��)	
							mIsInvincible = true; //���G�t���O��L���ɂ���
							new CEffect(((CXPlayer*)(o->mpParent))->GetSwordColPos(), 1.0f, 1.0f, "", 3, 5, 2); //�G�t�F�N�g�𐶐�����
							SE_Attack_Hit_1.Play(); //���ʉ����Đ�����
							//�X�^����ԂŖ�����΃m�b�N�o�b�N��Ԃֈڍs
							if (mState != ESTUN && mState != EATTACK_1 && mState != EATTACK_2) {
								mState = EKNOCKBACK;
								mHit = false; //�����̍U���̃q�b�g������I��������
							}
							break;

						case CCollider::EHEAD:	//��
							mHp -= DAMAGE_HEAD;	//�_���[�W���󂯂�(��)
							mIsInvincible = true; //���G�t���O��L���ɂ���
							new CEffect(((CXPlayer*)(o->mpParent))->GetSwordColPos(), 1.5f, 1.5f, "", 3, 5, 2); //�G�t�F�N�g�𐶐�����
							SE_Attack_Hit_1.Play(); //���ʉ����Đ�����
							//�X�^����ԂŖ�����΃m�b�N�o�b�N��Ԃֈڍs
							if (mState != ESTUN && mState != EATTACK_1 && mState != EATTACK_2) {
								mState = EKNOCKBACK;
								mHit = false; //�����̍U���̃q�b�g������I��������
							}
							break;

						default: //��L�ȊO
							break;
						}
					}
				}
			}
		}
		//㩂ɂ���������
		//����̐e�̃^�O���
		if (o->mpParent->mTag == ETRAP)
		{
			//�����̃R���C�_�̃^�O���{��
			if (m->mTag == CCollider::EBODY)
			{
				//���R���C�_���m�̏Փ˔���
				if (CCollider::Collision(m, o))
				{
					//�X�^����ԂłȂ���΁A�X�^����Ԃֈڍs
					if (mState != ESTUN) {
						mState = ESTUN;
						mStunTime = STUN_TIME; //�X�^�����Ԃ�����
						mHit = false; //�U���̃q�b�g����𖳌���
					}
				}
			}
		}
		//�G���m�̂��蔲���h�~
		//����̐e���G�̂Ƃ�
		if (o->mpParent->mTag == EENEMY)
		{
			//�G(����)�����S��Ԃ̂Ƃ����^�[������
			if (((CXEnemy*)(o->mpParent))->mState == CXEnemy::EDEATH)return;
			//�G�̃{�f�B�ɓ������Ă���Ƃ�
			if (m->mTag == CCollider::EBODY && o->mTag == CCollider::EBODY) {
				CVector adjust;
				if (CCollider::CollisionAdjust(m, o, &adjust)) {
					//�G(����)���X�^����Ԃ̂Ƃ�
					if (mState == CXEnemy::ESTUN) {
						//�G(����)�̃|�W�V�����𒲐�
						CXEnemy* Enemy = (CXEnemy*)o->mpParent;
						Enemy->SetPos(Enemy->GetPos() + adjust);
					}
					else {
						//�G(����)�̃|�W�V�����𒲐�
						mPosition -= adjust;
					}
				}
			}
		}
	}

	//���������R���C�_�ő��肪�O�p�R���C�_�̎�
	if (m->mType == CCollider::ESPHERE && o->mType == CCollider::ETRIANGLE)
	{
		//����̐e�̃^�O���}�b�v
		if (o->mpParent->mTag == EMAP)
		{
			//�����̃R���C�_�̃^�O����or��
			if (m->mTag == CCollider::EHEAD || m->mTag == CCollider::EBODY) {
				CVector adjust; //�����p
				//�O�p�R���C�_�Ƌ��R���C�_�̏Փ˔���
				if (CCollider::CollisionTriangleSphere(o, m, &adjust)) {
					//�}�b�v�O�ɏo�Ă����Ȃ��悤�ʒu�𒲐�
					mPosition += adjust;
					//�ړ���Ԃ̂Ƃ�
					if (mState == EAUTOMOVE) {
						//�ǂɌ������ĕ��������Ȃ��悤�ɖڕW�n�_���X�V
						mPoint = mPosition;
						mPoint.mX += -15.0f + (float)(rand() % 30);
						mPoint.mZ += -15.0f + (float)(rand() % 30);
					}
				}
			}
		}
	}
}

void CXEnemy::TaskCollision()
{
	CCollisionManager::Get()->Collision(&mColSphereBody, COLLISIONRANGE);
	CCollisionManager::Get()->Collision(&mColSphereHead, COLLISIONRANGE);
	CCollisionManager::Get()->Collision(&mColSphereSword0, COLLISIONRANGE);
	CCollisionManager::Get()->Collision(&mColSphereSword1, COLLISIONRANGE);
	CCollisionManager::Get()->Collision(&mColSphereSword2, COLLISIONRANGE);
}

//�ҋ@����
void CXEnemy::Idle()
{
	//�ҋ@�A�j���[�V����
	ChangeAnimation(2, true, 200);

	//�v���C���[�����S��Ԃł͖����Ƃ�
	if (CXPlayer::GetInstance()->mState != CXPlayer::EPlayerState::EDEATH) {
		//�v���C���[����苗���܂ŋ߂Â��ƒǐՏ�Ԃֈڍs
		if (mPlayerDis <= SEARCH_DIS) {
			mState = ECHASE;
		}
	}

	//�ҋ@��Ԓ������_���ňړ���Ԃֈڍs
	int random = rand() % 240;
	if (random == 0) {
		//�ڕW�n�_��ݒ�
		mPoint = mPosition;
		mPoint.mX += -15.0f + (float)(rand() % 30);
		mPoint.mZ += -15.0f + (float)(rand() % 30);
		//�ړ���Ԃֈڍs����
		mState = EAUTOMOVE;
	}
}

//�ړ�����
void CXEnemy::AutoMove()
{
	//�ړ��A�j���[�V����
	ChangeAnimation(1, true, 70);

	//�ړ��X�s�[�h��ύX
	mSpeed = SPEED_MOVE;

	//�ړI�n�_�܂ł̃x�N�g�������߂�
	CVector Point = mPoint - mPosition;
	//mMoveDir�ɖڕW�n�_�����̃x�N�g��������
	mMoveDir = Point.Normalize();

	//�v���C���[�����S��Ԃł͖����Ƃ�
	if (CXPlayer::GetInstance()->mState != CXPlayer::EPlayerState::EDEATH) {
		//�v���C���[����苗���܂ŋ߂Â��ƒǐՏ�Ԃֈڍs
		if (mPlayerDis <= SEARCH_DIS) {
			mState = ECHASE;
		}
	}

	//�ڕW�n�_�܂ł̋���
	float length = Point.Length();
	//�ڕW�n�_�t�߂܂ňړ�����Ƒҋ@��Ԃֈڍs
	if (length <= 1.0f) {
		mState = EIDLE;
	}
}

//�ǐՏ���
void CXEnemy::Chase() 
{
	//�ړ��A�j���[�V����
	ChangeAnimation(1, true, 70);

	//�ړ��X�s�[�h��ύX
	mSpeed = SPEED_CHASE;

	//mMoveDir�Ƀv���C���[�����̃x�N�g��������
	mMoveDir = mPlayerPoint.Normalize();

	//�v���C���[���U���\�ȋ����ɂ���Ƃ�
	if (mPlayerDis <= ATTACK_DIS) {
		mState = EATTACK_IDLE; //�U���ҋ@��Ԃֈڍs
	}

	//�v���C���[���ǐՉ\�ȋ����ɂ��Ȃ��Ƃ�
	if (mPlayerDis >= CHASE_DIS_MAX) {
		mState = EIDLE; //�ҋ@��Ԃֈڍs
	}

	int random = 0;
	//�U���\�ȂƂ�
	if (CEnemyManager::GetInstance()->mIsEnemyAttack()) {
		//�ǐՏ�Ԓ��Ƀ����_���ōU��2��Ԃֈڍs
		random = rand() % 360;
		if (random == 0) {
			mState = EATTACK_2;
		}
	}
}

//�U���ҋ@����
void CXEnemy::Attack_Idle()
{
	//�ҋ@�A�j���[�V����
	ChangeAnimation(2, true, 200);

	//mMoveDir�Ƀv���C���[�����̃x�N�g��������
	mMoveDir = mPlayerPoint.Normalize();

	int random = 0;

	//�v���C���[���U���������Ƃ�
	if (CXPlayer::GetInstance()->mAttackFlag_Once == true) {
		//�v���C���[�Ƃ̋���������\�ȋ����̂Ƃ�
		if (mPlayerDis <= AVOID_DIS) {
			//�����_���ŉ�����s�����ǂ������肷��
			random = rand() % 6;
			if (random == 0) {
				mState = EAVOID; //�����Ԃֈڍs
			}
		}
	}

	//�U���\�ȂƂ�
	if (CEnemyManager::GetInstance()->mIsEnemyAttack()) {
		//�v���C���[���U���\�ȋ����ɂ���Ƃ�
		if (mPlayerDis <= ATTACK_DIS) {
			//�����_���ōU�����s�����ǂ����𔻒肷��
			random = rand() % 180;
			if (random == 0) {
				//�����_���ōU���̎�ނ����߂�
				random = rand() % 2;
				switch (random) {
				case 0:
					mState = EATTACK_1; //�U��1��Ԃֈڍs
					break;
				case 1:
					mState = EATTACK_2; //�U��2��Ԃֈڍs
					break;
				}
			}
		}
	}

	//�v���C���[���U���\�ȋ����ɂ��Ȃ��Ƃ�
	if (mPlayerDis >= ATTACK_DIS) {
		//�����_���ŏ�Ԃ��ڍs���邩�ǂ������f����
		random = rand() % 120;
		if (random == 0) {
			//�v���C���[���ǐՉ\�ȋ����ɂ���Ƃ�
			if (mPlayerDis <= CHASE_DIS_MAX) {
				mState = ECHASE; //�ǐՏ�Ԃֈڍs
			}
			else {
				mState = EIDLE; //�ҋ@��Ԃֈڍs
			}
		}
	}
}

//�U��1����
void CXEnemy::Attack_1()
{
	//mMoveDir�Ƀv���C���[�����̃x�N�g��������
	mMoveDir = mPlayerPoint.Normalize();

	ChangeAnimation(7, false, 75);

	if (mAnimationIndex == 7)
	{
		//�q�b�g���蔭��
		if (mAnimationFrame == 30) {
			mHit = true;
		}
		//�q�b�g����I��
		if (mAnimationFrame == 60) {
			mHit = false;
		}
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mState = EIDLE;
		}
	}
}

//�U��2����
void CXEnemy::Attack_2()
{
	//�U��2�A�j���[�V�������ł͂Ȃ��Ƃ��A�U���\�ȋ����܂Ń_�b�V���ŋ߂Â�
	if (mAnimationIndex != 8) {
		//mMoveDir�Ƀv���C���[�����̃x�N�g��������
		mMoveDir = mPlayerPoint.Normalize();
		//�_�b�V���A�j���[�V����
		ChangeAnimation(14, true, 50);
		//�ړ��X�s�[�h��ύX
		mSpeed = SPEED_ATTACK_2;
	}

	//�U���\�ȋ����܂ŋ߂Â��ƍU��
	if (mPlayerDis <= ATTACK_DIS) {
		//�U��2�A�j���[�V����
		ChangeAnimation(8, false, 95);
	}

	if (mAnimationIndex == 8)
	{
		//�q�b�g���蔭��
		if (mAnimationFrame == 30) {
			mHit = true;
		}
		//�q�b�g����I��
		if (mAnimationFrame == 65) {
			mHit = false;
		}
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mState = EIDLE;	//�ҋ@��Ԃֈڍs
		}
	}
}

//�m�b�N�o�b�N����
void CXEnemy::KnockBack()
{
	ChangeAnimation(12, false, 30);
	
	//�m�b�N�o�b�N����
	CVector KnockBackVec = mPlayerPoint.Normalize();
	//�m�b�N�o�b�N��
	float KnockBackAmount = 0.2f;

	//�m�b�N�o�b�N������
	mPosition -= KnockBackVec * KnockBackAmount;

	if (mAnimationIndex == 12)
	{
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mState = EIDLE;	//�ҋ@��Ԃֈڍs
		}
	}
}

//���S����
void CXEnemy::Death()
{
	//���S�A�j���[�V����
	ChangeAnimation(11, false, 60);
}

//�X�^������
void CXEnemy::Stun()
{
	ChangeAnimation(13, true, 80);

	//�X�^�����Ԃ�����������
	mStunTime--;
	//�X�^�����Ԃ�0�ɂȂ�Ƒҋ@��Ԃֈڍs
	if (mStunTime <= 0) {
		mState = EIDLE;
	}
}

//�������
void CXEnemy::Avoid()
{
	//�v���C���[�̂�����������Ȃ�����։��
	mMoveDir = mPlayerPoint.Normalize();
	mSpeed = -0.2f;

	ChangeAnimation(15, false, 40);
	if (mAnimationIndex == 15) {
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			int random = rand() % 2;
			//����ド���_���ōU���\�ȏꍇ�U��2��Ԃֈڍs����
			if (random == 0) {
				//�U���\�ȂƂ�
				if (CEnemyManager::GetInstance()->mIsEnemyAttack()) {
					mState = EATTACK_2;
				}
				else {
					mState = EIDLE;
				}
			}
			else {
				mState = EIDLE; //�ҋ@��Ԃֈڍs
			}
		}
	}
}

//�ʒu��ݒ肷��
void CXEnemy::SetPos(CVector hpos)
{
	mPosition = hpos;
}

//�ʒu���擾����
CVector CXEnemy::GetPos()
{
	return mPosition;
}

//���S��true��Ԃ�
bool CXEnemy::DeathFlag()
{
	return (mState == EDEATH);
}

bool CXEnemy::mIsAttack()
{
	return (mState == EATTACK_1 || mState == EATTACK_2);
}
