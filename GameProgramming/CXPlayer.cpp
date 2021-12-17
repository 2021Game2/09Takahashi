#include "CXPlayer.h"
#include "CKey.h"
#include "CCamera.h"
#include "CUtil.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "CInput.h"
#include "CXEnemy.h"
#include "CTrap.h"

#define GRAVITY 0.9f			//�d��
#define HP_MAX 100				//�̗͍ő�l
#define STAMINA_MAX 100			//�X�^�~�i�ő�l
#define AVOID_STAMINA 30		//������̃X�^�~�i�̌�����
#define AVOID_TIME 30			//�������
#define AVOID_FIRSTSPEED 0.5f	//������̏���
#define SPEED_DEFAULT 0.15f		//�X�s�[�h(�ʏ펞)
#define SPEED_DASH_HIGH 0.2f	//�X�s�[�h(�_�b�V����)
#define SPEED_DASH_LOW 0.05f	//�X�s�[�h(�_�b�V�����A�X�^�~�i�؂�)
#define INVINCIBLETIME_AVOID 30	//���G����(�����)
#define INVINCIBLETIME_DAMAGE 60//���G����(�_���[�W������)
#define DAMAGE 20				//�_���[�W
#define ATTACK2_FIRSTSPEED 0.6f	//�U��2�g�p���̏���
#define GRACETIME 10			//�h���U���̎�t����
#define HEAL_AMOUNT HP_MAX		//�񕜗�

#define GAUGE_WID_MAX 350.0f	//�Q�[�W�̕��̍ő�l

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
	, mMoveKeep(0.0f, 0.0f, 0.0f)
	, mInvincibleFlag(false)
	, mInvincibleTime(0)
	, mMoveDir(0.0f, 0.0f, 0.0f)
	, mMove2(0.0f, 0.0f, 0.0f)
	, mSideVec(0.0f, 0.0f, 0.0f)
	, mFrontVec(0.0f, 0.0f, 0.0f)
	, mTurnspeed(0.0f)
	, mAttackFlag_1(false)
	, mAttackFlag_2(false)
	, mAttack2Speed(0.0f)
	, mAttackFlag_3(false)
	,mGraceTime(0)
	,mHit(false)
	,mItemSelect(EEMPTY)
	,mAttackFlag_Once(false)
{
	//�^�O�Ƀv���C���[��ݒ肵�܂�
	mTag = EPLAYER;
	mColSphereBody.mTag = CCollider::EBODY;
	mColSphereHead.mTag = CCollider::EHEAD;
	mColSphereSword.mTag = CCollider::ESWORD;

	mState = EIDLE;	//�ҋ@���

	mInstance = this;

	mFont.LoadTexture("FontG.png", 1, 4096 / 64);

	mTexture.Load("Gauge.png");
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

	mRotation.mY = 0.01f;
}

