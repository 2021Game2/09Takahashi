#include "CSceneResult.h"
#include "CKey.h"
#include "CUtil.h"
#include <stdio.h>
#include "CSceneGame.h"
#include "CRes.h"
#include "CCamera.h"

#include <fstream>
#include <string>
using namespace std;
#define FILE_RECORD_SAVE "Resource\\Record_Save.txt" //レコードが保存されているテキストファイル

#define COUNT_UP_FRAME 1.0f/120.0f //カウントアップが完了するまでのフレーム数
#define STAY_FRAME 240 //停止フレーム数

CSceneResult::CSceneResult()
	:mNewRecord(6)
	, count(0)
	, mFade(EFADE_STOP)
	, mSceneTransitionKeep(ERESULT)
	, mIsButtonPush(false)
	, mResultState(CLEARTIME_DISPLAY)
	, mClearTimeCountUp(0.0f)
	, mIsRecordSort(false)
	, mIsRankingChange(false)
	, mSceneStayCount(STAY_FRAME)
{
	//初期化
	for (int i = 0; i < 6; i++) {
		mRecord[i] = 0.0f;
	}

	//レコードが保存されているテキストファイルを読み込む
	std::ifstream inputfile(FILE_RECORD_SAVE);  //読み込むファイルのパスを指定
	std::string line;
	for (int i = 0; i < 6; i++) {
		std::getline(inputfile, line);
		mRecord[i] = std::stod(line);	//読み込んだレコードを代入する
	}

	mRecord[5] = CSceneGame::mClearTime; //クリア時間を入れる
	float tmp = 0; //退避用

	for (int i = 0; i < 6; i++) {
		//最後尾のみ
		if (i == 5) {
			mRecordTmp[i] = mRecord[i];			//ゲームプレイで出た記録を設定
			mClearTimePosX[i] = 460;			//クリアタイムの表示位置Xを設定
			mClearTimePosY[i] = -100;			//クリアタイムの表示位置Yを設定(画面外)
			mRankingPosY[i] = mClearTimePosY[i];//順位ごとの表示位置を設定(クリアタイムの表示位置Yと同一)
		}
		else {
			mRecordTmp[i] = mRecord[i];			//ソートを行う前のレコードを記録
			mClearTimePosX[i] = 460;			//クリアタイムの表示位置Xを設定
			mClearTimePosY[i] = 350 - i * 70;	//クリアタイムの表示位置Yを設定
			mRankingPosY[i] = mClearTimePosY[i];//順位ごとの表示位置を設定(クリアタイムの表示位置Yと同一)
		}
	}

	//クリアタイムが早い順になるように入れ替える
	for (int i = 5; i > 0; i--) {
		//一つ上の順位の記録より小さい時
		if (mRecord[i - 1] > mRecord[i]) {
			//記録されている値を入れ替える
			tmp = mRecord[i];
			mRecord[i] = mRecord[i - 1];
			mRecord[i - 1] = tmp;
			mNewRecord = i - 1;		//新記録の順位を設定
			mIsRecordSort = true;	//ソートを行ったか判断するフラグをtrueにする
		}
	}

	//レコードが保存されているテキストファイルへ書き込み
	ofstream outputfile(FILE_RECORD_SAVE);	//書き込むファイルのパスを指定
	for (int i = 0; i < 6; i++) {
		outputfile << mRecord[i] << "\n"; //ファイルへ書き込む
	}
	outputfile.close(); //ファイルを閉じる

	CRes::sBGMResult.Repeat(); //BGMを再生する
}

void CSceneResult::Init()
{
	mScene = ERESULT; //リザルト

	mFade = EFADE_IN; //フェードイン
}

void CSceneResult::Update()
{
	//リザルト画面の状態を判断
	switch (mResultState) {
	case CLEARTIME_DISPLAY: //クリアタイム表示状態
		ClearTimeDisplay();	//クリアタイム表示処理を呼ぶ
		break;

	case RANKING_CHANGE: //ランキング表示入れ替え状態
		RankingChange(); //ランキング表示入れ替え処理を呼ぶ
		break;
	}

	//ボタンが押されていたとき
	if (mIsButtonPush == true) {
		//シーン移動を遅らせるカウントが0以下のとき
		if (mSceneStayCount-- <= 0) {
			mFade = EFADE_OUT;				//フェードアウト開始
			mSceneTransitionKeep = ETITLE;	//シーンの遷移先を保存する
			CRes::sBGMResult.Stop(); //BGMを停止する
		}
	}

	//フェードを判断
	switch (mFade) {
	case EFADE_STOP: //フェード停止
		break;

	case EFADE_IN: //フェードイン
		if (CRes::sImageBlack.GetAlpha() > 0.0f) {
			//黒い画像のアルファ値を下げる
			CRes::sImageBlack.SetAlpha(-0.02f, true);
		}
		else if (CRes::sImageBlack.GetAlpha() == 0.0f) {
			//フェードインを停止する
			mFade = EFADE_STOP;
			//効果音を再生する
			CRes::sSECountUp.Repeat();
		}
		break;

	case EFADE_OUT: //フェードアウト
		if (CRes::sImageBlack.GetAlpha() < 1.0f) {
			//黒い画像のアルファ値を上げる
			CRes::sImageBlack.SetAlpha(0.02f, true);
		}
		else if (CRes::sImageBlack.GetAlpha() == 1.0f) {
			//保存された遷移先へシーンを移行する
			mScene = mSceneTransitionKeep;
		}
		break;
	}
}

