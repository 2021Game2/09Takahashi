#ifndef CRES_H
#define CRES_H
#include "CModelX.h"
#include "CModel.h"
#include "CText.h"
#include "CTexture.h"
#include "CSound.h"

//���\�[�X�N���X
class CRes {
private:
	static bool sIsResLoad; //���\�[�X�t�@�C���̓ǂݍ��ݔ��f�p
public:
	CRes();

	//���f���n
	static CModelX sModelXPlayer;	//�v���C���[���f��
	static CModelX sModelXEnemy;	//�G���f��
	static CModelX sModelXEnemy2;	//�G���f��2

	static CModel sModelMap;		//�}�b�v���f��
	static CModel sModelMap2;		//�}�b�v2���f��
	static CModel sModelTrap;		//㩃��f��
	static CModel sModelCylinder;	//�V�����_�[���f��

	//�摜�n
	static CText sFont;					//�t�H���g�摜
	static CTexture sImageGauge;		//�Q�[�W�摜
	static CTexture sImageGaugeFrame;	//�Q�[�W�g�摜
	static CTexture sImageTarget;		//�^�[�Q�b�g�摜
	static CTexture sImageStun;			//�X�^���摜

	static CTexture sImagePortion;	//�񕜖�摜
	static CTexture sImageTrap;		//㩉摜
	static CTexture sImageNixsign;	//�֎~�}�[�N�摜

	static CTexture sImagePlayerRun;		//�v���C���[�̑���������p�摜
	static CTexture sImagePlayerAttack;		//�v���C���[�̍U�����@�����p�摜
	static CTexture sImagePlayerAvoid;		//�v���C���[�̉����@�����p�摜
	static CTexture sImageMouse;			//�}�E�X��������p�摜
	static CTexture sImageTargetLook;		//��ԋ߂��G�̕����փJ����������������@�̐����p�摜

	static CTexture sImageButtonBack;		//�{�^���w�i�摜
	static CTexture sImageBackGround;		//�w�i�摜

	static CTexture sImageBlack;			//���摜

	static CTexture sImageTitleText;		//�^�C�g���e�L�X�g�摜
	static CTexture sImageTitleBack;		//�^�C�g���w�i�摜

	static CTexture sImageStageClearText;	//�X�e�[�W�N���A�e�L�X�g�摜
	static CTexture sImageGameClearText;	//�Q�[���N���A�e�L�X�g�摜
	static CTexture sImageGameOverText;		//�Q�[���I�[�o�[�e�L�X�g�摜

	//�T�E���h�n
	//SE
	static CSound sSEPlayerWalk;	//�v���C���[�̕��s���̌��ʉ�
	static CSound sSEPlayerRun;		//�v���C���[�̑��s���̌��ʉ�
	static CSound sSEPlayerRunSlow;	//�v���C���[�̑��s��(�X�^�~�i�؂�)�̌��ʉ�
	static CSound sSEPlayerAvoid;	//�v���C���[�̉�����̌��ʉ�
	static CSound sSEAttackHit1;	//�U���q�b�g���̌��ʉ�1
	static CSound sSEAttackHit2;	//�U���q�b�g���̌��ʉ�2
	static CSound sSEPortionUse;	//�񕜃A�C�e���g�p���̌��ʉ�
	static CSound sSETrapUse;		//㩃A�C�e���g�p���̌��ʉ�
	static CSound sSETrapActive;	//㩃A�C�e���쓮���̌��ʉ�
	static CSound sSETransitionTitle;	//�V�[�����^�C�g���ɑJ�ڂ��鎞�̌��ʉ�
	static CSound sSETransitionRecord;	//�V�[�������R�[�h�ɑJ�ڂ��鎞�̌��ʉ�
	//BGM
	static CSound sBGMTitle;	//�^�C�g�����BGM
	static CSound sBGMBattle;	//�퓬��BGM

};

#endif
