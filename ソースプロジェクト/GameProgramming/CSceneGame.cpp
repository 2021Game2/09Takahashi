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
#include "CTrapManager.h"
//
#include "CEffect.h"
//
#include "CEffect2.h"
//
#include "CInput.h"
//
#include "CEnemyManager.h"
//
#include "CXPlayer.h"
//
#include "CMap.h"

//�G�̐�����
#define ENEMY_GENERATE_NUM_PHASE1 1 //�t�F�[�Y1
#define ENEMY_GENERATE_NUM_PHASE2 2 //�t�F�[�Y2
#define ENEMY_GENERATE_NUM_FINALPHASE 3 //�ŏI�t�F�[�Y
//�G�̗̑�
#define ENEMY_HP_PHASE1 200
#define ENEMY_HP_PHASE2 150
#define ENEMY_HP_FINALPHASE 150
//�G�̏����ʒu
#define ENEMY_START_POS_PHASE1 CVector(0.0f,0.0f,-10.0f) //�t�F�[�Y1
#define ENEMY_START_POS_PHASE2 CVector(20.0f,0.0f,0.0f),CVector(-20.0f,0.0f,0.0f) //�t�F�[�Y2
#define ENEMY_START_POS_FINALPHASE CVector(0.0f,0.0f,20.0f),CVector(0.0f,0.0f,0.0f),CVector(0.0f,0.0f,-20.0f) //�ŏI�t�F�[�Y

//�摜�n
#define EFFECT_ATTACK_HIT "Resource\\Effect_Attack_Hit.png"		//�U���q�b�g���̃G�t�F�N�g�摜
#define EFFECT_PORTION_USE "Resource\\Effect_Portion_Use.png"	//�񕜃A�C�e���g�p���̃G�t�F�N�g�摜
#define TEXWIDTH  8192	//�e�N�X�`����
#define TEXHEIGHT  6144	//�e�N�X�`������

float CSceneGame::sClearTime = 0.0f; //�N���A�܂łɂ�����������

CSceneGame::CSceneGame()
	:mStartTime(0)
	, mEndTime(0)
	, mCountStart(false)
	, mPhase(EPHASE_1)
	, mFade(EFADE_IN)
	, mSceneTransitionKeep(EGAME)
	,mShadowMap()
{
}

CSceneGame::~CSceneGame() {
	//�^�C�g���ɖ߂�Ƃ�
	if (mScene == ETITLE) {
		CXPlayer::Release();		//�v���C���[���
		CMap::Release();			//�}�b�v���
		CMap2::Release();			//�}�b�v2���
		CEnemyManager::Release();	//�G�Ǘ����
		CTrapManager::Release();	//㩊Ǘ����
		ShowCursor(true);			//�J�[�\���\��
		Camera.SetCameraMode(CCamera::TARGET_LOOK);	//�J�����̃��[�h��ʏ탂�[�h�ɐݒ�
	}
	//���̃X�e�[�W�֐i�񂾂Ƃ�
	else {
		CEnemyManager::Release();	//�G�Ǘ����
		CTrapManager::Release();	//㩊Ǘ����
	}
}

void CSceneGame::Init() {
	mScene = EGAME; //�V�[���Q�[��

	mCountStart = false;
	sClearTime = 0.0f;

	//�v���C���[����
	CXPlayer::Generate();
	//�v���C���[�̏�����
	CXPlayer::GetInstance()->Init(&CRes::sModelXPlayer);

	//�G�Ǘ�����
	CEnemyManager::Generate();
	//�G�𐶐�����
	CVector pos[ENEMY_GENERATE_NUM_PHASE1] = { ENEMY_START_POS_PHASE1 }; //�G�̏����ʒu
	CEnemyManager::GetInstance()->EnemyGenerate(ENEMY_GENERATE_NUM_PHASE1, CXEnemy::ETYPE_1, ENEMY_HP_PHASE1, pos);

	//�}�b�v����
	CMap::Generate();
	//�}�b�v2����
	CMap2::Generate();

	//�g���b�v�Ǘ�����
	CTrapManager::Generate();

	//�J����������
	Camera.Init();
	//�J�������[�h��ݒ�
	Camera.SetCameraMode(CCamera::TARGET_LOOK);

	ShowCursor(false); //�J�[�\����\��

	//�e�̐ݒ�
	float shadowColor[] = { 0.4f, 0.4f, 0.4f, 0.2f };	//�e�̐F
	float lightPos[] = { 50.0f, 160.0f, 50.0f };		//�����̈ʒu
	mShadowMap.Init(TEXWIDTH, TEXHEIGHT, GlobalRender, shadowColor, lightPos); //�V���h�E�}�b�v������

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

	//BGM�Đ�
	CRes::sBGMBattle.Repeat();
}

