#include "CXPlayer.h"
#include "CKey.h"
#include "CCamera.h"
#include "CUtil.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "CInput.h"
//#include "CXEnemy.h"
#include "CTrapManager.h"
#include "CSound.h"
#include "CEffect2.h"
#include "CCollisionManager.h"
#include "CEnemyManager.h"

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

#define GAUGE_WID_MAX 400.0f	//�Q�[�W�̕��̍ő�l
#define GAUGE_LEFT 20			//�Q�[�W�`�掞�̍��[

#define FONT "Resource\\FontG.png"				//�t�H���g
#define IMAGE_GAUGE "Resource\\Gauge.png"		//�Q�[�W�摜
#define IMAGE_PORTION "Resource\\Image_Portion.png"	//�񕜖�摜
#define IMAGE_TRAP "Resource\\Image_Trap.png"		//㩉摜
#define IMAGE_NIXSIGN "Resource\\Image_Nixsign.png"	//�֎~�}�[�N�摜

extern CSound SE_Player_Walk;	//�v���C���[�̕��s���̌��ʉ�
extern CSound SE_Player_Run;	//�v���C���[�̑��s���̌��ʉ�
extern CSound SE_Player_Avoid;	//�v���C���[�̉�����̌��ʉ�
extern CSound SE_Attack_Hit_2;	//�U���q�b�g���̌��ʉ�2
extern CSound SE_Portion_Use;	//�񕜃A�C�e���g�p���̌��ʉ�
extern CSound SE_Trap_Use;		//㩃A�C�e���g�p���̌��ʉ�

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
	, mGraceTime(0)
	, mHit(false)
	, mItemSelect(HEAD + 1)
	, mAttackFlag_Once(false)
	,mCombo(0)
	,mPortionQuantity(PORTION_QUANTITY)
	,mKnockBackDir(0.0f,0.0f,0.0f)
	,mAttackDir(0.0f,0.0f,0.0f)
{
	//�^�O�Ƀv���C���[��ݒ肵�܂�
	mTag = EPLAYER;
	mColSphereBody.mTag = CCollider::EBODY;
	mColSphereHead.mTag = CCollider::EHEAD;
	mColSphereSword.mTag = CCollider::ESWORD;

	mState = EIDLE;	//�ҋ@���

	mInstance = this;

	mFont.LoadTexture(FONT, 1, 4096 / 64);

	//�摜�ǂݍ���
	mImageGauge.Load(IMAGE_GAUGE);
	mImagePortion.Load(IMAGE_PORTION);
	mImageTrap.Load(IMAGE_TRAP);
	mImageNixsign.Load(IMAGE_NIXSIGN);
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
			SE_Player_Walk.Repeat(); //���ʉ����Đ�����
		}
		//�E�N���b�N�ŃA�C�e���g�p�\�Ȏ��A�C�e�����g�p����
		else if (CKey::Once(VK_RBUTTON) && mIsItemUse()) {
			mState = EITEMUSE;
		}
		break;

	case EATTACK_1:	//�U��1���
		Attack_1();	//�U��1�̏������Ă�
		break;

	case EATTACK_2:	//�U��2���
		Attack_2();	//�U��2�̏������Ă�
		break;

	case EATTACK_3:	//�U��3��Ԃ̎�
		Attack_3();	//�U��3�̏������Ă�
		break;

	case EMOVE:	//�ړ����
		//���N���b�N�ōU��1�ֈڍs
		if (CKey::Once(VK_LBUTTON)) {
			mState = EATTACK_2;
		}
		//SPACE�L�[������������ɕK�v�ȗʂ̃X�^�~�i������Ƃ�����ֈڍs
		else if (CKey::Once(VK_SPACE) && mStamina >= AVOID_STAMINA) {
			mState = EAVOID;
			SE_Player_Avoid.Play(); //���ʉ����Đ�
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
				SE_Player_Run.Repeat(); //���ʉ����Đ�����
			}
			else {
				Move();	//�ړ��������Ă�
			}
		}
		//�ҋ@��Ԃֈڍs
		else {
			mState = EIDLE;
		}
		//��Ԃ��ڍs�����Ƃ��A���ʉ����~����
		if (mState != EMOVE) {
			SE_Player_Walk.Stop();
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
			SE_Player_Avoid.Play(); //���ʉ����Đ�
		}
		//�E�N���b�N�ŃA�C�e���g�p�\�Ȏ��A�C�e�����g�p����
		else if (CKey::Once(VK_RBUTTON) && mIsItemUse()) {
			mState = EITEMUSE;
		}
		//WASD�L�[�������ƈړ�
		else if (CKey::Push('W') || CKey::Push('A') || CKey::Push('S') || CKey::Push('D')) {
			//SHIFT�L�[�������Ă���ƃ_�b�V��
			if (CKey::Push(VK_SHIFT)) {
				Dash();	//�_�b�V���������Ă�
			}
			else {
				mState = EMOVE;
				SE_Player_Walk.Play(); //���ʉ����Đ�
			}
		}
		//�ҋ@��Ԃֈڍs
		else {
			mState = EIDLE;
		}
		//��Ԃ��ڍs�����Ƃ��A���ʉ����~����
		if (mState != EDASH) {
			SE_Player_Run.Stop();
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
					SE_Player_Run.Play(); //���ʉ����Đ�
				}
				else {
					mState = EMOVE;
					SE_Player_Walk.Play(); //���ʉ����Đ�
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

	case EKNOCKBACK: //�m�b�N�o�b�N���
		//���ʉ����~����
		SE_Player_Walk.Stop();
		SE_Player_Run.Stop();
		SE_Player_Avoid.Stop();
		KnockBack(); //�m�b�N�o�b�N�������Ă�
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

	//����������
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

	//�̗͂��̗͍ő�l�𒴂��Ȃ��悤�ɂ���
	if (mHp > HP_MAX)mHp = HP_MAX;

	//�����_
	Camera.SetTarget(mPosition);

	CXCharacter::Update();
}

