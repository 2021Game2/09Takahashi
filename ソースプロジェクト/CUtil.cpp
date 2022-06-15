#include "CUtil.h"
#include "glut.h"
#include <corecrt_math.h>

//2D描画スタート
//Start2D(左座標, 右座標, 下座標, 上座標)
void CUtil::Start2D(float left, float right, float bottom, float top)
{
	//モデルビュー行列の退避
	glPushMatrix();
	//モデルビュー行列の初期化
	glLoadIdentity();

	//モデルビュー行列から
	//プロジェクション行列へ切り替え
	glMatrixMode(GL_PROJECTION);
	//プロジェクション行列の退避
	glPushMatrix();
	//プロジェクション行列の初期化
	glLoadIdentity();
	//2D描画の設定
	gluOrtho2D(left, right, bottom, top);
	//Depthテストオフ
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glColor3f(1.0f, 1.0f, 1.0f);
}

void CUtil::End2D()
{
	//プロジェクション行列を戻す
	glPopMatrix();
	//モデルビューモードへ切り替え
	glMatrixMode(GL_MODELVIEW);
	//モデルビュー行列を戻す
	glPopMatrix();
	//Depthテストオン
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}

Check CUtil::GetCheck2D(float tx, float tz, float x, float z, float angle)
{
	Check localcheck;
	float v1x, v1z;
	float v2x, v2z;


	v1x = tx - x;
	v1z = tz - z;
	v2x = sinf(angle);//進行角度ｘ
	v2z = cosf(angle);//進行角度ｚ


	//外積で左右判定
	localcheck.cross = v1x * v2z - v1z * v2x;
	//内積で前後判定
	localcheck.dot = v1x * v2x + v1z * v2z;

	localcheck.dist = sqrtf(v1x * v1x + v1z * v1z);//距離
	localcheck.cos = localcheck.dot / localcheck.dist;//2次元内積を距離で割るとコサインの値になる

	localcheck.turn = (1 - localcheck.cos);

	return localcheck;
}
