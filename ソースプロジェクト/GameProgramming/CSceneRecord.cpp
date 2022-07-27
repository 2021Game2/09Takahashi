#include "CSceneRecord.h"
#include "CKey.h"
#include "CUtil.h"
#include <stdio.h>
#include <corecrt_math.h>
#include "CSceneResult.h"
#include "CRes.h"

#include <fstream>
#include <string>
using namespace std;

#define FILE_RECORD_SAVE "Resource\\Record_Save.txt" //レコードが保存されているテキストファイル
#define DEF_RECORD 600.0f //データが存在しなかった時に設定する値

CSceneRecord::CSceneRecord()
	:mFade(EFADE_STOP)
	, mSceneTransitionKeep(ERECORD)
	, mIsButtonPush(false)
	, mIsNoData(false)
{
	//初期化
	for (int i = 0; i < 6; i++) {
		mRecord[i] = 0.0f;
	}

	//レコードが保存されているテキストファイルを読み込む
	std::ifstream inputfile(FILE_RECORD_SAVE);  // 読み込むファイルのパスを指定
	std::string line;
	for (int i = 0; i < 6; i++) {
		//読み込み
		if (std::getline(inputfile, line)) {
			mRecord[i] = std::stod(line);	//読み込んだレコードを代入する
		}
		//読み込めなかったとき
		else {
			mIsNoData = true;	//データが存在しないのでtrue
			break;				//for文を抜ける
		}
	}

	//読み込んだファイルにデータが存在しなかった場合
	if (mIsNoData == true) {
		//レコードが保存されているテキストファイルへ書き込み
		ofstream outputfile(FILE_RECORD_SAVE);	//書き込むファイルのパスを指定
		for (int i = 0; i < 6; i++) {
			mRecord[i] = DEF_RECORD;			//値を設定する
			outputfile << mRecord[i] << "\n";	//ファイルへ書き込む
		}
		outputfile.close(); //ファイルを閉じる
	}
}

void CSceneRecord::Init()
{
	mScene = ERECORD; //シーンレコード

	mFade = EFADE_IN; //フェードイン

	CRes::sBGMRecord.Repeat(); //BGMを再生する
}

void CSceneRecord::Update()
{
	//左クリックorEnterキーを押したとき
	if (CKey::Once(VK_LBUTTON) || CKey::Once(VK_RETURN)) {
		//ボタンを押していないとき、フェードイン中ではないとき
		if (mIsButtonPush == false && mFade != EFADE_IN) {
			mIsButtonPush = true;	//ボタンを押した
			mFade = EFADE_OUT;		//フェードアウト開始
			mSceneTransitionKeep = ETITLE;	//シーンの遷移先を保存
			CRes::sSESelectBack.Play();	//効果音を再生する
			CRes::sBGMRecord.Stop();	//BGMを停止する
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

void CSceneRecord::Render()
{
	CUtil::Start2D(0, 800, 0, 600);
	char buf[64];
	CRes::sImageRecordBack.Draw(0, 800, 0, 600, 0, 799, 599, 0); //背景画像を表示

	CRes::sFont.DrawString("RECORD", 20, 570, 15, 15);

	for (int i = 0; i < 5; i++) {
		//クリアタイムの上位5つを表示
		sprintf(buf, "%02d:%05.2f", (int)mRecord[i] / 60, fmod(mRecord[i], 60));
		CRes::sFont.DrawString(buf, 460, 380 - i * 70, 15, 20);
	}

	//黒い画像を表示
	CRes::sImageBlack.Draw(0, 800, 0, 600, 1, 1, 1, 1);

	CUtil::End2D();
}

CScene::EScene CSceneRecord::GetNextScene()
{
	return mScene;
}