void CXPlayer::Render2D()
{
	//2D�`��J�n
	CUtil::Start2D(0, 800, 0, 600);

	float HpRate = (float)mHp / (float)HP_MAX;	//�̗͍ő�l�ɑ΂���A���݂̗̑͂̊���
	float HpGaugeWid = GAUGE_WID_MAX * HpRate;	//�̗̓Q�[�W�̕�

	mImageGauge.Draw(GAUGE_LEFT, GAUGE_LEFT + GAUGE_WID_MAX, 560, 590, 210, 290, 63, 0);	//�̗̓Q�[�W�w�i��\��
	mImageGauge.Draw(GAUGE_LEFT, GAUGE_LEFT + HpGaugeWid, 560, 590, 0, 0, 0, 0);;			//�̗̓Q�[�W��\��

	float StaminaRate = (float)mStamina / (float)STAMINA_MAX;	//�X�^�~�i�ő�l�ɑ΂���A���݂̃X�^�~�i�̊���
	float StaminaGaugeWid = GAUGE_WID_MAX * StaminaRate;		//�X�^�~�i�Q�[�W�̕�

	mImageGauge.Draw(GAUGE_LEFT, GAUGE_LEFT + GAUGE_WID_MAX, 520, 550, 210, 290, 63, 0);	//�X�^�~�i�Q�[�W�w�i��\��
	mImageGauge.Draw(GAUGE_LEFT, GAUGE_LEFT + StaminaGaugeWid, 520, 550, 110, 190, 63, 0);	//�X�^�~�i�Q�[�W��\��

	char buf[64];
	mImageGauge.Draw(630, 750, 30, 150, 310, 390, 63, 0);	//�A�C�e���w�i��\��
	//�I�𒆂̃A�C�e��
	switch (mItemSelect) {	
	case ETRAP: //�
		mImageTrap.Draw(640, 740, 40, 140, 0, 255, 255, 0); //㩉摜��\��
		sprintf(buf, "%d", CTrapManager::GetInstance()->mTrapQuantity); //㩂̏�����
		break;
	case EPORTION: //��
		mImagePortion.Draw(640, 740, 40, 140, 0, 255, 255, 0); //�񕜖�摜��\��
		sprintf(buf, "%d", mPortionQuantity); //�񕜂̏�����
		break;
	}
	//�A�C�e�����g�p�s�\�Ȏ�
	if (mIsItemUse() == false) {
		mImageNixsign.Draw(640, 740, 40, 140, 255, 0, 255, 0); //�֎~�}�[�N�摜��\��
	}

	mFont.DrawString(buf, 730, 50, 15, 15); //�A�C�e���̏�������\��

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
			if (((CXEnemy*)(o->mpParent))->mState == CXEnemy::EDEATH)return;
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
								mMove2 = CVector(0.0f, 0.0f, 0.0f);
								SE_Attack_Hit_2.Play();	//�U���q�b�g���̌��ʉ��Đ�
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
					if (((CXEnemy*)(o->mpParent))->mState != CXEnemy::ESTUN && mState != EATTACK_2) {
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

CXPlayer* CXPlayer::GetInstance()
{
	return mInstance;
}

//���̃R���C�_�̍��W���擾����
CVector CXPlayer::GetSwordColPos()
{
	return mColSphereSword.mpMatrix->GetPos();	//���̃R���C�_�̍��W��Ԃ�
}

//�ҋ@����
void CXPlayer::Idle()
{
	ChangeAnimation(0, true, 60);	//�ҋ@���[�V����
	mCombo = 0;
}

//�ړ�����
void CXPlayer::Move()
{
	ChangeAnimation(1, true, 65);
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
	//�U����������x�����ʂ�
	if (mAttackFlag_1 == false) {
		ChangeAnimation(3, true, 20);
		mAttackFlag_1 = true;
		mGraceTime = 0;
		mAttackFlag_Once = true;
		mCombo++;
		CXEnemy* tEnemy = CEnemyManager::GetInstance()->GetNearEnemy(); //�v���C���[�Ɉ�ԋ߂��G���擾����
		if (tEnemy) {
			mAttackDir = tEnemy->mPosition - mPosition; //�U�����̌��������߂�
		}
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
		if (mAnimationFrame < GRACETIME) {
			if (CKey::Once(VK_LBUTTON)) {
				if (mCombo < COMBO_MAX) {
					mState = EATTACK_3;
				}
				else {
					mState = EATTACK_2;
				}
				mAttackFlag_1 = false;
			}
		}
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mState = EIDLE;
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
	}

	if (mAnimationIndex == 7)
	{
		//�q�b�g���蔭��
		if (mAnimationFrame == 8) {
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
			mState = EIDLE;
			mAttackFlag_2 = false;
		}
	}

	CXEnemy* tEnemy = CEnemyManager::GetInstance()->GetNearEnemy(); //�v���C���[�Ɉ�ԋ߂��G���擾����
	if (tEnemy) {
		mAttackDir = tEnemy->mPosition - mPosition; //���������߂�
	}

	mMoveDir = mAttackDir.Normalize(); //�U�����̌����𐳋K�����đ������

	mMove2 = mMoveDir * mAttack2Speed;
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
		mCombo++;
		CXEnemy* tEnemy = CEnemyManager::GetInstance()->GetNearEnemy(); //�v���C���[�Ɉ�ԋ߂��G���擾����
		if (tEnemy) {
			mAttackDir = tEnemy->mPosition - mPosition; //�U�����̌��������߂�
		}
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
		if (mAnimationFrame < GRACETIME) {
			if (CKey::Once(VK_LBUTTON)) {
				if (mCombo < COMBO_MAX) {
					mState = EATTACK_1;
				}
				else {
					mState = EATTACK_2;
				}
				mAttackFlag_3 = false;
			}
		}
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mState = EIDLE;
			mAttackFlag_3 = false;
		}
	}

	mMoveDir = mAttackDir.Normalize(); //�U�����̌����𐳋K�����đ������
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
	ChangeAnimation(11, false, 50);	//�|���A�j���[�V����
}

