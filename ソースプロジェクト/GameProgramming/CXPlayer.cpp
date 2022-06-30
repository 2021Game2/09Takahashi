#include "CXPlayer.h"
#include "CKey.h"
#include "CCamera.h"
#include "CUtil.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "CInput.h"
#include "CTrapManager.h"
#include "CEffect2.h"
#include "CCollisionManager.h"
#include "CEnemyManager.h"
#include "CRes.h"

#define GRAVITY 0.9f			//�d��
#define HP_MAX 100				//�̗͍ő�l
#define STAMINA_MAX 100			//�X�^�~�i�ő�l
#define AVOID_STAMINA STAMINA_MAX*0.3f //������̃X�^�~�i�̌����ʁA�X�^�~�i�ő�l��3�������
#define AVOID_TIME 30			//�������
#define AVOID_FIRSTSPEED 0.5f	//������̏���
#define SPEED_DEFAULT 0.15f		//�X�s�[�h(�ʏ펞)
#define SPEED_DASH_HIGH 0.2f	//�X�s�[�h(�_�b�V����)
#define SPEED_DASH_LOW 0.05f	//�X�s�[�h(�_�b�V�����A�X�^�~�i�؂�)
#define INVINCIBLETIME_AVOID 30	//���G����(�����)
#define INVINCIBLETIME_DAMAGE 60//���G����(�_���[�W������)
#define KNOCKBACK_AMOUNT 0.1f	//�m�b�N�o�b�N���̈ړ���
#define DAMAGE 20				//�_���[�W
#define ATTACK2_FIRSTSPEED 0.6f	//�U��2�g�p���̏���
#define GRACETIME 15			//�h���U���̎�t����
#define COMBO_MAX 4				//�U����A���Ŕh������������

#define PORTION_QUANTITY 5		//�񕜖�̏�����
#define HEAL_AMOUNT HP_MAX*0.3f	//�񕜖���g�p�����Ƃ��̉񕜗ʁA�̗͍ő�l��3���񕜂���

#define GAUGE_FRAME_TEX_WID 426	//�Q�[�W�g�̉摜�̕�
#define GAUGE_FRAME_TEX_HEI 62	//�Q�[�W�g�̉摜�̍���
#define GAUGE_FRAME_TEX_GAUGE_START 87 //�Q�[�W�g�̉摜�̃Q�[�W�J�n�n�_

#define GAUGE_FRAME_LEFT 20	//�Q�[�W�g�����W
#define GAUGE_FRAME_RIGHT (GAUGE_FRAME_LEFT+GAUGE_FRAME_TEX_WID) //�Q�[�W�g�E���W
#define GAUGE_FRAME_TOP 590 //�Q�[�W�g����W
#define GAUGE_FRAME_BOTTOM (GAUGE_FRAME_TOP-GAUGE_FRAME_TEX_HEI) //�Q�[�W�g�����W

#define GAUGE_LEFT (GAUGE_FRAME_LEFT+GAUGE_FRAME_TEX_GAUGE_START)	//�Q�[�W�`�掞�̍����W
#define GAUGE_WID_MAX (GAUGE_FRAME_TEX_WID - GAUGE_FRAME_TEX_GAUGE_START)	//�Q�[�W�̕��̍ő�l

#define GAUGE_HEIGHT 30 //�Q�[�W�`�掞�̍���

#define GAUGE_HP_TOP GAUGE_FRAME_TOP //HP�Q�[�W�`�掞�̏���W
#define GAUGE_HP_BOTTOM (GAUGE_HP_TOP-GAUGE_HEIGHT) //HP�Q�[�W�`�掞�̉����W

#define GAUGE_STAMINA_TOP (GAUGE_FRAME_TOP-32) //�X�^�~�i�Q�[�W�`�掞�̏���W
#define GAUGE_STAMINA_BOTTOM (GAUGE_STAMINA_TOP-GAUGE_HEIGHT) //�X�^�~�i�Q�[�W�`�掞�̉����W

CXPlayer* CXPlayer::mInstance;

