#include "CRes.h"

bool CRes::sIsResLoad = false; //�t�@�C���ǂݍ��ݔ��f�p

//���f���n
#define MODEL_PLAYER "Resource\\���O�i.x"				//�v���C���[���f��
#define MODEL_ENEMY	"Resource\\knight\\knight_low.x"	//�G���f��
#define MODEL_ENEMY2 "Resource\\felguard\\felguard-X.x"	//�G���f��2

#define MODEL_MAP "Resource\\sky.obj", "Resource\\sky.mtl"				//�}�b�v���f��
#define MODEL_MAP2 "Resource\\Colosseum.obj", "Resource\\Colosseum.mtl" //�}�b�v2���f��
#define MODEL_TRAP "Resource\\trap.obj", "Resource\\trap.mtl"			//㩃��f��
#define MODEL_CYLINDER "Resource\\Cylinder.obj", "Resource\\Cylinder.mtl" //�V�����_�[���f��

//�摜�n
#define FONT "Resource\\FontG.png"	//�t�H���g�摜
#define FONT_ROW 1					//�t�H���g�s��
#define FONT_COL 4096 / 64			//�t�H���g��

#define IMAGE_GAUGE "Resource\\Gauge.png"			//�Q�[�W�摜
#define IMAGE_GAUGE_FRAME "Resource\\Image_Gauge_Frame.png"	//�Q�[�W�g�摜
#define IMAGE_TARGET "Resource\\Image_Target.png"	//�^�[�Q�b�g�摜
#define IMAGE_STUN "Resource\\Image_Stun.png"		//�X�^���摜

#define IMAGE_PORTION "Resource\\Image_Portion.png"	//�񕜖�摜
#define IMAGE_TRAP "Resource\\Image_Trap.png"		//㩉摜
#define IMAGE_NIXSIGN "Resource\\Image_Nixsign.png"	//�֎~�}�[�N�摜

#define IMAGE_PLAYER_RUN "Resource\\Image_Player_Run.png"		//�v���C���[�̑���������p�摜
#define IMAGE_PLAYER_ATTACK "Resource\\Image_Player_Attack.png" //�v���C���[�̍U�����@�����p�摜
#define IMAGE_PLAYER_AVOID "Resource\\Image_Player_Avoid.png"	//�v���C���[�̉����@�����p�摜
#define IMAGE_MOUSE "Resource\\Image_Mouse.png"					//�}�E�X��������p�摜
#define IMAGE_TARGET_LOOK "Resource\\Image_Target_look.png"		//��ԋ߂��G�̕����փJ����������������@�̐����p�摜

#define IMAGE_BUTTONBACK "Resource\\Gauge.png"			//�{�^���w�i�摜
#define IMAGE_BACKGROUND "Resource\\Result_Back.png"	//�w�i�摜

#define IMAGE_BLACK "Resource\\Image_Black.png"				//���摜

#define IMAGE_TITLE_TEXT "Resource\\Image_Title_Text.png"	//�^�C�g���e�L�X�g�摜
#define IMAGE_TITLE_BACK "Resource\\Image_Title_Back.png"	//�^�C�g���w�i�摜
#define IMAGE_RESULT_BACK "Resource\\Image_Result_Back.png"	//���U���g�w�i�摜
#define IMAGE_RECORD_BACK "Resource\\Image_Record_Back.png"	//���R�[�h�w�i�摜

#define IMAGE_STAGECLEAR_TEXT "Resource\\Image_StageClear_Text.png"	//�X�e�[�W�N���A�e�L�X�g�摜
#define IMAGE_GAMECLEAR_TEXT "Resource\\Image_GameClear_Text.png"	//�Q�[���N���A�e�L�X�g�摜
#define IMAGE_GAMEOVER_TEXT "Resource\\Image_GameOver_Text.png"		//�Q�[���I�[�o�[�e�L�X�g�摜