//�m�b�N�o�b�N����
void CXPlayer::KnockBack()
{
	ChangeAnimation(2, false, 20);

	//�m�b�N�o�b�N�������K��
	mKnockBackDir = mKnockBackDir.Normalize();

	//�U���𓖂ĂĂ����G�̕����������邽�߂ɑ������
	mMoveDir = mKnockBackDir;

	//�m�b�N�o�b�N������
	mPosition -= mKnockBackDir * KNOCKBACK_AMOUNT;

	if (mAnimationIndex == 2)
	{
		if (mAnimationFrame >= mAnimationFrameSize)
		{
			mState = EIDLE;	//�ҋ@��Ԃֈڍs
			mInvincibleFlag = false; //���G�t���O�𖳌��ɂ���
		}
	}
}

//�A�C�e�����g�p�\�����f����
bool CXPlayer::mIsItemUse()
{
	switch (mItemSelect) {
	case ETRAP:	//�
		//㩃A�C�e�����g�p�\�Ȏ�
		if (CTrapManager::GetInstance()->TrapAvailable()) {
			return true;
		}
		else {
			return false;
		}
		break;

	case EPORTION: //�񕜖�
		//�񕜖�̏�������0��葽���Ƃ��A���݂̗̑͂��̗͍ő�l��������Ă���Ƃ�
		if (mPortionQuantity > 0 && mHp < HP_MAX) {
			return true;
		}
		else {
			return false;
		}
		break;
	}
}

//�A�C�e���g�p����
void CXPlayer::ItemUse()
{
	switch (mItemSelect) {
	case ETRAP:	//�
		//㩃A�C�e�����g�p�\�Ȏ�
			//㩐���
		CTrapManager::GetInstance()->TrapGenerate(mPosition, mRotation);
		//㩃A�C�e���g�p���̌��ʉ����Đ�����
		SE_Trap_Use.Play();
		break;

	case EPORTION: //�񕜖�
			//�񕜖�̏����������炷
		mPortionQuantity--;
		//�̗͂��񕜂�����
		mHp += HEAL_AMOUNT;
		//�񕜃A�C�e���g�p���̃G�t�F�N�g�𐶐�����
		new CEffect2(mPosition, 2.0f, 2.0f, "", 2, 5, 3);
		//�񕜃A�C�e���g�p���̌��ʉ��Đ�
		SE_Portion_Use.Play();
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