CXPlayer::CXPlayer()
	: mColSphereBody(this, nullptr, CVector(), 0.5f)
	, mColSphereHead(this, nullptr, CVector(0.0f, 5.0f, -3.0f), 0.5f)
	, mColSphereSword(this, nullptr, CVector(-10.0f, 10.0f, 50.0f), 0.3f)
	, mHp(HP_MAX)
	, mStamina(STAMINA_MAX)
	, mAvoid(false)
	, mAvoidTime(0)
	, mAvoidSpeed(0.0f)
	, mSpeed(SPEED_DEFAULT)
	, mMoveDirKeep(0.0f, 0.0f, 0.0f)
	, mInvincibleFlag(false)
	, mInvincibleTime(0)
	, mMoveDir(0.0f, 0.0f, 0.0f)
	, mMove(0.0f, 0.0f, 0.0f)
	, mSideVec(0.0f, 0.0f, 0.0f)
	, mFrontVec(0.0f, 0.0f, 0.0f)
	, mTurnspeed(0.0f)
	, mAttackFlag_1(false)
	, mAttackFlag_2(false)
	, mAttack2Speed(0.0f)
	, mAttackFlag_3(false)
	, mGraceTime(0)
	, mHit(false)
	, mItemSelect(HEAD + 1)
	, mAttackFlag_Once(false)
	, mComboCount(0)
	, mPortionQuantity(PORTION_QUANTITY)
	, mKnockBackDir(0.0f, 0.0f, 0.0f)
	, mAttackDir(0.0f, 0.0f, 0.0f)
	, mpTargetEnemy(nullptr)
	, mFollowGaugeWid(GAUGE_WID_MAX)
{
	//�^�O��ݒ�
	mTag = EPLAYER;	//�v���C���[

	//�R���C�_�̃^�O��ݒ�
	mColSphereBody.mTag = CCollider::EBODY;		//��
	mColSphereHead.mTag = CCollider::EHEAD;		//��
	mColSphereSword.mTag = CCollider::ESWORD;	//��

	//������Ԃ�ݒ�
	mState = EIDLE;	//�ҋ@���
}

CXPlayer::~CXPlayer()
{
}

void CXPlayer::Init(CModelX* model)
{
	CXCharacter::Init(model);
	//�����s��̐ݒ�
	mColSphereBody.mpMatrix = &mpCombinedMatrix[9];
	//��
	mColSphereHead.mpMatrix = &mpCombinedMatrix[12];
	//��
	mColSphereSword.mpMatrix = &mpCombinedMatrix[22];

	mPosition.Set(0.0f, 0.0f, 20.0);	//�ʒu��ݒ�
	mRotation.Set(0.0f, 180.0f, 0.0f);	//��]��ݒ�
}

void CXPlayer::Update()
{
	mAttackFlag_Once = false;

	//�A�C�e���I������
	ItemSelect();

	//��Ԃ𔻕�
	switch (mState) {
	case EIDLE:	//�ҋ@���
		Idle();	//�ҋ@�������Ă�
		break;

	case EATTACK_1:	//�U��1��Ԃ̎�
		Attack_1();	//�U��1�̏������Ă�
		break;

	case EATTACK_2:	//�U��2��Ԃ̎�
		Attack_2();	//�U��2�̏������Ă�
		break;

	case EATTACK_3:	//�U��3��Ԃ̎�
		Attack_3();	//�U��3�̏������Ă�
		break;

	case EMOVE:	//�ړ����
		Move();	//�ړ���Ԃ̏������Ă�
		break;

	case EDASH:	//�_�b�V�����
		Dash();	//�_�b�V����Ԃ̏������Ă�
		break;

	case EAVOID: //������
		Avoid(); //����������Ă�
		break;

	case EDEATH: //���S���
		Death(); //���S�������Ă�
		break;

	case EITEMUSE: //�A�C�e���g�p��
		ItemUse(); //�A�C�e���g�p�������Ă�
		break;

	case EKNOCKBACK: //�m�b�N�o�b�N���
		KnockBack(); //�m�b�N�o�b�N�������Ă�
		break;
	}

	//�_�b�V���A��������Ă��Ȃ���Ԃ̎��X�^�~�i���񕜂�����
	if (mState != EDASH && mState != EAVOID && mStamina < STAMINA_MAX) {
		mStamina++;
	}

	//���W�ړ�
	mPosition += mMove;

	//����������
	mMove = mMove * GRAVITY; 

	//���ʂ�3�����x�N�g���v�Z�ŎZ�o�����ق������m�����v�Z�ʂ����O����ꍇ�͋[���v�Z�Ōy�ʉ�
	//�[���x�N�g���v�Z
	Check tCheck = CUtil::GetCheck2D(mMoveDir.mX, mMoveDir.mZ, 0, 0, mRotation.mY * (M_PI / 180.0f));

	//��]���x�@degree�ɒ���
	mTurnspeed = (180.0f / M_PI) * 0.5f;

	//�}�ȐU��Ԃ��}��
	if (tCheck.turn > 1.5f) tCheck.turn = 1.5f;

	//�ړ������փL��������������
	if (tCheck.cross > 0.0f) {
		mRotation.mY += tCheck.turn * mTurnspeed;
	}
	if (tCheck.cross < 0.0f) {
		mRotation.mY -= tCheck.turn * mTurnspeed;
	}

	//���Z�b�g
	mMoveDir = CVector(0.0f, 0.0f, 0.0f);

	//�̗͂�0�ɂȂ�Ǝ��S
	if (mHp <= 0) {
		mState = EDEATH;	//���S��Ԃֈڍs
		mHp = 0;
	}

	//�����_
	Camera.SetTarget(mPosition);

	CXCharacter::Update();
}