void CSceneGame::Update() {
	//�X�V
	CEnemyManager::GetInstance()->Update(); //�G�Ǘ��X�V
	CTrapManager::GetInstance()->Update();	//㩊Ǘ��X�V
	CTaskManager::Get()->Update(); //�^�X�N�X�V

	//�Փˏ���
	CTaskManager::Get()->TaskCollision();

	//�^�X�N���X�g�폜
	CTaskManager::Get()->Delete();

	//��x�����ʂ�
	if (mCountStart == false) {
		mCountStart = true;
		mStartTime = clock(); //�v���J�n����������
		//�J����������
		Camera.Init();
	}

	//���݂̃t�F�[�Y�𔻒f
	switch (mPhase) {
	case EPHASE_1: //�t�F�[�Y1
		//�G���S�Ď��S��ԂɂȂ����Ƃ�
		if (CEnemyManager::GetInstance()->mIsEnemyAllDeath()) {
			CVector pos[ENEMY_GENERATE_NUM_PHASE2] = { ENEMY_START_POS_PHASE2 }; //�G�̏����ʒu
			CEnemyManager::GetInstance()->EnemyGenerate(ENEMY_GENERATE_NUM_PHASE2, CXEnemy::ETYPE_1, ENEMY_HP_PHASE2,pos); //�G�𐶐�
			mPhase = EPHASE_2;	//�t�F�[�Y2�ֈڍs
		}
		break;

	case EPHASE_2: //�t�F�[�Y2
		//�G���S�Ď��S��ԂɂȂ����Ƃ�
		if (CEnemyManager::GetInstance()->mIsEnemyAllDeath()) {
			CVector pos[ENEMY_GENERATE_NUM_FINALPHASE] = { ENEMY_START_POS_FINALPHASE }; //�G�̏����ʒu
			CEnemyManager::GetInstance()->EnemyGenerate(ENEMY_GENERATE_NUM_FINALPHASE, CXEnemy::ETYPE_1, ENEMY_HP_FINALPHASE,pos); //�G�𐶐�
			mPhase = EPHASE_FINAL;	//�ŏI�t�F�[�Y�ֈڍs
		}
		break;

	case EPHASE_FINAL: //�ŏI�t�F�[�Y
		//�G���S�Ď��S��ԂɂȂ����Ƃ�
		if (CEnemyManager::GetInstance()->mIsEnemyAllDeath()) {
			//�J�����̃��[�h��ʏ탂�[�h�ɐݒ肷��
			Camera.SetCameraMode(CCamera::NORMAL);
			//Enter�L�[���������Ƃ�
			if (CKey::Once(VK_RETURN)) {
				//�N���A���Ԃ��L�^
				sClearTime = (float)(mEndTime - mStartTime) / 1000;
				mSceneTransitionKeep = EGAME2; //�V�[���̑J�ڐ��ۑ�
				mFade = EFADE_OUT; //�t�F�[�h�A�E�g�J�n
			}
		}
		break;
	}

	//�S�Ă̓G�����S��ԂłȂ��Ƃ�
	if (CEnemyManager::GetInstance()->mIsEnemyAllDeath() == false) {
		mEndTime = clock();	//�v���I�����Ԃ�����
	}
	
	//�v���C���[�����S��Ԃ̂Ƃ�
	if (CXPlayer::GetInstance()->GetState() == CXPlayer::EPlayerState::EDEATH) {
		//Enter�L�[���������Ƃ�
		if (CKey::Once(VK_RETURN)) {
			mSceneTransitionKeep = ETITLE; //�V�[���̑J�ڐ��ۑ�
			mFade = EFADE_OUT; //�t�F�[�h�A�E�g�J�n
		}
	}

	//�t�F�[�h�𔻒f
	switch (mFade) {
	case EFADE_IN: //�t�F�[�h�C��
		if (CRes::sImageBlack.GetAlpha() > 0.0f) {
			//�����摜�̃A���t�@�l��������
			CRes::sImageBlack.SetAlpha(-0.02f, true);
		}
		break;

	case EFADE_OUT: //�t�F�[�h�A�E�g
		if (CRes::sImageBlack.GetAlpha() < 1.0f) {
			//�����摜�̃A���t�@�l���グ��
			CRes::sImageBlack.SetAlpha(0.02f, true);
		}
		else if (CRes::sImageBlack.GetAlpha() == 1.0f) {
			//�ۑ����ꂽ�J�ڐ�փV�[�����ڍs����
			mScene = mSceneTransitionKeep;
			CRes::sBGMBattle.Stop(); //BGM��~
		}
		break;
	}

	//���Z�b�g
	CInput::InputReset();

	return;
}