void CSceneResult::Render()
{
	CUtil::Start2D(0, 800, 0, 600);
	char buf[64];

	//背景画像を表示
	CRes::sImageResultBack.Draw(0, 800, 0, 600, 0, 799, 599, 0);

	CRes::sFont.DrawString("RESULT", 20, 570, 15, 15);

	//クリアタイムをカウントアップで表示
	sprintf(buf, "%02d:%05.2f", (int)mClearTimeCountUp / 60, fmod(mClearTimeCountUp, 60));
	CRes::sFont.DrawString(buf, 290, 490, 15, 20);

	for (int i = 0; i < 6; i++) {
		//クリアタイムの上位5つを表示する
		sprintf(buf, "%02d:%05.2f", (int)mRecordTmp[i] / 60, fmod(mRecordTmp[i], 60));
		//ランキング表示入れ替えフラグがtrueのとき
		if (mIsRankingChange == true) {
			//新記録以下の記録の表示位置を変更
			if (mNewRecord <= i) {
				//ゲームをプレイして出た記録
				if (i == 5) {
					//新記録の順位の表示位置へ移動させる
					mClearTimePosY[i] = Camera.mLerp(mClearTimePosY[i], mRankingPosY[mNewRecord], 0.1f);
				}
				//順位が5位の記録
				else if (i == 4) {
					//画面外へ移動させる
					mClearTimePosX[i] = Camera.mLerp(mClearTimePosX[i], 900, 0.1f);
				}
				//上記以外
				else {
					//1つ下の順位の表示位置へ移動させる
					mClearTimePosY[i] = Camera.mLerp(mClearTimePosY[i], mRankingPosY[i + 1], 0.1f);
				}
			}
			//新記録が出た順位の表示位置に文字を表示する
			if (mNewRecord == i) {
				//点滅させる
				if (count % 30 < 15) {
					CRes::sFont.DrawString("NEW", 320, mRankingPosY[mNewRecord], 15, 15);
				}
			}
		}
		//クリアタイムを表示
		CRes::sFont.DrawString(buf, mClearTimePosX[i], mClearTimePosY[i], 15, 20);
	}

	//黒い画像を表示
	CRes::sImageBlack.Draw(0, 800, 0, 600, 1, 1, 1, 1);

	CUtil::End2D();
}

//クリアタイム表示状態の処理
void CSceneResult::ClearTimeDisplay()
{
	//フェードイン中ではないとき
	if (mFade != EFADE_IN) {
		//カウントアップがクリアタイムより低い時
		if (mClearTimeCountUp < CSceneGame::mClearTime) {
			//カウントアップする
			mClearTimeCountUp += Camera.mLerp(0.0f, CSceneGame::mClearTime, COUNT_UP_FRAME);
			//カウントアップがクリアタイムを超えた時
			if (mClearTimeCountUp > CSceneGame::mClearTime) {
				//カウントアップにクリアタイムを代入する
				mClearTimeCountUp = CSceneGame::mClearTime;
				//効果音を停止する
				CRes::sSECountUp.Stop();
			}
		}
		//左クリックorEnterキーを押したとき
		if (CKey::Once(VK_LBUTTON) || CKey::Once(VK_RETURN)) {
			//カウントアップがクリアタイムより低い時
			if (mClearTimeCountUp < CSceneGame::mClearTime) {
				//カウントアップにクリアタイムを代入する
				mClearTimeCountUp = CSceneGame::mClearTime;
				//効果音を停止する
				CRes::sSECountUp.Stop();
			}
			//レコードのソートが行われていたとき
			else if (mIsRecordSort == true) {
				//ランキング表示入れ替え状態へ移行
				mResultState = RANKING_CHANGE;
				//効果音再生
				CRes::sSENewRecord.Play();
			}
			//上記以外はタイトルへ移行
			else {
				//ボタンを押していないとき、フェードイン中ではないとき
				if (mIsButtonPush == false && mFade != EFADE_IN) {
					mIsButtonPush = true;			//ボタンを押した
					CRes::sSEResultEnd.Play();		//効果音を再生する
				}
			}
		}
	}
}

//ランキング表示入れ替え状態の処理
void CSceneResult::RankingChange()
{
	//ランキング表示変更フラグをtrueにする
	mIsRankingChange = true;

	//カウント加算
	count++;

	//左クリックorEnterキーを押したとき
	if (CKey::Once(VK_LBUTTON) || CKey::Once(VK_RETURN)) {
		//ボタンを押していないとき、フェードイン中ではないとき
		if (mIsButtonPush == false && mFade != EFADE_IN) {
			mIsButtonPush = true;			//ボタンを押した
			CRes::sSEResultEnd.Play();		//効果音を再生する
		}
	}
}

CScene::EScene CSceneResult::GetNextScene()
{
	return mScene;
}