void CXPlayer::Render2D()
{
	//2D�`��J�n
	CUtil::Start2D(0, 800, 0, 600);

	//�Q�[�W��h�炷�p
	int shakeX = 0;
	int shakeY = 0;

	//�m�b�N�o�b�N��Ԃ̂Ƃ�
	if (mState == EKNOCKBACK) {
		//�Ԃ��摜��\������
		CRes::sImageGauge.mAlpha = 0.3f;	//�A���t�@�l��ύX
		CRes::sImageGauge.Draw(0, 800, 0, 600, 410, 410, 410, 410);	//�`��
		CRes::sImageGauge.mAlpha = 1.0f;	//�A���t�@�l��߂�
		//�Q�[�W��h�炷�l��ݒ�
		shakeX = -5 + rand() % 10;
		shakeY = -5 + rand() % 10;
	}

	//�Q�[�W�g�\��
	CRes::sImageGaugeFrame.Draw(GAUGE_FRAME_LEFT, GAUGE_FRAME_RIGHT, GAUGE_FRAME_BOTTOM, GAUGE_FRAME_TOP, 0, GAUGE_FRAME_TEX_WID, GAUGE_FRAME_TEX_HEI, 0);

	//�̗̓Q�[�W
	float HpRate = (float)mHp / (float)HP_MAX;	//�̗͍ő�l�ɑ΂���A���݂̗̑͂̊���
	float HpGaugeWid = GAUGE_WID_MAX * HpRate;	//�̗̓Q�[�W�̕�
	//��_���[�W����ǂ�����Q�[�W�̕����̗̓Q�[�W�̕����傫����
	if (mFollowGaugeWid > HpGaugeWid) {
		//���`��ԂŔ�_���[�W����ǂ�����Q�[�W�̕���ݒ肷��
		mFollowGaugeWid = Camera.mLerp(mFollowGaugeWid, HpGaugeWid, 0.05f);
	}
	//��_���[�W����ǂ�����Q�[�W�̕����̗̓Q�[�W�̕���菬�����Ƃ�
	else if (mFollowGaugeWid < HpGaugeWid) {
		//��_���[�W����ǂ�����Q�[�W�̕��ɑ̗̓Q�[�W�̕���ݒ肷��
		mFollowGaugeWid = HpGaugeWid;
	}
	CRes::sImageGauge.Draw(GAUGE_LEFT + shakeX, GAUGE_LEFT + mFollowGaugeWid + shakeX, GAUGE_HP_BOTTOM + shakeY, GAUGE_HP_TOP + shakeY, 443, 443, 36, 36);	//��_���[�W����ǂ�����Q�[�W��\��
	CRes::sImageGauge.Draw(GAUGE_LEFT + shakeX, GAUGE_LEFT + HpGaugeWid + shakeX, GAUGE_HP_BOTTOM + shakeY, GAUGE_HP_TOP + shakeY, 0, 0, 0, 0);				//�̗̓Q�[�W��\��

	//�X�^�~�i�Q�[�W
	float StaminaRate = (float)mStamina / (float)STAMINA_MAX;	//�X�^�~�i�ő�l�ɑ΂���A���݂̃X�^�~�i�̊���
	float StaminaGaugeWid = GAUGE_WID_MAX * StaminaRate;		//�X�^�~�i�Q�[�W�̕�
	CRes::sImageGauge.Draw(GAUGE_LEFT, GAUGE_LEFT + StaminaGaugeWid, GAUGE_STAMINA_BOTTOM, GAUGE_STAMINA_TOP, 110, 190, 63, 0);	//�X�^�~�i�Q�[�W��\��

	char buf[64];
	CRes::sImageGauge.Draw(630, 750, 30, 150, 310, 390, 63, 0);	//�A�C�e���w�i��\��
	//�I�𒆂̃A�C�e��
	switch (mItemSelect) {
	case ETRAP: //�
		CRes::sImageTrap.Draw(640, 740, 40, 140, 0, 255, 255, 0); //㩉摜��\��
		sprintf(buf, "%d", CTrapManager::GetInstance()->mTrapQuantity); //㩂̏�����
		break;
	case EPORTION: //��
		CRes::sImagePortion.Draw(640, 740, 40, 140, 0, 255, 255, 0); //�񕜖�摜��\��
		sprintf(buf, "%d", mPortionQuantity); //�񕜂̏�����
		break;
	}
	//�A�C�e�����g�p�s�\�Ȏ�
	if (mIsItemUse() == false) {
		CRes::sImageNixsign.Draw(640, 740, 40, 140, 255, 0, 255, 0); //�֎~�}�[�N�摜��\��
	}

	CRes::sFont.DrawString(buf, 730, 50, 15, 15); //�A�C�e���̏�������\��

	//2D�̕`��I��
	CUtil::End2D();
}