void CXPlayer::Update()
{
	mAttackFlag_Once = false;

	ItemSelect();

	switch (mState) {
	case EIDLE:	//�ҋ@���
		Idle();	//�ҋ@�������Ă�
		//���N���b�N�ōU��1�ֈڍs
		if (CKey::Once(VK_LBUTTON)) {
			mState = EATTACK_1;
		}
		//WASD�L�[�������ƈړ��ֈڍs
		else if (CKey::Push('W') || CKey::Push('A') || CKey::Push('S') || CKey::Push('D')) {
			mState = EMOVE;
		}
		//E�L�[�������ƃA�C�e���g�p
		else if (CKey::Once('E')) {
			mState = EITEMUSE;
		}
		break;

	case EATTACK_1:	//�U��1���
		Attack_1();	//�U��1�̏������Ă�
		//��t���ԓ��ɍ��N���b�N�ōU��3�ֈڍs
		if (mGraceTime > 0 && CKey::Once(VK_LBUTTON)) {
			mState = EATTACK_3;
		}
		if (mAttackFlag_1 == false) {
			mState = EIDLE;
		}
		break;

	case EATTACK_2:	//�U��2���
		Attack_2();	//�U��2�̏������Ă�
		if (mAttackFlag_2 == false) {
			mState = EIDLE;
		}
		break;

	case EATTACK_3:	//�U��3��Ԃ̎�
		Attack_3();	//�U��3�̏������Ă�
		if (mAttackFlag_3 == false) {
			mState = EIDLE;
		}
		break;

	case EMOVE:	//�ړ����
		//���N���b�N�ōU��1�ֈڍs
		if (CKey::Once(VK_LBUTTON)) {
			mState = EATTACK_2;
		}
		//SPACE�L�[������������ɕK�v�ȗʂ̃X�^�~�i������Ƃ�����ֈڍs
		else if (CKey::Once(VK_SPACE) && mStamina >= AVOID_STAMINA) {
			mState = EAVOID;
		}
		//E�L�[�������ƃA�C�e���g�p
		else if (CKey::Once('E')) {
			mState = EITEMUSE;
		}
		//WASD�L�[�������ƈړ�
		else if (CKey::Push('W') || CKey::Push('A') || CKey::Push('S') || CKey::Push('D')) {
			//SHIFT�L�[�������Ă���ƃ_�b�V���ֈڍs
			if (CKey::Push(VK_SHIFT)) {
				mState = EDASH;
			}
			else {
				Move();	//�ړ��������Ă�
			}
		}
		//�ҋ@��Ԃֈڍs
		else {
			mState = EIDLE;
		}
		break;

	case EDASH:	//�_�b�V�����
		//���N���b�N�ōU��1�ֈڍs
		if (CKey::Once(VK_LBUTTON)) {
			mState = EATTACK_2;
		}
		//SPACE�L�[������������ɕK�v�ȗʂ̃X�^�~�i������Ƃ�����ֈڍs
		else if (CKey::Once(VK_SPACE) && mStamina >= AVOID_STAMINA) {
			mState = EAVOID;
		}
		//E�L�[�������ƃA�C�e���g�p
		else if (CKey::Once('E')) {
			mState = EITEMUSE;
		}
		//WASD�L�[�������ƈړ�
		else if (CKey::Push('W') || CKey::Push('A') || CKey::Push('S') || CKey::Push('D')) {
			//SHIFT�L�[�������Ă���ƃ_�b�V��
			if (CKey::Push(VK_SHIFT)) {
				Dash();	//�_�b�V���������Ă�
			}
			else{
				mState = EMOVE;
			}
		}
		//�ҋ@��Ԃֈڍs
		else {
			mState = EIDLE;
		}
		break;

	case EAVOID:	//������
		Avoid();	//����������Ă�
		if (mAvoid == false) {
			//����I����WASD�L�[��������Ă���ƈړ�
			if (CKey::Push('W') || CKey::Push('A') || CKey::Push('S') || CKey::Push('D')) {
				//SHIFT�L�[�������Ă���ƃ_�b�V���ֈڍs
				if (CKey::Push(VK_SHIFT)) {
					mState = EDASH;
				}
				else {
					mState = EMOVE;
				}
			}
			//�ҋ@��Ԃֈڍs
			else {
				mState = EIDLE;
			}	
		}
		break;

	case EDEATH:	//���S���
		Death();	//���S�������Ă�
		break;

	case EITEMUSE:	//�A�C�e���g�p��
		ItemUse();	//�A�C�e���g�p�������Ă�
		break;
	}

	//�_�b�V���A��������Ă��Ȃ���Ԃ̎��X�^�~�i���񕜂�����
	if (mState != EDASH && mState != EAVOID && mStamina < STAMINA_MAX) {
		mStamina++;
	}

	//���G���Ԃ̃J�E���g�_�E��
	if (mInvincibleTime > 0) {
		mInvincibleTime--;
	}
	//���G���Ԑ؂�
	else {
		mInvincibleFlag = false;	//���G��ԏI��
	}

	//���W�ړ�
	mPosition += mMove2;

	mMove2 = mMove2 * GRAVITY;

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
	mSpeed = 0.0f;

	//�̗͂�0�ɂȂ�Ǝ��S
	if (mHp <= 0) {
		mState = EDEATH;	//���S��Ԃֈڍs
		mHp = 0;
	}

	if (mHp > HP_MAX)mHp = HP_MAX;

	//��ō폜����
	//////////////////////////////
#ifdef _DEBUG
	//���^�[���������ƕ���
	if (mHp<=0&&CKey::Once(VK_RETURN)) {
		mHp = HP_MAX;
		mState = EIDLE;
	}
#endif
	//////////////////////////////

	//�����_
	Camera.SetTarget(mPosition);

	CXCharacter::Update();
}

