#ifndef CUTIL_H
#define CUTIL_H
/*
ユーティリティクラス
共通に使用できるプログラム
*/
typedef struct {
	float dot;//内積
	float cross;//外積
	float dist;//距離
	float cos;//角度
	float turn;//回転
}Check;

class CUtil
{
public:
	//2D描画スタート
	//Start2D(左座標, 右座標, 下座標, 上座標)
	static void Start2D(float left, float right, float bottom, float top);
	static void End2D();

	//疑似2次元ベクトル計算　angleはRadian角度に変換して入れる
	static Check GetCheck2D(float tx, float tz, float x, float z, float angle);
};

#endif
