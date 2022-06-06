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

//�摜�n
#define EFFECT_ATTACK_HIT "Resource\\Effect_Attack_Hit.png"		//�U���q�b�g���̃G�t�F�N�g�摜
#define EFFECT_PORTION_USE "Resource\\Effect_Portion_Use.png"	//�񕜃A�C�e���g�p���̃G�t�F�N�g�摜
#define TEXWIDTH  8192	//�e�N�X�`����
#define TEXHEIGHT  6144	//�e�N�X�`������

float CSceneGame::mClearTime = 0.0f; //�N���A�܂łɂ�����������

CSceneGame::CSceneGame()
	:mStartTime(0)
	, mEndTime(0)
	, mCountStart(false)
	, mPhase(EPHASE_1)
{
}

CSceneGame::~CSceneGame() {
	CXPlayer::Release();		//�v���C���[���
	CMap::Release();			//�}�b�v���
	CMap2::Release();			//�}�b�v2���
	CEnemyManager::Release();	//�G�Ǘ����
	CTrapManager::Release();	//㩊Ǘ����
}

void CSceneGame::Init() {
	mScene = EGAME; //�V�[���Q�[��

	mCountStart = false;
	mClearTime = 0.0f;

	//�v���C���[����
	CXPlayer::Generate();
	//�v���C���[�̏�����
	CXPlayer::GetInstance()->Init(&CRes::sModelXPlayer);

	//�G�Ǘ�����
	CEnemyManager::Generate();
	//�G�𐶐�����
	CEnemyManager::GetInstance()->EnemyGenerate(1, 1, 100);

	//�}�b�v����
	CMap::Generate();
	//�}�b�v2����
	CMap2::Generate();

	//�g���b�v�Ǘ�����
	CTrapManager::Generate();

	//�J����������
	Camera.Init();

	ShowCursor(false); //�J�[�\����\��

	//�e�̐ݒ�
	float shadowColor[] = { 0.4f, 0.4f, 0.4f, 0.2f };	//�e�̐F
	float lightPos[] = { 50.0f, 160.0f, 50.0f };		//�����̈ʒu
	mShadowMap.Init(TEXWIDTH, TEXHEIGHT, Render, shadowColor, lightPos); //�V���h�E�}�b�v������

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
}

void CSceneGame::Update() {
	//�X�V
	CEnemyManager::GetInstance()->Update(); //�G�Ǘ��X�V
	CTaskManager::Get()->Update(); //�^�X�N�X�V

	//�Փˏ���
	CTaskManager::Get()->TaskCollision();

	//�^�X�N���X�g�폜
	CTaskManager::Get()->Delete();

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

	//��x�����ʂ�
	if (mCountStart == false) {
		mStartTime = clock(); //�v���J�n����������
		//�J����������
		Camera.Init();
		mCountStart = true;
	}

	//���݂̃t�F�[�Y�𔻒f
	switch (mPhase) {
	case EPHASE_1: //�t�F�[�Y1
		//�G���S�Ď��S��ԂɂȂ����Ƃ�
		if (CEnemyManager::GetInstance()->mIsEnemyAllDeath()) {
			CEnemyManager::GetInstance()->EnemyGenerate(2, 0, 100); //�G�𐶐�
			mPhase = EPHASE_2;	//�t�F�[�Y2�ֈڍs
		}
		break;

	case EPHASE_2: //�t�F�[�Y2
		//�G���S�Ď��S��ԂɂȂ����Ƃ�
		if (CEnemyManager::GetInstance()->mIsEnemyAllDeath()) {
			CEnemyManager::GetInstance()->EnemyGenerate(3, 0, 200); //�G�𐶐�
			mPhase = EPHASE_FINAL;	//�ŏI�t�F�[�Y�ֈڍs
		}
		break;

	case EPHASE_FINAL: //�ŏI�t�F�[�Y
		//�G���S�Ď��S��ԂɂȂ����Ƃ�
		if (CEnemyManager::GetInstance()->mIsEnemyAllDeath()) {
			//Enter�L�[�������ƃ��U���g�Ɉڍs����
			if (CKey::Once(VK_RETURN)) {
				//�N���A���Ԃ��L�^
				mClearTime = (float)(mEndTime - mStartTime) / 1000;
				mScene = ERESULT;
				ShowCursor(true); //�J�[�\���\��
			}
		}
		break;
	}

	//�S�Ă̓G�����S��ԂłȂ��Ƃ�
	if (CEnemyManager::GetInstance()->mIsEnemyAllDeath() == false) {
		mEndTime = clock();	//�v���I�����Ԃ�����
	}
	
	//�v���C���[�����S��Ԃ̂Ƃ��AEnter�L�[�Ń^�C�g���Ɉڍs����
	if (CXPlayer::GetInstance()->mState == CXPlayer::EPlayerState::EDEATH) {
		if (CKey::Once(VK_RETURN)) {
			mScene = ETITLE;
			ShowCursor(true); //�J�[�\���\��
		}
	}

	//2D�`��J�n
	CUtil::Start2D(0, 800, 0, 600);

	//������@�̉摜�\��
	CRes::sImagePlayerRun.Draw(110, 190, 20, 100, 0, 255, 255, 0);		//�v���C���[�̑����
	CRes::sImagePlayerAttack.Draw(20, 100, 20, 100, 0, 255, 255, 0);	//�v���C���[�̍U�����@
	CRes::sImagePlayerAvoid.Draw(200, 280, 20, 100, 0, 255, 255, 0);	//�v���C���[�̉����@
	CRes::sImageMouse.Draw(590, 630, 70, 110, 0, 255, 255, 0);			//�E�N���b�N�p
	CRes::sImageMouse.Draw(750, 790, 70, 110, 0, 255, 511, 256);		//�z�C�[���p

	//�v���C���[�����S��ԂɂȂ��GAMEOVER�ƕ\������
	if (CXPlayer::GetInstance()->mState == CXPlayer::EPlayerState::EDEATH) {
		CRes::sFont.DrawString("GAMEOVER", 120, 350, 40, 40);
		CRes::sFont.DrawString("PUSH ENTER", 125, 270, 30, 30);
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
			CRes::sFont.DrawString("CLEAR", 230, 350, 40, 40);
			CRes::sFont.DrawString("PUSH ENTER", 125, 270, 30, 30);
		}
		break;
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
	