//�T�E���h�n
//SE
#define SE_PLAYER_WALK "Resource\\SE_Player_Walk.wav"		//�v���C���[�̕��s���̌��ʉ�
#define SE_PLAYER_RUN "Resource\\SE_Player_Run.wav"			//�v���C���[�̑��s���̌��ʉ�
#define SE_PLAYER_RUN_SLOW "Resource\\SE_Player_Run_Slow.wav" //�v���C���[�̑��s��(�X�^�~�i�؂�)�̌��ʉ�
#define SE_PLAYER_AVOID "Resource\\SE_Player_Avoid.wav"		//�v���C���[�̉�����̌��ʉ�
#define SE_ATTACK_HIT_1 "Resource\\SE_Attack_Hit_1.wav"		//�U���q�b�g���̌��ʉ�1
#define SE_ATTACK_HIT_2 "Resource\\SE_Attack_Hit_2.wav"		//�U���q�b�g���̌��ʉ�2
#define SE_ITEM_CHANGE "Resource\\SE_Item_Change.wav"		//�A�C�e���ύX���̌��ʉ�
#define SE_ITEM_USE_ERROR "Resource\\SE_Item_Use_Error.wav"	//�A�C�e�����g�p�s�̎��Ɏg�p�{�^�����������Ƃ��̌��ʉ�
#define SE_PORTION_USE "Resource\\SE_Portion_Use.wav"		//�񕜃A�C�e���g�p���̌��ʉ�
#define SE_TRAP_USE "Resource\\SE_Trap_Use.wav"				//㩃A�C�e���g�p���̌��ʉ�
#define SE_TRAP_ACTIVE "Resource\\SE_Trap_Active.wav"		//㩃A�C�e���쓮���̌��ʉ�
#define SE_SELECT "Resource\\SE_Select.wav"					//�Z���N�g���̌��ʉ�
#define SE_SELECT_BACK "Resource\\SE_Select_Back.wav"		//�Z���N�g���̌��ʉ�(���ǂ�)
#define SE_BUTTON_CURSOR "Resource\\SE_Button_CurSor.wav"	//�{�^���ɃJ�[�\�������킹�����̌��ʉ�
#define SE_TARGET_LOOK_ON "Resource\\SE_Target_Look_On.wav"	//�J�����̃��[�hTARGET_LOOK��ON�ɂȂ����Ƃ��̌��ʉ�
#define SE_TARGET_LOOK_OFF "Resource\\SE_Target_Look_Off.wav" //�J�����̃��[�hTARGET_LOOK��OFF�ɂȂ����Ƃ��̌��ʉ�
#define SE_COUNT_UP "Resource\\SE_Count_Up.wav"				//�J�E���g�A�b�v�̌��ʉ�
#define SE_NEWRECORD "Resource\\SE_NewRecord.wav"			//�V�L�^���o�����̌��ʉ�
#define SE_RESULT_END "Resource\\SE_Result_End.wav"			//���U���g��ʏI�����̌��ʉ�
//BGM
#define BGM_TITLE "Resource\\BGM_Title.wav"		//�^�C�g�����BGM
#define BGM_RESULT "Resource\\BGM_Result.wav"	//���U���g���BGM
#define BGM_RECORD "Resource\\BGM_Record.wav"	//���R�[�h���BGM
#define BGM_BATTLE "Resource\\BGM_Battle.wav"	//�퓬��BGM


//���f���n
CModelX CRes::sModelXPlayer;	//�v���C���[���f��
CModelX CRes::sModelXEnemy;		//�G���f��
CModelX CRes::sModelXEnemy2;	//�G���f��2

CModel CRes::sModelMap;			//�}�b�v���f��
CModel CRes::sModelMap2;		//�}�b�v2���f��
CModel CRes::sModelTrap;		//㩃��f��
CModel CRes::sModelCylinder;	//�V�����_�[���f��

//�摜�n
CText CRes::sFont;				//�t�H���g�摜

CTexture CRes::sImageGauge;		//�Q�[�W�摜
CTexture CRes::sImageGaugeFrame;	//�Q�[�W�g�摜
CTexture CRes::sImageTarget;	//�^�[�Q�b�g�摜

CTexture CRes::sImagePortion;	//�񕜖�摜
CTexture CRes::sImageTrap;		//㩉摜
CTexture CRes::sImageNixsign;	//�֎~�}�[�N�摜
CTexture CRes::sImageStun;		//�X�^���摜

CTexture CRes::sImagePlayerRun;		//�v���C���[�̑���������p�摜
CTexture CRes::sImagePlayerAttack;	//�v���C���[�̍U�����@�����p�摜
CTexture CRes::sImagePlayerAvoid;	//�v���C���[�̉����@�����p�摜
CTexture CRes::sImageMouse;			//�}�E�X��������p�摜
CTexture CRes::sImageTargetLook;	//��ԋ߂��G�̕����փJ����������������@�̐����p�摜

