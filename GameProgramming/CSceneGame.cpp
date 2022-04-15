#include "CSceneGame.h"
//
#include "CCamera.h"
//
#include "CUtil.h"
//
#include "CRes.h"

#include "Ckey.h"
//
#include "CMaterial.h"
//
#include "CCollisionManager.h"
//
#include "CMap2.h"
//
#include "CTrap.h"
//
#include "CTrapManager.h"
//
#include "CEffect.h"
//
#include "CEffect2.h"
//
#include "CSound.h"
//
#include "CInput.h"
//
//CMatrix Matrix;

//�摜�n
#define FONT "Resource\\FontG.png" //�t�H���g
#define EFFECT_ATTACK_HIT "Resource\\Effect_Attack_Hit.png"		//�U���q�b�g���̃G�t�F�N�g�摜
#define EFFECT_PORTION_USE "Resource\\Effect_Portion_Use.png"	//�񕜃A�C�e���g�p���̃G�t�F�N�g�摜
#define TEXWIDTH  8192	//�e�N�X�`����
#define TEXHEIGHT  6144	//�e�N�X�`������

//���f���n
#define MODEL_ENEMY "Resource\\knight\\knight_low.x" //�G���f��
#define MODEL_MAP "Resource\\Colosseum.obj", "Resource\\Colosseum.mtl" //�}�b�v���f��

//�T�E���h�n
#define SE_PLAYER_WALK "Resource\\SE_Player_Walk.wav"	//�v���C���[�̕��s���̌��ʉ�
#define SE_PLAYER_RUN "Resource\\SE_Player_Run.wav"		//�v���C���[�̑��s���̌��ʉ�
#define SE_PLAYER_AVOID "Resource\\SE_Player_Avoid.wav"	//�v���C���[�̉�����̌��ʉ�
#define SE_KNIGHT_WALK "Resource\\SE_Knight_Walk.wav"	//�G(�i�C�g)�̕��s���̌��ʉ�
#define SE_KNIGHT_RUN "Resource\\SE_Knight_Run.wav"		//�G(�i�C�g)�̑��s���̌��ʉ�
#define SE_ATTACK_HIT_1 "Resource\\SE_Attack_Hit_1.wav"	//�U���q�b�g���̌��ʉ�1
#define SE_ATTACK_HIT_2 "Resource\\SE_Attack_Hit_2.wav"	//�U���q�b�g���̌��ʉ�2
#define SE_PORTION_USE "Resource\\SE_Portion_Use.wav"	//�񕜃A�C�e���g�p���̌��ʉ�
#define SE_TRAP_USE "Resource\\SE_Trap_Use.wav"			//㩃A�C�e���g�p���̌��ʉ�
#define SE_TRAP_ACTIVE "Resource\\SE_Trap_Active.wav"	//㩃A�C�e���쓮���̌��ʉ�

CSound SE_Player_Walk;	//�v���C���[�̕��s���̌��ʉ�
CSound SE_Player_Run;	//�v���C���[�̑��s���̌��ʉ�
CSound SE_Player_Avoid;	//�v���C���[�̉�����̌��ʉ�
CSound SE_Knight_Walk;	//�G(�i�C�g)�̕��s���̌��ʉ�
CSound SE_Knight_Run;	//�G(�i�C�g)�̑��s���̌��ʉ�
CSound SE_Attack_Hit_1;	//�U���q�b�g���̌��ʉ�1
CSound SE_Attack_Hit_2;	//�U���q�b�g���̌��ʉ�2
CSound SE_Portion_Use;	//�񕜃A�C�e���g�p���̌��ʉ�
CSound SE_Trap_Use;		//㩃A�C�e���g�p���̌��ʉ�
CSound SE_Trap_Active;	//㩃A�C�e���쓮���̌��ʉ�

float CSceneGame::mClearTime = 0.0f; //�N���A�܂łɂ�����������

CSceneGame::~CSceneGame() {
	CTrapManager::Release();
	CMap2::Release();
}

