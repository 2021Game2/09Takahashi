#ifndef CCAMERA_H
#define CCAMERA_H

#include "CVector.h"

#include "CCharacter.h"
#include "CColliderLine.h"

#define DEF_CAMERA_DIST 7.0f	//カメラの初期距離
#define CAMERA_HEAD_ADJUST 3.0f	//注視点の高さ調整
/*
カメラクラス
*/
class CCamera :public CCharacter {
public:
	//カメラのモード
	enum ECameraMode {
		NORMAL = 0,	//通常モード
		TARGET_LOOK	//ターゲット状態の敵の方へ向くモード
	};
private:
	CColliderLine mColliderLine; //視点から注視点まで伸びる線コライダ

	bool mIsRot;		//一番近い敵の方向へ回転するフラグ
	float mRotRad;		//回転させたい角度

	//アングル遅延用、通常モード時に使用
	float mAngleDelayX;
	float mAngleDelayY;

	ECameraMode mCameraMode;	//カメラのモード判定用
public:
	CCamera();
	//視点
	CVector mEye;
	//注視点
	CVector mCenter;
	//上方向
	CVector mUp;

	CMatrix mMatrix;

	//重複するがカメラ制御用
	CVector mPos;		//位置
	CVector mTarget;	//ターゲット
	float	mAngleX;	//アングル
	float	mAngleY;	//アングル
	float	mDist;	//距離

	//カメラの設定
	//Set(視点, 注視点, 上方向)
	void Set(const CVector& eye, const CVector& center,
		const CVector& up);

	void SetTarget(const CVector& target);

	//カメラ初期化
	void Init();
	//カメラ更新
	void Update();
	//カメラ適用
	void Draw();

	//処理をしないRender
	void Render();

	//ベクトル取得
	CMatrix GetMat();

	//スクリーン座標変換
	//戻り値 表示範囲
	//pOut 結果格納用
	//pos 2Dに変換したいワールド座標
	bool WorldToScreen(CVector* pOut, const CVector& pos);
	
	//衝突判定
	void CCamera::Collision(CCollider* m, CCollider* o);
	void CCamera::TaskCollision();

	//ターゲットになっている敵の方向へカメラを向かせる処理
	void mTargetLook();

	//線形補間
	float mLerp(float start, float point, float rate);

	//カメラモードを設定する
	void SetCameraMode(ECameraMode cameramode);
	//カメラモードを取得する
	CCamera::ECameraMode GetCameraMode();
};

//カメラの外部参照
extern CCamera Camera;
#endif