CTexture CRes::sImageButtonBack;	//�{�^���w�i�摜
CTexture CRes::sImageBackGround;	//�w�i�摜

CTexture CRes::sImageBlack;			//���摜

CTexture CRes::sImageTitleText;		//�^�C�g���e�L�X�g�摜
CTexture CRes::sImageTitleBack;		//�^�C�g���w�i�摜
CTexture CRes::sImageResultBack;	//���U���g�w�i�摜
CTexture CRes::sImageRecordBack;	//���R�[�h�w�i�摜

CTexture CRes::sImageStageClearText;	//�X�e�[�W�N���A�e�L�X�g�摜
CTexture CRes::sImageGameClearText;		//�Q�[���N���A�e�L�X�g�摜
CTexture CRes::sImageGameOverText;		//�Q�[���I�[�o�[�e�L�X�g�摜

//�T�E���h�n
//SE
CSound CRes::sSEPlayerWalk;		//�v���C���[�̕��s���̌��ʉ�
CSound CRes::sSEPlayerRun;		//�v���C���[�̑��s���̌��ʉ�
CSound CRes::sSEPlayerRunSlow;	//�v���C���[�̑��s��(�X�^�~�i�؂�)�̌��ʉ�
CSound CRes::sSEPlayerAvoid;	//�v���C���[�̉�����̌��ʉ�
CSound CRes::sSEAttackHit1;		//�U���q�b�g���̌��ʉ�1
CSound CRes::sSEAttackHit2;		//�U���q�b�g���̌��ʉ�2
CSound CRes::sSEItemChange;		//�A�C�e���ύX���̌��ʉ�
CSound CRes::sSEItemUseError;	//�A�C�e�����g�p�s�̎��Ɏg�p�{�^�����������Ƃ��̌��ʉ�
CSound CRes::sSEPortionUse;		//�񕜃A�C�e���g�p���̌��ʉ�
CSound CRes::sSETrapUse;		//㩃A�C�e���g�p���̌��ʉ�
CSound CRes::sSETrapActive;		//㩃A�C�e���쓮���̌��ʉ�
CSound CRes::sSESelect;			//�Z���N�g���̌��ʉ�
CSound CRes::sSESelectBack;		//�Z���N�g���̌��ʉ�(���ǂ�)
CSound CRes::sSEButtonCursor;	//�{�^���ɃJ�[�\�������킹�����̌��ʉ�
CSound CRes::sSETargetLookOn;	//�J�����̃��[�hTARGET_LOOK��ON�ɂȂ����Ƃ��̌��ʉ�
CSound CRes::sSETargetLookOff;	//�J�����̃��[�hTARGET_LOOK��OFF�ɂȂ����Ƃ��̌��ʉ�
CSound CRes::sSECountUp;		//�J�E���g�A�b�v�̌��ʉ�
CSound CRes::sSENewRecord;		//�V�L�^���o�����̌��ʉ�
CSound CRes::sSEResultEnd;		//���U���g��ʏI�����̌��ʉ�
//BGM
CSound CRes::sBGMTitle;		//�^�C�g�����BGM
CSound CRes::sBGMResult;	//���U���g���BGM
CSound CRes::sBGMRecord;	//���R�[�h���BGM
CSound CRes::sBGMBattle;	//�퓬��BGM