void CXPlayer::Render2D()
{
	//2D�`��J�n
	CUtil::Start2D(0, 800, 0, 600);
	/*
	//�̗͂�����ƕ�����������A������
	mFont.DrawString("0", 400 - GAUGE_WID_MAX * (1.0f - hpRate), 575.0f, hpGaugeWid, 15);
	//�X�^�~�i������ƕ�����������A������
	mFont.DrawString("1", 400 - GAUGE_WID_MAX * (1.0f - staminaRate), 535.0f, staminaGaugeWid, 15);
	*/

	//�̗͂̊���
	float hpRate = (float)mHp / (float)HP_MAX;
	//�̗̓Q�[�W�̕�
	float hpGaugeWid = GAUGE_WID_MAX * hpRate;

	//�X�^�~�i�̊���
	float staminaRate = (float)mStamina / (float)STAMINA_MAX;
	//�X�^�~�i�Q�[�W�̕�
	float staminaGaugeWid = GAUGE_WID_MAX * staminaRate;

	mTexture.Draw(20, GAUGE_WID_MAX, 560, 590, 210, 290, 63, 0);	//�Q�[�W�w�i
	mTexture.Draw(20, hpGaugeWid, 560, 590, 0, 0, 0, 0);			//�̗̓Q�[�W

	mTexture.Draw(20, GAUGE_WID_MAX, 520, 550, 210, 290, 63, 0);	//�Q�[�W�w�i
	mTexture.Draw(20, staminaGaugeWid, 520, 550, 110, 190, 63, 0);	//�X�^�~�i�Q�[�W

#ifdef _DEBUG
	char buf[64];

	sprintf(buf, "INVICIBLETIME:%d", mInvincibleTime);
	mFont.DrawString(buf, 50, 150, 10, 12);

	switch (mItemSelect) {
	case 1:
		sprintf(buf, "SELECTITEM:EMPTY");
		break;
	case 2:
		sprintf(buf, "SELECTITEM:TRAP");
		break;
	case 3:
		sprintf(buf, "SELECTITEM:PORTION");
		break;
	}
	mFont.DrawString(buf, 50, 50, 10, 12);
#endif

	//2D�̕`��I��
	CUtil::End2D();
}

void CXPlayer::Collision(CCollider* m, CCollider* o)
{
	//���������R���C�_
	if (m->mType == CCollider::ESPHERE) 
	{
		//���肪���R���C�_
		if (o->mType == CCollider::ESPHERE) 
		{
			//����̐e�̃^�O���G
			if (o->mpParent->mTag == EENEMY) 
			{
				//����̃R���C�_�̃^�O����
				if (o->mTag == CCollider::ESWORD) 
				{
					//���G��Ԃł͂Ȃ��Ƃ�
					if (mInvincibleFlag == false) 
					{
						//�Փ˔���
						if (CCollider::Collision(m, o)) {
							//�L���X�g�ϊ�
							//�G�̍U���̃q�b�g���肪�L���ȂƂ�
							if (((CXEnemy*)(o->mpParent))->mHit == true)
							{
								//�U�����󂯂��ӏ�
								switch (m->mTag) {
								case CCollider::EHEAD:	//��
								case CCollider::EBODY:	//��
									mHp -= DAMAGE;		//�_���[�W���󂯂�
									((CXEnemy*)(o->mpParent))->mHit = false; //�G�̍U���̃q�b�g������I��������
									break;
								}
							}
						}
					}
				}
				//���蔲���h�~
				//�v���C���[�̃{�f�B�ƓG�̃{�f�B���������Ă���Ƃ�
				if (m->mTag == CCollider::EBODY && o->mTag == CCollider::EBODY) {
					CVector adjust;
					if (CCollider::CollisionAdjust(m, o, &adjust)) {
						//�G���X�^����Ԃł͂Ȃ��Ƃ��A�v���C���[���U��2��Ԃł͂Ȃ��Ƃ�
						if (((CXEnemy*)(o->mpParent))->mState != CXEnemy::ESTUN && mState != EATTACK_2) {
							//�G�̃|�W�V�����𒲐�
							CXEnemy* Enemy = (CXEnemy*)o->mpParent;
							Enemy->SetPos(Enemy->GetPos() + adjust);
						}
						else{
							//�v���C���[�̃|�W�V�����𒲐�
							mPosition -= adjust;
						}
					}
				}
			}
		}
	}
}

CXPlayer* CXPlayer::GetInstance()
{
	return mInstance;
}

//�ҋ@����
void CXPlayer::Idle()
{
	ChangeAnimation(0, true, 60);	//�ҋ@���[�V����
}

//�ړ�����
void CXPlayer::Move()
{
	ChangeAnimation(1, true, 60);
	//�_�b�V�����ɃX�s�[�h���㏑�����Ȃ��p
	if (mState == EMOVE) {
		mSpeed = SPEED_DEFAULT;
	}

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
	mMoveKeep = mMoveDir;	//MoveDir�ۑ�
	mMove2 = mMoveDir * mSpeed;
}

//�_�b�V������
void CXPlayer::Dash()
{
	//�X�^�~�i���c���Ă���Ƃ�
	if (mStamina > 0) {
		mSpeed = SPEED_DASH_HIGH;
		mStamina--;
	}
	//�X�^�~�i�؂���
	else {
		mSpeed = SPEED_DASH_LOW;
	}

	Move();	//�ړ��������Ă�
}

