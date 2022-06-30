#include "CXEnemy.h"
#include "CXPlayer.h"
#include "CUtil.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "CCamera.h"
#include <windows.h>
#include "CEffect.h"
#include "CCollisionManager.h"
#include "CRes.h"
#include "CEnemyManager.h"

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
#define CHASE_RESTART_TIME 60 //�ǐՂ��ĊJ����܂ł̎���

CXEnemy::CXEnemy()
	: mHp(0)
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
	, mChaseRestartCount(0)
	, mEnemyType(ETYPE_1)
{
	Init(&CRes::sModelXEnemy);

	//�^�O��ݒ�
	mTag = EENEMY;	//�G

	//������Ԃ�ݒ�
	mState = EIDLE;	//�ҋ@���

	srand(timeGetTime()); //�����p
}

void CXEnemy::Init(CModelX* model)
{
	CXCharacter::Init(model);
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

	//��Ԃ𔻕�
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

	//�G�̎�ނ��^�C�v1�̂Ƃ�
	if (mEnemyType == ETYPE_1) {
		Check tCheck = CUtil::GetCheck2D(mMoveDir.mX, mMoveDir.mZ, 0, 0, mRot.mY);
		float turnspeed = 0.5f;	//��]���x

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
		//�\����180�x���]���Ă�̂Œ���
		CVector adjust_rot = mRot;
		adjust_rot.mY += M_PI;
		mRotation = (adjust_rot) * (180.0f / M_PI);
	}
	//����ȊO
	else {
		//���ʂ�3�����x�N�g���v�Z�ŎZ�o�����ق������m�����v�Z�ʂ����O����ꍇ�͋[���v�Z�Ōy�ʉ�
		//�[���x�N�g���v�Z
		Check tCheck = CUtil::GetCheck2D(mMoveDir.mX, mMoveDir.mZ, 0, 0, mRotation.mY * (M_PI / 180.0f));

		//��]���x�@degree�ɒ���
		float Turnspeed = (180.0f / M_PI) * 0.5f;

		//�}�ȐU��Ԃ��}��
		if (tCheck.turn > 1.5f) tCheck.turn = 1.5f;

		//�ړ������փL��������������
		if (tCheck.cross > 0.0f) {
			mRotation.mY += tCheck.turn * Turnspeed;
		}
		if (tCheck.cross < 0.0f) {
			mRotation.mY -= tCheck.turn * Turnspeed;
		}
	}

	//�ړ�����
	mPosition += mMoveDir * mSpeed;

	//�ړ��������Z�b�g
	mMoveDir = CVector(0.0f, 0.0f, 0.0f);
	//�ړ��X�s�[�h���Z�b�g
	mSpeed = 0.0f;

	//�̗͂�0�ɂȂ�Ǝ��S��Ԃֈڍs
	if (mHp <= 0) {
		mState = EDEATH;
		mHp = 0;
	}

	CXCharacter::Update();
}

void CXEnemy::Render2D()
{
	//���������S��Ԃ̎����^�[������
	if (mState == EDEATH)return;

	//2D�`��J�n
	CUtil::Start2D(0, 800, 0, 600);

	CVector ret;
	CVector tpos;
	//�G�̎�ނ𔻒f
	switch (mEnemyType) {
	case ETYPE_1:
		tpos = mpCombinedMatrix[6].GetPos();
		break;

	case ETYPE_2:
		tpos = mpCombinedMatrix[30].GetPos();
		break;
	}
	Camera.WorldToScreen(&ret, tpos);

	float HpRate = (float)mHp / (float)mHpMax; //�̗͍ő�l�ɑ΂���A���݂̗̑͂̊���
	float HpGaugeWid = GAUGE_WID_MAX * HpRate; //�̗̓Q�[�W�̕�
	
	//��ʊO�̎��ɕ\�����Ȃ�
	if (ret.mX > 0 && ret.mX < 800) {
		//�Q�[�W�w�i
		CRes::sImageGauge.Draw(ret.mX - GAUGE_WID_MAX, ret.mX + GAUGE_WID_MAX, ret.mY + 30.0f, ret.mY + 45.0f, 210, 290, 63, 0);
		//�̗̓Q�[�W
		CRes::sImageGauge.Draw(ret.mX - GAUGE_WID_MAX, (ret.mX - GAUGE_WID_MAX) + HpGaugeWid * 2.0f, ret.mY + 30.0f, ret.mY + 45.0f, 0, 0, 0, 0);
		//�v���C���[�̍U���ΏۂɂȂ��Ă���Ƃ�
		if (mIsTarget) {
			//�^�[�Q�b�g�摜�\��
			CRes::sImageTarget.Draw(ret.mX - 30.0f, ret.mX + 30.0f, ret.mY - 30.0f, ret.mY + 30.0f, 0, 255, 255, 0);
		}
		//�X�^����Ԃ̂Ƃ�
		if (mState == ESTUN) {
			//�X�^���摜�\��
			CRes::sImageStun.Draw(ret.mX - 20.0f, ret.mX + 20.0f, ret.mY + 45.0f, ret.mY + 85.0f, 0, 255, 255, 0);
		}
	}
	//2D�̕`��I��
	CUtil::End2D();
}