void CSceneGame::Render() {
	//�J�����`��
	Camera.Draw();

	//�V���h�E�}�b�v�`��
	mShadowMap.Render();

	//�^�X�N2D�`��
	CTaskManager::Get()->Render2D();

#ifdef _DEBUG
	//�R���C�_�̕`��
	//CCollisionManager::Get()->Render();
#endif

	//2D�`��J�n
	CUtil::Start2D(0, 800, 0, 600);

	//������@�̉摜�\��
	CRes::sImagePlayerRun.Draw(110, 190, 20, 100, 0, 255, 255, 0);		//�v���C���[�̑����
	CRes::sImagePlayerAttack.Draw(20, 100, 20, 100, 0, 255, 255, 0);	//�v���C���[�̍U�����@
	CRes::sImagePlayerAvoid.Draw(200, 280, 20, 100, 0, 255, 255, 0);	//�v���C���[�̉����@
	CRes::sImageMouse.Draw(590, 630, 70, 110, 0, 255, 255, 0);			//�E�N���b�N�p
	CRes::sImageMouse.Draw(750, 790, 70, 110, 0, 255, 511, 256);		//�z�C�[���p
	//��ԋ߂��G�̕����փJ����������������@
	//�J�������[�h�𔻒f�A�^�[�Q�b�g��Ԃ̓G�̕��֌������[�h��������
	if (Camera.GetCameraMode() == CCamera::TARGET_LOOK) {
		CRes::sImageTargetLook.Draw(280, 360, 20, 100, 0, 255, 255, 0);	//ON��ԗp
	}
	else {
		CRes::sImageTargetLook.Draw(280, 360, 20, 100, 256, 511, 255, 0); //OFF��ԗp
	}

	//�v���C���[�����S��ԂɂȂ��GAMEOVER�ƕ\������
	if (CXPlayer::GetInstance()->GetState() == CXPlayer::EPlayerState::EDEATH) {
		//�Q�[���I�[�o�[���̉摜��\��
		CRes::sImageGameOverText.Draw(0, 800, 0, 600, 0, 800, 600, 0);
	}

	//���݂̃t�F�[�Y�𔻒f
	switch (mPhase) {
	case EPHASE_1: //�t�F�[�Y1
		CRes::sFont.DrawString("PHASE1", 660, 550, 10, 10);
		break;

	case EPHASE_2: //�t�F�[�Y2
		CRes::sFont.DrawString("PHASE2", 660, 550, 10, 10);
		break;

	case EPHASE_FINAL: //�ŏI�t�F�[�Y
		CRes::sFont.DrawString("FINALPHASE", 580, 550, 10, 10);
		//�G���S�Ď��S��Ԃ̂Ƃ�CLEAR��PUSHENTER��\������
		if (CEnemyManager::GetInstance()->mIsEnemyAllDeath()) {
			//�X�e�[�W�N���A���̉摜��\��
			CRes::sImageStageClearText.Draw(0, 800, 0, 600, 0, 800, 600, 0);
		}
		break;
	}

	//�����摜��\��
	CRes::sImageBlack.Draw(0, 800, 0, 600, 1, 1, 1, 1);

	//2D�̕`��I��
	CUtil::End2D();
}

void GlobalRender()
{
	//�^�X�N�`��
	CTaskManager::Get()->Render();
}

CScene::EScene CSceneGame::GetNextScene()
{
	return mScene;
}
	

