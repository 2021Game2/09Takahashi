#pragma once
#include "CScene.h"

//リザルト画面のシーンクラス
class CSceneResult :public CScene {
private:
	int mNewRecord;	//新記録の順位を記録

	int count; //文字点滅用カウント

	//フェード
	enum EFade {
		EFADE_STOP = 0,	//フェード停止
		EFADE_IN,		//フェードイン
		EFADE_OUT		//フェードアウト
	};
	EFade mFade; //フェード判断用

	EScene mSceneTransitionKeep; //シーンの遷移先を保存する

	bool mIsButtonPush; //ボタンを押しているとtrue

	//リザルト画面の状態
	enum EResultState {
		CLEARTIME_DISPLAY = 0,	//クリアタイム表示
		RANKING_CHANGE			//ランキング表示入れ替え
	};
	EResultState mResultState;	//リザルト画面の状態を判断

	void ClearTimeDisplay();	//クリアタイム表示状態の処理
	void RankingChange();		//ランキング表示入れ替え状態の処理

	float mClearTimeCountUp;	//クリアタイムをカウントアップする用

	bool mIsRecordSort;			//レコードのソートが行われていたらtrue

	float mRecordTmp[6];		//ランキング表示用、ソートを行う前のレコードを保持
	float mClearTimePosX[6];	//クリアタイムを表示する位置X
	float mClearTimePosY[6];	//クリアタイムを表示する位置Y
	float mRankingPosY[6];		//順位ごとの表示する座標Y、表示入れ替えの際に移動の目標地点として使用
	bool mIsRankingChange;		//ランキングの表示を入れ替えるか判断するフラグ、trueで入れ替え発生

	int mSceneStayCount;		//シーン移動を遅らせる用のカウント

	float mRecord[6];			//ファイルから読み取ったレコードを保持する
	bool mIsNoData;				//レコードが保存されたファイルを読み込んだ際、データが存在しなければtrue

public:
	CSceneResult();	//デフォルトコンストラクタ

	void Init();	//初期化処理
	void Update();	//更新処理
	void Render();	//描画処理

	EScene GetNextScene(); //次のシーンを取得
};