CRes::CRes()
{
	//���\�[�X�t�@�C���ǂݍ��݁A��x�����ʂ�
	if (sIsResLoad == false) {
		sIsResLoad = true;

		//���f���n
		CRes::sModelXPlayer.Load(MODEL_PLAYER); //�v���C���[���f��

		CRes::sModelXEnemy.Load(MODEL_ENEMY); //�G���f��
		CRes::sModelXEnemy.SeparateAnimationSet(0, 10, 80, "walk");			//1:�ړ�
		CRes::sModelXEnemy.SeparateAnimationSet(0, 1530, 1830, "idle1");	//2:�ҋ@
		CRes::sModelXEnemy.SeparateAnimationSet(0, 10, 80, "walk");			//3:�_�~�[
		CRes::sModelXEnemy.SeparateAnimationSet(0, 10, 80, "walk");			//4:�_�~�[
		CRes::sModelXEnemy.SeparateAnimationSet(0, 10, 80, "walk");			//5:�_�~�[
		CRes::sModelXEnemy.SeparateAnimationSet(0, 10, 80, "walk");			//6:�_�~�[
		CRes::sModelXEnemy.SeparateAnimationSet(0, 440, 520, "attack1_1");	//7:Attack1
		CRes::sModelXEnemy.SeparateAnimationSet(0, 520, 615, "attack2");	//8:Attack2
		CRes::sModelXEnemy.SeparateAnimationSet(0, 10, 80, "walk");			//9:�_�~�[
		CRes::sModelXEnemy.SeparateAnimationSet(0, 10, 80, "walk");			//10:�_�~�[
		CRes::sModelXEnemy.SeparateAnimationSet(0, 1160, 1260, "death1");	//11:�_�E��
		CRes::sModelXEnemy.SeparateAnimationSet(0, 90, 160, "knockback");	//12:�m�b�N�o�b�N
		CRes::sModelXEnemy.SeparateAnimationSet(0, 1120, 1160, "stun");		//13:�X�^��
		CRes::sModelXEnemy.SeparateAnimationSet(0, 170, 220, "dash");		//14:�_�b�V��
		CRes::sModelXEnemy.SeparateAnimationSet(0, 380, 430, "jump");		//15:�W�����v

		CRes::sModelXEnemy2.Load(MODEL_ENEMY2); //�G���f��2
		CRes::sModelXEnemy2.SeparateAnimationSet(0, 0, 100, "Idle");		//1:�ҋ@
		CRes::sModelXEnemy2.SeparateAnimationSet(0, 150, 200, "Attack1");	//2:�U��1
		CRes::sModelXEnemy2.SeparateAnimationSet(0, 200, 260, "AttackIdle");//3:�U���ҋ@
		CRes::sModelXEnemy2.SeparateAnimationSet(0, 260, 350, "Attack2");	//4:�U��2
		CRes::sModelXEnemy2.SeparateAnimationSet(0, 380, 420, "Walk");		//5:�ړ�
		CRes::sModelXEnemy2.SeparateAnimationSet(0, 425, 460, "Run");		//6:����
		CRes::sModelXEnemy2.SeparateAnimationSet(0, 470, 510, "Death1");	//7:���S
		CRes::sModelXEnemy2.SeparateAnimationSet(0, 520, 558, "KnockBack");	//8:�m�b�N�o�b�N
		CRes::sModelXEnemy2.SeparateAnimationSet(0, 520, 562, "Stun");		//9:�X�^��

		CRes::sModelMap.Load(MODEL_MAP);			//�}�b�v���f��
		CRes::sModelMap2.Load(MODEL_MAP2);			//�}�b�v2���f��
		CRes::sModelTrap.Load(MODEL_TRAP);			//㩃��f��
		CRes::sModelCylinder.Load(MODEL_CYLINDER);	//�V�����_�[���f��

		//�摜�n
		CRes::sFont.LoadTexture(FONT, FONT_ROW, FONT_COL);	//�t�H���g�摜
		CRes::sImageGauge.Load(IMAGE_GAUGE);	//�Q�[�W�摜
		CRes::sImageGaugeFrame.Load(IMAGE_GAUGE_FRAME);	//�Q�[�W�g�摜
		CRes::sImageTarget.Load(IMAGE_TARGET);	//�^�[�Q�b�g�摜

		CRes::sImagePortion.Load(IMAGE_PORTION);	//�񕜃A�C�e���摜
		CRes::sImageTrap.Load(IMAGE_TRAP);			//㩃A�C�e���摜
		CRes::sImageNixsign.Load(IMAGE_NIXSIGN);	//�֎~�}�[�N�摜
		CRes::sImageStun.Load(IMAGE_STUN);			//�X�^���摜

		CRes::sImagePlayerRun.Load(IMAGE_PLAYER_RUN);		//�v���C���[�̑���������p�摜
		CRes::sImagePlayerAttack.Load(IMAGE_PLAYER_ATTACK);	//�v���C���[�̍U�����@�����p�摜
		CRes::sImagePlayerAvoid.Load(IMAGE_PLAYER_AVOID);	//�v���C���[�̉����@�����p�摜
		CRes::sImageMouse.Load(IMAGE_MOUSE);				//�}�E�X��������p�摜
		CRes::sImageTargetLook.Load(IMAGE_TARGET_LOOK);		//��ԋ߂��G�̕����փJ����������������@�̐����p�摜

		CRes::sImageButtonBack.Load(IMAGE_BUTTONBACK);	//�{�^���w�i�摜
		CRes::sImageBackGround.Load(IMAGE_BACKGROUND);	//�w�i�摜

		CRes::sImageBlack.Load(IMAGE_BLACK);			//���摜
		CRes::sImageBlack.SetAlpha(0.0f);				//���摜�̃A���t�@�l��ݒ�

		CRes::sImageTitleText.Load(IMAGE_TITLE_TEXT);	//�^�C�g���e�L�X�g�摜
		CRes::sImageTitleBack.Load(IMAGE_TITLE_BACK);	//�^�C�g���w�i�摜
		CRes::sImageResultBack.Load(IMAGE_RESULT_BACK);	//���U���g�w�i�摜
		CRes::sImageRecordBack.Load(IMAGE_RECORD_BACK);	//���R�[�h�w�i�摜

		CRes::sImageStageClearText.Load(IMAGE_STAGECLEAR_TEXT);	//�X�e�[�W�N���A�e�L�X�g�摜
		CRes::sImageGameClearText.Load(IMAGE_GAMECLEAR_TEXT);	//�Q�[���N���A�e�L�X�g�摜
		CRes::sImageGameOverText.Load(IMAGE_GAMEOVER_TEXT);		//�Q�[���I�[�o�[�e�L�X�g�摜

		//�T�E���h�n
		//SE
		CRes::sSEPlayerWalk.Load(SE_PLAYER_WALK);	//�v���C���[�̕��s���̌��ʉ�
		CRes::sSEPlayerRun.Load(SE_PLAYER_RUN);		//�v���C���[�̑��s���̌��ʉ�
		CRes::sSEPlayerRunSlow.Load(SE_PLAYER_RUN_SLOW); //�v���C���[�̑��s��(�X�^�~�i�؂�)�̌��ʉ�
		CRes::sSEPlayerAvoid.Load(SE_PLAYER_AVOID);	//�v���C���[�̉�����̌��ʉ�
		CRes::sSEAttackHit1.Load(SE_ATTACK_HIT_1);	//�U���q�b�g���̌��ʉ�1
		CRes::sSEAttackHit2.Load(SE_ATTACK_HIT_2);	//�U���q�b�g���̌��ʉ�2
		CRes::sSEItemChange.Load(SE_ITEM_CHANGE);	//�A�C�e���ύX���̌��ʉ�
		CRes::sSEItemUseError.Load(SE_ITEM_USE_ERROR); //�A�C�e�����g�p�s�̎��Ɏg�p�{�^�����������Ƃ��̌��ʉ�
		CRes::sSEPortionUse.Load(SE_PORTION_USE);	//�񕜃A�C�e���g�p���̌��ʉ�
		CRes::sSETrapUse.Load(SE_TRAP_USE);			//㩃A�C�e���g�p���̌��ʉ�
		CRes::sSETrapActive.Load(SE_TRAP_ACTIVE);	//㩃A�C�e���쓮���̌��ʉ�
		CRes::sSESelect.Load(SE_SELECT);			//�Z���N�g���̌��ʉ�
		CRes::sSESelectBack.Load(SE_SELECT_BACK);	//�Z���N�g���̌��ʉ�(���ǂ�)
		CRes::sSEButtonCursor.Load(SE_BUTTON_CURSOR);	//�{�^���ɃJ�[�\�������킹�����̌��ʉ�
		CRes::sSETargetLookOn.Load(SE_TARGET_LOOK_ON);	//�J�����̃��[�hTARGET_LOOK��ON�ɂȂ����Ƃ��̌��ʉ�
		CRes::sSETargetLookOff.Load(SE_TARGET_LOOK_OFF);//�J�����̃��[�hTARGET_LOOK��OFF�ɂȂ����Ƃ��̌��ʉ�
		CRes::sSECountUp.Load(SE_COUNT_UP);			//�J�E���g�A�b�v�̌��ʉ�
		CRes::sSENewRecord.Load(SE_NEWRECORD);		//�V�L�^���o�����̌��ʉ�
		CRes::sSEResultEnd.Load(SE_RESULT_END);		//���U���g��ʏI�����̌��ʉ�
		//BGM
		CRes::sBGMTitle.Load(BGM_TITLE);	//�^�C�g�����BGM
		CRes::sBGMResult.Load(BGM_RESULT);	//���U���g���BGM
		CRes::sBGMRecord.Load(BGM_RECORD);	//���R�[�h���BGM
		CRes::sBGMBattle.Load(BGM_BATTLE);	//�퓬��BGM
	}
}
