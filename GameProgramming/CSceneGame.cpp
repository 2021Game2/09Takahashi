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
//CMatrix Matrix;

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
	mFont.LoadTexture("Resource\\FontG.png", 1, 4096 / 64);

	CRes::sModelX.Load(MODEL_FILE);
	CRes::sKnight.Load("Resource\\knight\\knight_low.x");
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

	mMap2.Load("Resource\\Colosseum.obj", "Resource\\Colosseum.mtl");
	new CMap2(&mMap2, CVector(0.0f, -5.0f, 0.0f),
		CVector(), CVector(4.0f, 3.0f, 4.0f));

	CTrapManager::Generate();

	ShowCursor(false);
}


void CSceneGame::Update() {
	//�X�V
	CTaskManager::Get()->Update();

	//�Փˏ���
	CCollisionManager::Get()->Collision();

	Camera.Update();

	Camera.Render();

	//�^�X�N���X�g�폜
	CTaskManager::Get()->Delete();
	//�^�X�N�`��
	CTaskManager::Get()->Render();
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
			ShowCursor(true);
		}
	}

	//�v���C���[�����S��Ԃ̂Ƃ�
	if (CXPlayer::GetInstance()->mState == CXPlayer::EPlayerState::EDEATH) {
		//Enter�L�[�Ń^�C�g���Ɉڍs����
		if (CKey::Once(VK_RETURN)) {
			mScene = ETITLE;
			ShowCursor(true);
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

	return;
}


CScene::EScene CSceneGame::GetNextScene()
{
	return mScene;
}
	

