//@file GameOverScene.h
#ifndef GAMEOVERSCENE_H_INCLUDED
#define GAMEOVERSCENE_H_INCLUDED

#include "../Engine/VecMath.h"
#include "../Engine/Debug.h"
#include "../Application/Scene.h"
#include "../Engine/Engine.h"

using namespace VecMath;

//どっちを選択しているか
enum class Choose
{
	Game,	//ゲーム
	Title,	//タイトル
};

//ゲームオーバーシーンコンポーネント
class GameOverScene :public Scene
{
public:
	GameOverScene() = default;				//コンストラクタ
	virtual ~GameOverScene() = default;		//デストラクタ

	virtual bool Initialize(Engine& engine)override;				//初期化
	virtual void Update(Engine& engine, float deltaTime)override;	//更新処理


private:
	float Timer = 0;

	int Action = 0;
	bool ClickEnter = false;//エンターを押したかどうか
	GameObjectPtr uiPressSpace;
	GameObjectPtr uiGameover;
	GameObjectPtr uiTitle;		//たいとるテキスト
	GameObjectPtr uiGameStart;	//メインゲームテキスト

	Choose select = Choose::Game;
};




#endif //GameOverScene_H_INCLUDED