void CSceneGame::Init() {
	mScene = EGAME;

	mCountStart = false;
	mClearTime = 0.0f;

	//�e�L�X�g�t�H���g�̓ǂݍ��݂Ɛݒ�
	mFont.LoadTexture(FONT, 1, 4096 / 64);

	CRes::sModelX.Load(MODEL_FILE);
	CRes::sKnight.Load(MODEL_ENEMY);
	CRes::sKnight.SeparateAnimationSet(0, 10, 80, "walk");//1:�ړ�
	CRes::sKnight.SeparateAnimationSet(0, 1530, 1830, "idle1");//2:�ҋ@
	CRes::sKnight.SeparateAnimationSet(0, 10, 80, "walk");//3:�_�~�[
	CRes::sKnight.SeparateAnimationSet(0, 10, 80, "walk");//4:�_�~�[
	CRes::sKnight.SeparateAnimationSet(0, 10, 80, "walk");//5:�_�~�[
	CRes::sKnight.SeparateAnimationSet(0, 10, 80, "walk");//6:�_�~�[
	CRes::sKnight.SeparateAnimationSet(0, 440, 520, "attack1_1");//7:Attack1
	CRes::sKnight.SeparateAnimationSet(0, 520, 615, "attack2");//8:Attack2
	CRes::sKnight.SeparateAnimationSet(0, 10, 80, "walk");//9:�_�~�[
	CRes::sKnight.SeparateAnimationSet(0, 10, 80, "walk");//10:�_�~�[
	CRes::sKnight.SeparateAnimationSet(0, 1160, 1260, "death1");//11:�_�E��
	CRes::sKnight.SeparateAnimationSet(0, 90, 160, "knockback");//12:�m�b�N�o�b�N
	CRes::sKnight.SeparateAnimationSet(0, 1120, 1160, "stun");//13:�X�^��
	CRes::sKnight.SeparateAnimationSet(0, 170, 220, "Dash");//14:�_�b�V��
	CRes::sKnight.SeparateAnimationSet(0, 380, 430, "Jump");//15:�W�����v

	//�L�����N�^�[�Ƀ��f����ݒ�
	mPlayer.Init(&CRes::sModelX);
	mPlayer.mPosition = CVector(0.0f, 0.0f, 20.0f);
	mPlayer.mRotation = CVector(0.0f, 180.0f, 0.0f);

	//�G�̏����ݒ�
	mEnemy.Init(&CRes::sKnight);
	mEnemy.mAnimationFrameSize = 1024;
	//�G�̔z�u
	mEnemy.mPosition = CVector(0.0f, 0.0f, -10.0f);
	//mEnemy.ChangeAnimation(2, true, 200);

	//�J����������
	Camera.Init();

	//�}�b�v���f���̓ǂݍ��݁A����
	mMap2.Load(MODEL_MAP);
	new CMap2(&mMap2, CVector(0.0f, -5.0f, 0.0f),
		CVector(), CVector(4.0f, 3.0f, 4.0f));

	CTrapManager::Generate();

	ShowCursor(false); //�J�[�\����\��

	//�e�̐ݒ�
	float shadowColor[] = { 0.4f, 0.4f, 0.4f, 0.2f };  //�e�̐F
	float lightPos[] = { 50.0f, 160.0f, 50.0f };  //�����̈ʒu
	mShadowMap.Init(TEXWIDTH, TEXHEIGHT, Render, shadowColor, lightPos);

	//�G�t�F�N�g�摜�ǂݍ���
	if (CEffect::sMaterial.mTexture.mId == 0) {
		CEffect::sMaterial.mTexture.Load(EFFECT_ATTACK_HIT); //�U���q�b�g���̃G�t�F�N�g
		CEffect::sMaterial.mDiffuse[0] = CEffect::sMaterial.mDiffuse[1] =
			CEffect::sMaterial.mDiffuse[2] = CEffect::sMaterial.mDiffuse[3] = 1.0f;
	}
	if (CEffect2::sMaterial.mTexture.mId == 0) {
		CEffect2::sMaterial.mTexture.Load(EFFECT_PORTION_USE); //�񕜃A�C�e���g�p���̃G�t�F�N�g
		CEffect2::sMaterial.mDiffuse[0] = CEffect2::sMaterial.mDiffuse[1] =
			CEffect2::sMaterial.mDiffuse[2] = CEffect2::sMaterial.mDiffuse[3] = 1.0f;
	}

	//���ʉ��ǂݍ���
	SE_Player_Walk.Load(SE_PLAYER_WALK);	//�v���C���[�̕��s���̌��ʉ�
	SE_Player_Run.Load(SE_PLAYER_RUN);		//�v���C���[�̑��s���̌��ʉ�
	SE_Player_Avoid.Load(SE_PLAYER_AVOID);	//�v���C���[�̉�����̌��ʉ�
	SE_Knight_Walk.Load(SE_KNIGHT_WALK);	//�G(�i�C�g)�̕��s���̌��ʉ�
	SE_Knight_Run.Load(SE_KNIGHT_RUN);		//�G(�i�C�g)�̑��s���̌��ʉ�
	SE_Attack_Hit_1.Load(SE_ATTACK_HIT_1);	//�U���q�b�g���̌���1
	SE_Attack_Hit_2.Load(SE_ATTACK_HIT_2);	//�U���q�b�g���̌���2
	SE_Portion_Use.Load(SE_PORTION_USE);	//�񕜃A�C�e���g�p���̌��ʉ�
	SE_Trap_Use.Load(SE_TRAP_USE);			//㩃A�C�e���g�p���̌��ʉ�
	SE_Trap_Active.Load(SE_TRAP_ACTIVE);	//㩃A�C�e���쓮���̌��ʉ�
}