//�U��1����
void CXPlayer::Attack_1()
{
	if (mAttackFlag_1 == false) {
		ChangeAnimation(3, true, 20);
		mAttackFlag_1 = true;
		mGraceTime = 0;
		mAttackFlag_Once = true;
	}

	if (mGraceTime > 0) {
		mGraceTime--;	//��t���Ԃ�����
	}

	if (mAnimationIndex == 3)
	{
		//�q�b�g���蔭��
		if (mAnimationFrame == 5) {
			mHit = true;
		}
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mHit = false; //�q�b�g����I��
			ChangeAnimation(4, false, 30);
			mGraceTime = GRACETIME;	//��t���Ԃ�����
		}
	}
	else if (mAnimationIndex == 4)
	{
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mAttackFlag_1 = false;
		}
	}

	mMoveDir = CXEnemy::GetInstance()->mPosition - mPosition;
	mMoveDir = mMoveDir.Normalize();
}

//�U��2����
void CXPlayer::Attack_2()
{
	if (mAttackFlag_2 == false) {
		ChangeAnimation(7, true, 30);
		mAttackFlag_2 = true;
		mAttack2Speed = ATTACK2_FIRSTSPEED;
		mAttackFlag_Once = true;
	}

	if (mAnimationIndex == 7)
	{
		//�q�b�g���蔭��
		if (mAnimationFrame == 5) {
			mHit = true;
		}
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mHit = false; //�q�b�g����I��
			ChangeAnimation(8, false, 30);
		}
	}
	else if (mAnimationIndex == 8)
	{
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mAttackFlag_2 = false;
			//mHit = false;
		}
	}

	//�ړ��ʐ��K���@��������Ȃ��Ǝ΂߈ړ��������Ȃ��Ă��܂��̂Œ���
	//�W�����v���Ȃǂ�Y���𐳋K�����Ȃ��悤����
	mMoveDir = CXEnemy::GetInstance()->mPosition - mPosition;
	mMoveDir = mMoveDir.Normalize();

	mMove2 = mMoveDir * mAttack2Speed;
	mAttack2Speed = mAttack2Speed * GRAVITY;
}

//�U��3����
void CXPlayer::Attack_3()
{
	if (mAttackFlag_3 == false) {
		ChangeAnimation(5, true, 15);
		mAttackFlag_3 = true;
		mAttackFlag_Once = true;
	}

	if (mAnimationIndex == 5)
	{
		//�q�b�g���蔭��
		if (mAnimationFrame == 0) {
			mHit = true;
		}
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mHit = false; //�q�b�g����I��
			ChangeAnimation(6, false, 30);
		}
	}
	else if (mAnimationIndex == 6)
	{
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mAttackFlag_3 = false;
			mAttackFlag_1 = false;
			mHit = false;
		}
	}
}

//�������
void CXPlayer::Avoid()
{
	if (mAvoid == false) {
		mAvoid = true;							//���
		mStamina -= AVOID_STAMINA;				//�X�^�~�i����	
		mAvoidTime = AVOID_TIME;				//�������
		mAvoidSpeed = AVOID_FIRSTSPEED;			//����
		mInvincibleFlag = true;					//���G���
		mInvincibleTime = INVINCIBLETIME_AVOID;	//���G����
	}

	mMove2 = mMoveKeep * mAvoidSpeed;
	mAvoidSpeed = mAvoidSpeed * GRAVITY;	//�X�s�[�h�������Ă���

	//������ԃJ�E���g�_�E��
	mAvoidTime--;
	//������Ԑ؂�
	if (mAvoidTime <= 0) {
		mAvoid = false;	//����I��
	}
}

//���S����
void CXPlayer::Death()
{
	ChangeAnimation(11, false, 60);	//�|���A�j���[�V����
}

//�A�C�e���g�p����
void CXPlayer::ItemUse()
{
	switch (mItemSelect) {
	case EEMPTY: //��
		break;

	case ETRAP:	//�
	{
		//㩐���
		CTrap* trap = new CTrap;
		trap->SetPos(mPosition);
		trap->SetRot(mRotation);
		trap->Update();
	}
	break;

	case EPORTION: //��
		//�̗͉�
		mHp += HEAL_AMOUNT;
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
			if (mItemSelect == mItemTail) {
				mItemSelect = mItemHead + 1;
			}
		}
		//������
		else {
			mItemSelect--;
			if (mItemSelect == mItemHead) {
				mItemSelect = mItemTail - 1;
			}
		}
	}
}


