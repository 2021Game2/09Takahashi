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
#include "CXEnemy.h"

//CMatrix Matrix;

CSceneGame::~CSceneGame() {

}

void CSceneGame::Init() {
	mScene = EGAME;

	//�e�L�X�g�t�H���g�̓ǂݍ��݂Ɛݒ�
	mFont.LoadTexture("FontG.png", 1, 4096 / 64);

	CRes::sModelX.Load(MODEL_FILE);
	CRes::sKnight.Load("knight\\knight_low.x");
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

	//�G�̏����ݒ�
	mEnemy.Init(&CRes::sKnight);
	mEnemy.mAnimationFrameSize = 1024;
	//�G�̔z�u
	mEnemy.mPosition = CVector(20.0f, 0.0f, 0.0f);
	//mEnemy.ChangeAnimation(2, true, 200);

	//�J����������
	Camera.Init();

	mTime = 0;
	mFrameCount = 1;
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

	//�G�����S��ԂɂȂ�܂Ń^�C�������Z
	if (CXEnemy::GetInstance()->DeathFlag() != true) {
		mFrameCount++;
		if (mFrameCount % 60 == 0)mTime++;
	}
	else {
#ifdef _DEBUG
		if (CKey::Once(VK_RETURN)) {
			mScene = ERESULT;
		}
#endif
	}

	//2D�`��J�n
	CUtil::Start2D(0, 800, 0, 600);

#ifdef _DEBUG
	//�m�F�p�A��ō폜
	char buf[64];
	//�^�C��
	sprintf(buf, "TIME:%d", mTime);
	mFont.DrawString(buf, 50, 100, 10, 12);
#endif

	//2D�̕`��I��
	CUtil::End2D();

	return;
}


CScene::EScene CSceneGame::GetNextScene()
{
	return mScene;
}
	