void CXPlayer::Collision(CCollider* m, CCollider* o)
{
	//���S��Ԃ̎���return����
	if (mState == EDEATH) return;

	//�����Ƒ��肪���R���C�_�̎�
	if (m->mType == CCollider::ESPHERE && o->mType == CCollider::ESPHERE)
	{
		//����̐e�̃^�O���G
		if (o->mpParent->mTag == EENEMY)
		{
			//�G�����S��Ԃ̂Ƃ����^�[������
			if (((CXEnemy*)(o->mpParent))->GetState() == CXEnemy::EDEATH)return;
			//����̃R���C�_�̃^�O����
			if (o->mTag == CCollider::ESWORD)
			{
				//�v���C���[�����G��Ԃł͂Ȃ��Ƃ�
				if (mInvincibleFlag == false)
				{
					//�L���X�g�ϊ�
					//�G�̍U���̃q�b�g���肪�L���ȂƂ�
					if (((CXEnemy*)(o->mpParent))->mHit == true)
					{
						//���R���C�_���m�̏Փ˔���
						if (CCollider::Collision(m, o)) {
							//�U�����󂯂��ӏ�
							switch (m->mTag) {
							case CCollider::EHEAD:	//��
							case CCollider::EBODY:	//��
								mHp -= DAMAGE;		//�_���[�W���󂯂�
								((CXEnemy*)(o->mpParent))->mHit = false; //�G�̍U���̃q�b�g������I��������
								mState = EKNOCKBACK;	//�m�b�N�o�b�N��Ԃֈڍs
								mKnockBackDir = ((CXEnemy*)(o->mpParent))->mPosition - mPosition; //�m�b�N�o�b�N������������߂�
								mInvincibleFlag = true;	//���G�t���O��L���ɂ���
								mHit = false;			//�����̍U���̃q�b�g����𖳌��ɂ���
								mAttackFlag_1 = false;
								mAttackFlag_2 = false;
								mAttackFlag_3 = false;
								mMove = CVector(0.0f, 0.0f, 0.0f);
								CRes::sSEAttackHit2.Play();	//�U���q�b�g���̌��ʉ��Đ�
								break;
							}
						}
					}
				}
			}
			//�v���C���[�ƓG�̂��蔲���h�~����
			//�����Ƒ���̃R���C�_�̃^�O���̂̂Ƃ�
			if (m->mTag == CCollider::EBODY && o->mTag == CCollider::EBODY) {
				CVector adjust; //�����p
				//���R���C�_���m�̏Փ˔���(Y���W�𒲐����Ȃ�)
				if (CCollider::CollisionAdjust(m, o, &adjust)) {
					//�G���X�^����Ԃł͂Ȃ��Ƃ��A�v���C���[���U��2��Ԃł͂Ȃ��Ƃ�
					if (((CXEnemy*)(o->mpParent))->GetState() != CXEnemy::ESTUN && mState != EATTACK_2) {
						//�G�̃|�W�V�����𒲐�
						CXEnemy* Enemy = (CXEnemy*)o->mpParent;
						Enemy->SetPos(Enemy->GetPos() + adjust);
					}
					else {
						//�v���C���[�̃|�W�V�����𒲐�
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
					mPosition += adjust; //�|�W�V�����𒲐�����
				}
			}
		}
	}
}

void CXPlayer::TaskCollision()
{
	CCollisionManager::Get()->Collision(&mColSphereBody, COLLISIONRANGE);
	CCollisionManager::Get()->Collision(&mColSphereHead, COLLISIONRANGE);
	CCollisionManager::Get()->Collision(&mColSphereSword, COLLISIONRANGE);
}

void CXPlayer::Generate()
{
	mInstance = new CXPlayer;
}

void CXPlayer::Release()
{
	if (mInstance) {
		delete mInstance;
		mInstance = nullptr;
	}
}

CXPlayer* CXPlayer::GetInstance()
{
	return mInstance; //�C���X�^���X��Ԃ�
}

//���̃R���C�_�̍��W���擾����
CVector CXPlayer::GetSwordColPos()
{
	return mColSphereSword.mpMatrix->GetPos();	//���̃R���C�_�̍��W��Ԃ�
}

//�v���C���[�̏�Ԃ��擾����
CXPlayer::EPlayerState CXPlayer::GetState()
{
	return mState;
}

//�ҋ@����
void CXPlayer::Idle()
{
	ChangeAnimation(0, true, 60); //�ҋ@���[�V����
	mComboCount = 0; //�R���{�������Z�b�g

	//���N���b�N�ōU��1�ֈڍs
	if (CKey::Once(VK_LBUTTON)) {
		mState = EATTACK_1;
	}
	//WASD�L�[�������ƈړ��ֈڍs
	else if (CKey::Push('W') || CKey::Push('A') || CKey::Push('S') || CKey::Push('D')) {
		mState = EMOVE;
		CRes::sSEPlayerWalk.Repeat(); //���ʉ����Đ�����
	}
	//�E�N���b�N�ŃA�C�e���g�p�\�Ȏ��A�C�e�����g�p����
	else if (CKey::Once(VK_RBUTTON) && mIsItemUse()) {
		mState = EITEMUSE;
	}
}

//�ړ�����
void CXPlayer::Move()
{
	ChangeAnimation(1, true, 65);

	mSpeed = SPEED_DEFAULT;

	//���N���b�N�ōU��1�ֈڍs
	if (CKey::Once(VK_LBUTTON)) {
		mState = EATTACK_2;
	}
	//SPACE�L�[������������ɕK�v�ȗʂ̃X�^�~�i������Ƃ�����ֈڍs
	else if (CKey::Once(VK_SPACE) && mStamina >= AVOID_STAMINA) {
		mState = EAVOID;
		CRes::sSEPlayerAvoid.Play(); //���ʉ����Đ�
	}
	//�E�N���b�N�ŃA�C�e���g�p�\�Ȏ��A�C�e�����g�p����
	else if (CKey::Once(VK_RBUTTON) && mIsItemUse()) {
		mState = EITEMUSE;
	}
	//WASD�L�[�������ƈړ�
	else if (CKey::Push('W') || CKey::Push('A') || CKey::Push('S') || CKey::Push('D')) {
		//SHIFT�L�[�������Ă���ƃ_�b�V���ֈڍs
		if (CKey::Push(VK_SHIFT)) {
			mState = EDASH;
			CRes::sSEPlayerRun.Repeat(); //���ʉ����Đ�����
		}
		else {
			MoveCamera(); //�J��������ɂ����ړ��������Ă�
		}
	}
	//�ҋ@��Ԃֈڍs
	else {
		mState = EIDLE;
	}
	//��Ԃ��ڍs�����Ƃ��A���ʉ����~����
	if (mState != EMOVE) {
		CRes::sSEPlayerWalk.Stop();
	}
}

//�_�b�V������
void CXPlayer::Dash()
{
	//�X�^�~�i���c���Ă���Ƃ�
	if (mStamina > 0) {
		ChangeAnimation(1, true, 40);
		mSpeed = SPEED_DASH_HIGH;
		mStamina--;
		if (mStamina == 0) {
			CRes::sSEPlayerRun.Stop();		//���ʉ���~
			CRes::sSEPlayerRunSlow.Repeat();	//���ʉ��Đ�
		}
	}
	//�X�^�~�i�؂���
	else {
		ChangeAnimation(1, true, 100);
		mSpeed = SPEED_DASH_LOW;
	}

	//���N���b�N�ōU��1�ֈڍs
	if (CKey::Once(VK_LBUTTON)) {
		mState = EATTACK_2;
	}
	//SPACE�L�[������������ɕK�v�ȗʂ̃X�^�~�i������Ƃ�����ֈڍs
	else if (CKey::Once(VK_SPACE) && mStamina >= AVOID_STAMINA) {
		mState = EAVOID;
		CRes::sSEPlayerAvoid.Play(); //���ʉ����Đ�
	}
	//�E�N���b�N�ŃA�C�e���g�p�\�Ȏ��A�C�e�����g�p����
	else if (CKey::Once(VK_RBUTTON) && mIsItemUse()) {
		mState = EITEMUSE;
	}
	//WASD�L�[�������ƈړ�
	else if (CKey::Push('W') || CKey::Push('A') || CKey::Push('S') || CKey::Push('D')) {
		//SHIFT�L�[�������Ă���ƃ_�b�V��
		if (CKey::Push(VK_SHIFT) == false) {
			mState = EMOVE;
			CRes::sSEPlayerWalk.Repeat(); //���ʉ����Đ�
		}
		else {
			MoveCamera(); //�J��������ɂ����ړ��������Ă�
		}
	}
	//�ҋ@��Ԃֈڍs
	else {
		mState = EIDLE;
	}
	//��Ԃ��ڍs�����Ƃ��A���ʉ����~����
	if (mState != EDASH) {
		CRes::sSEPlayerRun.Stop();
		CRes::sSEPlayerRunSlow.Stop();
	}
}

//�U��1����
void CXPlayer::Attack_1()
{
	//�U����������x�����ʂ�
	if (mAttackFlag_1 == false) {
		ChangeAnimation(3, true, 20);
		mAttackFlag_1 = true;
		mGraceTime = 0;
		mAttackFlag_Once = true;
		mComboCount++; //�R���{�����Z
		mpTargetEnemy = CEnemyManager::GetInstance()->GetNearEnemy(); //�v���C���[�Ɉ�ԋ߂��G���擾����
		if (mpTargetEnemy) {
			mAttackDir = mpTargetEnemy->mPosition - mPosition; //�U�����̌��������߂�
			mMoveDirKeep = mAttackDir.Normalize();
		}
		else {
			mAttackDir = mMoveDirKeep.Normalize();
		}
	}

	if (mGraceTime > 0) {
		mGraceTime--;	//��t���Ԃ�����
	}

	if (mAnimationIndex == 3)
	{
		//�U�������Ɉړ�������
		mPosition += mAttackDir.Normalize() * 0.04f;
		//�q�b�g���蔭��
		if (mAnimationFrame == 5) {
			mHit = true;
		}
		//�A�j���[�V�����I����
		if (mIsAnimationEnd())
		{
			mHit = false; //�q�b�g����I��
			ChangeAnimation(4, false, 30);
			mGraceTime = GRACETIME;	//��t���Ԃ�����
		}
	}
	else if (mAnimationIndex == 4)
	{
		//�A�j���[�V�����̍Đ��t���[�������h���U���̎�t���ԓ��̂Ƃ�
		if (mAnimationFrame < GRACETIME) {
			//���N���b�N�����Ă���Ƃ�
			if (CKey::Once(VK_LBUTTON)) {
				//�R���{�����R���{�̏���񐔂ɒB���Ă��Ȃ��Ƃ�
				if (mComboCount < COMBO_MAX) {
					mState = EATTACK_3; //�U��3��Ԃֈڍs
				}
				else {
					mState = EATTACK_2; //�U��2��Ԃֈڍs
				}
				mAttackFlag_1 = false;
			}
		}
		//�A�j���[�V�����I����
		if (mIsAnimationEnd())
		{
			mState = EIDLE; //�ҋ@��Ԃֈڍs
			mAttackFlag_1 = false;
		}
	}

	mMoveDir = mAttackDir.Normalize(); //�U�����̌����𐳋K�����đ������
}

//�U��2����
void CXPlayer::Attack_2()
{
	//�U����������x�����ʂ�
	if (mAttackFlag_2 == false) {
		ChangeAnimation(7, true, 30);
		mAttackFlag_2 = true;
		mAttack2Speed = ATTACK2_FIRSTSPEED;
		mAttackFlag_Once = true;
		mpTargetEnemy = CEnemyManager::GetInstance()->GetNearEnemy(); //�v���C���[�Ɉ�ԋ߂��G���擾����
	}

	if (mAnimationIndex == 7)
	{
		//�G��ǔ�������
		if (mpTargetEnemy) {
			mAttackDir = mpTargetEnemy->mPosition - mPosition; //���������߂�
			mMoveDirKeep = mAttackDir.Normalize();
		}
		else {
			mAttackDir = mMoveDirKeep.Normalize();
		}
		//�q�b�g���蔭��
		if (mAnimationFrame == 8) {
			mHit = true;
		}
		//�A�j���[�V�����I����
		if (mIsAnimationEnd())
		{
			mHit = false; //�q�b�g����I��
			ChangeAnimation(8, false, 30);
		}
	}
	else if (mAnimationIndex == 8)
	{
		//�A�j���[�V�����I����
		if (mIsAnimationEnd())
		{
			mState = EIDLE; //�ҋ@��Ԃֈڍs
			mAttackFlag_2 = false;
		}
	}

	mMoveDir = mAttackDir.Normalize(); //�U�����̌����𐳋K�����đ������

	mMove = mMoveDir * mAttack2Speed;
	mAttack2Speed = mAttack2Speed * GRAVITY;
}

//�U��3����
void CXPlayer::Attack_3()
{
	//�U������x�����ʂ�
	if (mAttackFlag_3 == false) {
		ChangeAnimation(5, true, 15);
		mAttackFlag_3 = true;
		mAttackFlag_Once = true;
		mComboCount++; //�R���{�����Z
		mpTargetEnemy = CEnemyManager::GetInstance()->GetNearEnemy(); //�v���C���[�Ɉ�ԋ߂��G���擾����
		if (mpTargetEnemy) {
			mAttackDir = mpTargetEnemy->mPosition - mPosition; //�U�����̌��������߂�
			mMoveDirKeep = mAttackDir.Normalize();
		}
		else {
			mAttackDir = mMoveDirKeep.Normalize();
		}
	}

	if (mAnimationIndex == 5)
	{
		//�U�������Ɉړ�������
		mPosition += mAttackDir.Normalize() * 0.03f;
		//�q�b�g���蔭��
		if (mAnimationFrame == 0) {
			mHit = true;
		}
		//�A�j���[�V�����I����
		if (mIsAnimationEnd())
		{
			mHit = false; //�q�b�g����I��
			ChangeAnimation(6, false, 30);
		}
	}
	else if (mAnimationIndex == 6)
	{
		//�A�j���[�V�����̍Đ��t���[�������h���U���̎�t���ԓ��̂Ƃ�
		if (mAnimationFrame < GRACETIME) {
			//���N���b�N�����Ă���Ƃ�
			if (CKey::Once(VK_LBUTTON)) {
				//�R���{�����R���{�̏���񐔂ɒB���Ă��Ȃ��Ƃ�
				if (mComboCount < COMBO_MAX) {
					mState = EATTACK_1; //�U��1��Ԃֈڍs
				}
				else {
					mState = EATTACK_2; //�U��2��Ԃֈڍs
				}
				mAttackFlag_3 = false;
			}
		}
		//�A�j���[�V�����I����
		if (mIsAnimationEnd())
		{
			mState = EIDLE; //�ҋ@��Ԃֈڍs
			mAttackFlag_3 = false;
		}
	}

	mMoveDir = mAttackDir.Normalize(); //�U�����̌����𐳋K�����đ������
}

//�������
void CXPlayer::Avoid()
{
	//�������x�����ʂ�
	if (mAvoid == false) {
		mAvoid = true;							//���
		mStamina -= AVOID_STAMINA;				//�X�^�~�i����	
		mAvoidTime = AVOID_TIME;				//�������
		mAvoidSpeed = AVOID_FIRSTSPEED;			//�����ݒ�
		mInvincibleFlag = true;					//���G���
		mInvincibleTime = INVINCIBLETIME_AVOID;	//���G����
	}

	//���G���Ԃ��J�E���g�_�E��
	mInvincibleTime--;
	//���G���Ԃ�0�ɂȂ����Ƃ�
	if (mInvincibleTime == 0) {
		mInvincibleFlag = false; //���G��Ԃ��I������
	}

	//������Ԃ��J�E���g�_�E��
	mAvoidTime--;
	//�������0�ɂȂ�����
	if (mAvoidTime == 0) {
		mAvoid = false;	//����I��
	}

	mMove = mMoveDirKeep * mAvoidSpeed;
	mAvoidSpeed = mAvoidSpeed * GRAVITY;	//�X�s�[�h�������Ă���

	//�����Ԃ��I�������Ƃ�
	if (mAvoid == false) {
		//����I����WASD�L�[��������Ă���ƈړ�
		if (CKey::Push('W') || CKey::Push('A') || CKey::Push('S') || CKey::Push('D')) {
			//SHIFT�L�[�������Ă���ƃ_�b�V���ֈڍs
			if (CKey::Push(VK_SHIFT)) {
				mState = EDASH;
				CRes::sSEPlayerRun.Repeat(); //���ʉ����Đ�
			}
			else {
				mState = EMOVE;
				CRes::sSEPlayerWalk.Repeat(); //���ʉ����Đ�
			}
		}
		//�ҋ@��Ԃֈڍs
		else {
			mState = EIDLE;
		}
	}
}

//���S����
void CXPlayer::Death()
{
	ChangeAnimation(11, false, 50);	//�|���A�j���[�V����
}

//�m�b�N�o�b�N����
void CXPlayer::KnockBack()
{
	ChangeAnimation(2, false, 20);

	//���ʉ����~����
	CRes::sSEPlayerWalk.Stop();
	CRes::sSEPlayerRun.Stop();
	CRes::sSEPlayerAvoid.Stop();

	//�m�b�N�o�b�N�������K��
	mKnockBackDir = mKnockBackDir.Normalize();

	//�U���𓖂ĂĂ����G�̕����������邽�߂ɑ������
	mMoveDir = mKnockBackDir;

	//�m�b�N�o�b�N������
	mPosition -= mKnockBackDir * KNOCKBACK_AMOUNT;

	if (mAnimationIndex == 2)
	{
		//�A�j���[�V�����I����
		if (mIsAnimationEnd())
		{
			mState = EIDLE;	//�ҋ@��Ԃֈڍs
			mInvincibleFlag = false; //���G��Ԃ��I������
		}
	}
}

//�J��������ɂ����ړ�����
void CXPlayer::MoveCamera()
{
	//�J�������_�ړ��@�ʏ̖��o�ړ�
	//�J�����̍��E�ƑO��̃x�N�g�����擾
	mSideVec = Camera.GetMat().GetXVec();
	mFrontVec = Camera.GetMat().GetZVec();
	//�����ړ��̓J�b�g����
	mSideVec.mY = 0.0f;
	mFrontVec.mY = 0.0f;
	//���K������
	mSideVec.Normalize();
	mFrontVec.Normalize();

	if (CKey::Push('A'))
	{
		mMoveDir -= mSideVec;
	}
	else if (CKey::Push('D'))
	{
		mMoveDir += mSideVec;
	}
	if (CKey::Push('W')) {
		mMoveDir += mFrontVec;
	}
	else if (CKey::Push('S'))
	{
		mMoveDir -= mFrontVec;
	}

	//�ړ��ʐ��K���@��������Ȃ��Ǝ΂߈ړ��������Ȃ��Ă��܂��̂Œ���
	//�W�����v���Ȃǂ�Y���𐳋K�����Ȃ��悤����
	mMoveDir = mMoveDir.Normalize();
	mMoveDirKeep = mMoveDir;	//MoveDir�ۑ�
	mMove = mMoveDir * mSpeed;	//�ړ��ʂ�ݒ�
}

//�A�C�e�����g�p�\�����f����
bool CXPlayer::mIsItemUse()
{
	//�I�𒆂̃A�C�e��
	switch (mItemSelect) {
	case ETRAP:	//�
		//㩃A�C�e�����g�p�\�Ȏ�
		if (CTrapManager::GetInstance()->TrapAvailable()) {
			return true;
		}
		break;

	case EPORTION: //�񕜖�
		//�񕜖�̏�������0��葽���Ƃ��A���݂̗̑͂��̗͍ő�l��������Ă���Ƃ�
		if (mPortionQuantity > 0 && mHp < HP_MAX) {
			return true;
		}
		break;
	}
	return false;
}

//�A�C�e���g�p����
void CXPlayer::ItemUse()
{
	//�I�𒆂̃A�C�e��
	switch (mItemSelect) {
	case ETRAP:	//�
		//㩐���
		CTrapManager::GetInstance()->TrapGenerate(mPosition, mRotation);
		//㩃A�C�e���g�p���̌��ʉ����Đ�����
		CRes::sSETrapUse.Play();
		break;

	case EPORTION: //�񕜖�
		//�񕜖�̏����������炷
		mPortionQuantity--;
		//�̗͂��񕜂�����
		mHp += HEAL_AMOUNT;
		//���݂̗̑͂��̗͍ő�l�𒴂��Ȃ��悤�ɂ���
		if (mHp > HP_MAX)mHp = HP_MAX;
		//�񕜃A�C�e���g�p���̃G�t�F�N�g�𐶐�����
		new CEffect2(mPosition, 2.0f, 2.0f, "", 2, 5, 3);
		//�񕜃A�C�e���g�p���̌��ʉ��Đ�
		CRes::sSEPortionUse.Play();
		break;
	}
	//�ҋ@��Ԃֈڍs
	mState = EIDLE;
}

//�A�C�e���I������
void CXPlayer::ItemSelect()
{
	//�z�C�\���ړ��ʎ擾
	int wheel = CInput::GetWheelValue();
	//�z�C�[���𓮂����Ă���Ƃ�
	if (wheel != 0) {
		//�����
		if (wheel > 0) {
			mItemSelect++;
		}
		//������
		else {
			mItemSelect--;
		}
	}

	if (mItemSelect == TAIL) {
		mItemSelect = HEAD + 1;
	}
	if (mItemSelect == HEAD) {
		mItemSelect = TAIL - 1;
	}
}