void CXEnemy::Collision(CCollider* m, CCollider* o)
{
	//���������S��Ԃ̎��̓��^�[��
	if (mState == EDEATH)return;

	//����̐e�������̎��̓��^�[��
	if (o->mpParent == this)return;

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
							CRes::sSEAttackHit1.Play(); //���ʉ����Đ�����
							//�m�b�N�o�b�N���\�ȂƂ�
							if (mIsKnockBack()) {
								mState = EKNOCKBACK; //�m�b�N�o�b�N��Ԃֈڍs
								mHit = false; //�����̍U���̃q�b�g������I��������
							}
							break;

						case CCollider::EHEAD:	//��
							mHp -= DAMAGE_HEAD;	//�_���[�W���󂯂�(��)
							mIsInvincible = true; //���G�t���O��L���ɂ���
							new CEffect(((CXPlayer*)(o->mpParent))->GetSwordColPos(), 1.5f, 1.5f, "", 3, 5, 2); //�G�t�F�N�g�𐶐�����
							CRes::sSEAttackHit1.Play(); //���ʉ����Đ�����
							//�m�b�N�o�b�N���\�ȂƂ�
							if (mIsKnockBack()) {
								mState = EKNOCKBACK; //�m�b�N�o�b�N��Ԃֈڍs
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
				//�X�^����ԂłȂ����
				if (mState != ESTUN) 
				{
					//���R���C�_���m�̏Փ˔���
					if (CCollider::Collision(m, o))
					{
						mState = ESTUN; //�X�^����Ԃֈڍs
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

}

//�ҋ@����
void CXEnemy::Idle()
{
	//�ҋ@�A�j���[�V����
	ChangeAnimation(2, true, 200);

	//�v���C���[�����S��Ԃł͖����Ƃ�
	if (CXPlayer::GetInstance()->GetState() != CXPlayer::EPlayerState::EDEATH) {
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
		//mPoint��mX��mZ�̒l�֓�����0.0f������ƓG��������̂ŁA�Ō�ɒl�𑫂��Ă���
		mPoint.mX += -15.0f + (float)(rand() % 30) + 0.001f;
		mPoint.mZ += -15.0f + (float)(rand() % 30) + 0.001f;
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
	if (CXPlayer::GetInstance()->GetState() != CXPlayer::EPlayerState::EDEATH) {
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
		random = rand() % 480;
		if (random == 0) {
			mState = EATTACK_2;
		}
	}
}

//�U���ҋ@����
void CXEnemy::Attack_Idle()
{
	//�v���C���[�����S��Ԃ������Ƃ��ҋ@��ԂɈڍs����
	if (CXPlayer::GetInstance()->GetState() == CXPlayer::EPlayerState::EDEATH)mState = EIDLE;

	//�ҋ@�A�j���[�V����
	ChangeAnimation(2, true, 200);

	//mMoveDir�Ƀv���C���[�����̃x�N�g��������
	mMoveDir = mPlayerPoint.Normalize();

	int random = 0;

	//�v���C���[�Ƃ̋���������\�ȋ����̂Ƃ�
	if (mPlayerDis <= AVOID_DIS) {
		//�v���C���[���U���������Ƃ�
		if (CXPlayer::GetInstance()->mAttackFlag_Once == true) {
			//�����_���ŉ�����s�����ǂ������肷��
			random = rand() % 6;
			if (random == 0) {
				mState = EAVOID; //�����Ԃֈڍs
			}
		}
	}

	//�v���C���[���U���\�ȋ����ɂ���Ƃ�
	if (mPlayerDis <= ATTACK_DIS) {
		mChaseRestartCount = 0; //�J�E���g�����Z�b�g����
		//�U���\�ȂƂ�
		if (CEnemyManager::GetInstance()->mIsEnemyAttack()) {
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
	else {
		mChaseRestartCount++; //�J�E���g�����Z����
		//�J�E���g���ǐՂ��ĊJ���鎞�ԂɂȂ�Ə�Ԃ��ڍs����
		if (mChaseRestartCount == CHASE_RESTART_TIME) {
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
		//�A�j���[�V�����I����
		if (mIsAnimationEnd())
		{
			mState = EIDLE; //�ҋ@��Ԃֈڍs
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
		//�A�j���[�V�����I����
		if (mIsAnimationEnd())
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
		//�A�j���[�V�����I����
		if (mIsAnimationEnd())
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
	if (mAnimationIndex == 15) 
	{
		//�A�j���[�V�����I����
		if (mIsAnimationEnd())
		{
			int random = rand() % 2;
			//����ド���_���ōU��2��Ԃֈڍs���邩�ǂ������f
			switch (random) {
			case 0:
				//�U���\�ȂƂ�
				if (CEnemyManager::GetInstance()->mIsEnemyAttack()) {
					mState = EATTACK_2; //�U��2��Ԃֈڍs
				}
				//�U���\�ł͂Ȃ��Ƃ�
				else {
					mState = EIDLE; //�ҋ@��Ԃֈڍs
				}
				break;

			case 1:
				mState = EIDLE; //�ҋ@��Ԃֈڍs
				break;
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

//�G�̏�Ԃ��擾����
CXEnemy::EEnemyState CXEnemy::GetState()
{
	return mState;
}

//�̗͂�ݒ肷��
void CXEnemy::SetHp(int hp)
{
	mHp = hp;		//�̗͂̏����l��ݒ�
	mHpMax = mHp;	//�̗͂̏����l��̗͂̍ő�l�Ƃ���
}

//���S��Ԃ̂Ƃ�true��Ԃ�
bool CXEnemy::mIsDeath()
{
	return (mState == EDEATH);
}

//�m�b�N�o�b�N���\�ȂƂ���true��Ԃ�
bool CXEnemy::mIsKnockBack()
{
	//�X�^����Ԃ������͍U����ԂłȂ����true
	return (mState != ESTUN && mState != EATTACK_1 && mState != EATTACK_2);
}

//�U����Ԃ̂Ƃ�true��Ԃ�
bool CXEnemy::mIsAttack()
{
	return (mState == EATTACK_1 || mState == EATTACK_2);
}