void CSceneGame::Update() {
	//�X�V
	CTaskManager::Get()->Update();

	//�Փˏ���
	//CCollisionManager::Get()->Collision();
	CTaskManager::Get()->TaskCollision();
	/*
	Camera.Update();

	Camera.Render();
	*/
	//�^�X�N���X�g�폜
	CTaskManager::Get()->Delete();
	//�^�X�N�`��
	//CTaskManager::Get()->Render();
	//�J�����`��
	Camera.Draw();
	//�V���h�E�}�b�v�`��
	mShadowMap.Render();
	//�^�X�N2D�`��
	CTaskManager::Get()->Render2D();

#ifdef _DEBUG
	//�R���C�_�̕`��
	CCollisionManager::Get()->Render();
#endif

	//���Ԍv���J�n
	if (mCountStart == false) {
		start = clock();
		mCountStart = true;
	}

	//�G�����S��ԂɂȂ�Ǝ��Ԍv���I��
	if (CXEnemy::GetInstance()->DeathFlag() != true) {
		end = clock();
	}
	else {
		//�N���A���Ԃ��L�^
		mClearTime = (float)(end - start) / 1000;
		//Enter�L�[�������ƃ��U���g�Ɉڍs����
		if (CKey::Once(VK_RETURN)) {
			mScene = ERESULT;
			ShowCursor(true); //�J�[�\���\��
		}
	}

	//�v���C���[�����S��Ԃ̂Ƃ�
	if (CXPlayer::GetInstance()->mState == CXPlayer::EPlayerState::EDEATH) {
		//Enter�L�[�Ń^�C�g���Ɉڍs����
		if (CKey::Once(VK_RETURN)) {
			mScene = ETITLE;
			ShowCursor(true); //�J�[�\���\��
		}
	}

	//2D�`��J�n
	CUtil::Start2D(0, 800, 0, 600);

#ifdef _DEBUG
	//�m�F�p�A��ō폜
	char buf[64];
	//�^�C��
	sprintf(buf, "TIME:%06.2f", (float)(end - start) / 1000);
	mFont.DrawString(buf, 50, 100, 10, 12);

	//���ʉ��̃e�X�g
	if (CKey::Once('1')) {
		SE_Attack_Hit_1.Play();
	}
	else if (CKey::Once('2')) {
		SE_Attack_Hit_2.Play();
	}
	else if (CKey::Once('3')) {
		SE_Trap_Active.Play();
	}
#endif

	if (CXPlayer::GetInstance()->mState == CXPlayer::EPlayerState::EDEATH) {
		mFont.DrawString("GAMEOVER", 120, 300, 40, 40);
		mFont.DrawString("PUSH ENTER", 30, 30, 20, 20);
	}
	else if (CXEnemy::GetInstance()->DeathFlag() == true) {
		mFont.DrawString("CLEAR", 230, 300, 40, 40);
		mFont.DrawString("PUSH ENTER", 30, 30, 20, 20);
	}

	//2D�̕`��I��
	CUtil::End2D();

	//���Z�b�g
	CInput::InputReset();

	return;
}

void Render()
{
	//�^�X�N�`��
	CTaskManager::Get()->Render();
}

CScene::EScene CSceneGame::GetNextScene()
{
	return mScene;
}
	

