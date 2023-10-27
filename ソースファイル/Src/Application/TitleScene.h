//@file TitleScene.h
#ifndef TITLESCENE_H_INCLUDED
#define TITLESCENE_H_INCLUDED

#include "../Engine/VecMath.h"
#include "../Engine/Debug.h"
#include "../Application/Scene.h"
#include "../Engine/Engine.h"

using namespace VecMath;

//どっちを選択しているか
enum class Select
{
	Tutorial,	//チュートリアル
	Game,		//メインゲーム
};

//タイトルコンポーネント
class TitleScene :public Scene
{
public:
	TitleScene() = default;				//コンストラクタ
	virtual ~TitleScene() = default;		//デストラクタ

	virtual bool Initialize(Engine& engine)override;				//初期化
	virtual void Update(Engine& engine, float deltaTime)override;	//更新処理


private:
	float Timer = 0;
	bool Click = false;	//ボタンを押したかどうか
	GameObjectPtr uiPressSpace;	//プレススペーステキスト
	GameObjectPtr uiTutorial;	//チュートリアルテキスト
	GameObjectPtr uiGameStart;	//メインゲームテキスト

	GameObjectPtr uiMonster;	//タイトル画像
	GameObjectPtr uiPanic;		//タイトル画像

	//最初にチュートリアルを選択しておく
	Select select = Select::Tutorial;
};




#endif //TitleScene_H_INCLUDED