#include "CRes.h"

bool CRes::sIsResLoad = false; //�t�@�C���ǂݍ��ݔ��f�p

//���f���n
#define MODEL_PLAYER "Resource\\���O�i.x"				//�v���C���[���f��
#define MODEL_ENEMY "Resource\\knight\\knight_low.x"	//�G���f��

#define MODEL_MAP "Resource\\sky.obj", "Resource\\sky.mtl"				//�}�b�v���f��
#define MODEL_MAP2 "Resource\\Colosseum.obj", "Resource\\Colosseum.mtl" //�}�b�v2���f��
#define MODEL_TRAP "Resource\\trap.obj", "Resource\\trap.mtl"			//㩃��f��

//�摜�n
#define FONT "Resource\\FontG.png"	//�t�H���g�摜
#define FONT_ROW 1					//�t�H���g�s��
#define FONT_COL 4096 / 64			//�t�H���g��

#define IMAGE_GAUGE "Resource\\Gauge.png"			//�Q�[�W�摜
#define IMAGE_TARGET "Resource\\Image_Target.png"	//�^�[�Q�b�g�摜

#define IMAGE_PORTION "Resource\\Image_Portion.png"	//�񕜖�摜
#define IMAGE_TRAP "Resource\\Image_Trap.png"		//㩉摜
#define IMAGE_NIXSIGN "Resource\\Image_Nixsign.png"	//�֎~�}�[�N�摜

#define IMAGE_PLAYER_RUN "Resource\\Image_Player_Run.png"		//�v���C���[�̑���������p�摜
#define IMAGE_PLAYER_ATTACK "Resource\\Image_Player_Attack.png" //�v���C���[�̍U�����@�����p�摜
#define IMAGE_PLAYER_AVOID "Resource\\Image_Player_Avoid.png"	//�v���C���[�̉����@�����p�摜
#define IMAGE_MOUSE "Resource\\Image_Mouse.png"					//�}�E�X��������p�摜

#define IMAGE_BUTTONBACK "Resource\\Gauge.png"			//�{�^���w�i�摜
#define IMAGE_BACKGROUND "Resource\\Result_Back.png"	//�w�i�摜

//�T�E���h�n
#define SE_PLAYER_WALK "Resource\\SE_Player_Walk.wav"	//�v���C���[�̕��s���̌��ʉ�
#define SE_PLAYER_RUN "Resource\\SE_Player_Run.wav"		//�v���C���[�̑��s���̌��ʉ�
#define SE_PLAYER_AVOID "Resource\\SE_Player_Avoid.wav"	//�v���C���[�̉�����̌��ʉ�
#define SE_ATTACK_HIT_1 "Resource\\SE_Attack_Hit_1.wav"	//�U���q�b�g���̌��ʉ�1
#define SE_ATTACK_HIT_2 "Resource\\SE_Attack_Hit_2.wav"	//�U���q�b�g���̌��ʉ�2
#define SE_PORTION_USE "Resource\\SE_Portion_Use.wav"	//�񕜃A�C�e���g�p���̌��ʉ�
#define SE_TRAP_USE "Resource\\SE_Trap_Use.wav"			//㩃A�C�e���g�p���̌��ʉ�
#define SE_TRAP_ACTIVE "Resource\\SE_Trap_Active.wav"	//㩃A�C�e���쓮���̌��ʉ�
#define SE_TRANSITION_TITLE "Resource\\SE_Transition_Title.wav" //�V�[�����^�C�g���ɑJ�ڂ��鎞�̌��ʉ�
#define SE_TRANSITION_RECORD "Resource\\SE_Transition_Record.wav" //�V�[�������R�[�h�ɑJ�ڂ��鎞�̌��ʉ�

//���f���n
CModelX CRes::sModelXPlayer;	//�v���C���[���f��
CModelX CRes::sModelXEnemy;		//�G���f��

CModel CRes::sModelMap;			//�}�b�v���f��
CModel CRes::sModelMap2;		//�}�b�v2���f��
CModel CRes::sModelTrap;		//㩃��f��

//�摜�n
CText CRes::sFont;				//�t�H���g�摜

CTexture CRes::sImageGauge;		//�Q�[�W�摜
CTexture CRes::sImageTarget;	//�^�[�Q�b�g�摜

CTexture CRes::sImagePortion;	//�񕜖�摜
CTexture CRes::sImageTrap;		//㩉摜
CTexture CRes::sImageNixsign;	//�֎~�}�[�N�摜

CTexture CRes::sImagePlayerRun;		//�v���C���[�̑���������p�摜
CTexture CRes::sImagePlayerAttack;	//�v���C���[�̍U�����@�����p�摜
CTexture CRes::sImagePlayerAvoid;	//�v���C���[�̉����@�����p�摜
CTexture CRes::sImageMouse;			//�}�E�X��������p�摜

