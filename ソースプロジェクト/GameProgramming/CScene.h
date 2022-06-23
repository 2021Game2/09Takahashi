#ifndef CSCENE_H
#define CSCENE_H

#include "CText.h"
#include "CRes.h"

/*
シーンの基底クラス
*/
class CScene {
public:
	enum EScene {
		ETITLE = 0,	//タイトル
		EGAME,		//ゲーム
		EGAME2,		//ゲーム2
		ERESULT,	//リザルト
		ERECORD,	//レコード
	};
	EScene mScene = ETITLE;

	CRes mResource; //リソース
	//CText mFont; //フォント
	//virtual 仮想関数
	//ポリモーフィズムの実装
	//=0 純粋仮想関数の設定
	virtual void Init() = 0;	//純粋仮想関数 初期化処理
	virtual void Update() = 0; //純粋仮想関数 更新処理
	virtual void Render() = 0; //純粋仮想関数 描画処理
	//次のシーンの取得
	virtual EScene GetNextScene() = 0;
	virtual ~CScene() {}	//デストラクタの定義
};


#endif