CTexture CRes::sImageButtonBack;	//�{�^���w�i�摜
CTexture CRes::sImageBackGround;	//�w�i�摜

//�T�E���h�n
CSound CRes::sSEPlayerWalk;		//�v���C���[�̕��s���̌��ʉ�
CSound CRes::sSEPlayerRun;		//�v���C���[�̑��s���̌��ʉ�
CSound CRes::sSEPlayerAvoid;	//�v���C���[�̉�����̌��ʉ�
CSound CRes::sSEAttackHit1;		//�U���q�b�g���̌��ʉ�1
CSound CRes::sSEAttackHit2;		//�U���q�b�g���̌��ʉ�2
CSound CRes::sSEPortionUse;		//�񕜃A�C�e���g�p���̌��ʉ�
CSound CRes::sSETrapUse;		//㩃A�C�e���g�p���̌��ʉ�
CSound CRes::sSETrapActive;		//㩃A�C�e���쓮���̌��ʉ�
CSound CRes::sSETransitionTitle;	//�V�[�����^�C�g���ɑJ�ڂ��鎞�̌��ʉ�
CSound CRes::sSETransitionRecord;	//�V�[�������R�[�h�ɑJ�ڂ��鎞�̌��ʉ�

CRes::CRes()
{
	//���\�[�X�t�@�C���ǂݍ��݁A��x�����ʂ�
	if (sIsResLoad == false) {
		sIsResLoad = true;
		//���f���n
		CRes::sModelXPlayer.Load(MODEL_PLAYER);		//�v���C���[���f��
		CRes::sModelXEnemy.Load(MODEL_ENEMY);		//�G���f��

		CRes::sModelMap.Load(MODEL_MAP);			//�}�b�v���f��
		CRes::sModelMap2.Load(MODEL_MAP2);			//�}�b�v2���f��
		CRes::sModelTrap.Load(MODEL_TRAP);			//㩃��f��

		//�摜�n
		CRes::sFont.LoadTexture(FONT, FONT_ROW, FONT_COL);	//�t�H���g�摜
		CRes::sImageGauge.Load(IMAGE_GAUGE);	//�Q�[�W�摜
		CRes::sImageTarget.Load(IMAGE_TARGET);	//�^�[�Q�b�g�摜

		CRes::sImagePortion.Load(IMAGE_PORTION);	//�񕜃A�C�e���摜
		CRes::sImageTrap.Load(IMAGE_TRAP);			//㩃A�C�e���摜
		CRes::sImageNixsign.Load(IMAGE_NIXSIGN);	//�֎~�}�[�N�摜

		CRes::sImagePlayerRun.Load(IMAGE_PLAYER_RUN);		//�v���C���[�̑���������p�摜
		CRes::sImagePlayerAttack.Load(IMAGE_PLAYER_ATTACK);	//�v���C���[�̍U�����@�����p�摜
		CRes::sImagePlayerAvoid.Load(IMAGE_PLAYER_AVOID);	//�v���C���[�̉����@�����p�摜
		CRes::sImageMouse.Load(IMAGE_MOUSE);				//�}�E�X��������p�摜

		CRes::sImageButtonBack.Load(IMAGE_BUTTONBACK);	//�{�^���w�i�摜
		CRes::sImageBackGround.Load(IMAGE_BACKGROUND);	//�w�i�摜

		//�T�E���h�n
		CRes::sSEPlayerWalk.Load(SE_PLAYER_WALK);	//�v���C���[�̕��s���̌��ʉ�
		CRes::sSEPlayerRun.Load(SE_PLAYER_RUN);		//�v���C���[�̑��s���̌��ʉ�
		CRes::sSEPlayerAvoid.Load(SE_PLAYER_AVOID);	//�v���C���[�̉�����̌��ʉ�
		CRes::sSEAttackHit1.Load(SE_ATTACK_HIT_1);	//�U���q�b�g���̌��ʉ�1
		CRes::sSEAttackHit2.Load(SE_ATTACK_HIT_2);	//�U���q�b�g���̌��ʉ�2
		CRes::sSEPortionUse.Load(SE_PORTION_USE);	//�񕜃A�C�e���g�p���̌��ʉ�
		CRes::sSETrapUse.Load(SE_TRAP_USE);			//㩃A�C�e���g�p���̌��ʉ�
		CRes::sSETrapActive.Load(SE_TRAP_ACTIVE);	//㩃A�C�e���쓮���̌��ʉ�
		CRes::sSETransitionTitle.Load(SE_TRANSITION_TITLE);		//�V�[�����^�C�g���ɑJ�ڂ��鎞�̌��ʉ�
		CRes::sSETransitionRecord.Load(SE_TRANSITION_RECORD);	//�V�[�������R�[�h�ɑJ�ڂ��鎞�̌��ʉ�
	}
}
