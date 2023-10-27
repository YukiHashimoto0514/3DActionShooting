//@file GameClearScene.h
#ifndef GAMECLEARSCENE_H_INCLUDED
#define GAMECLEARSCENE_H_INCLUDED

#include "../Engine/VecMath.h"
#include "../Engine/Debug.h"
#include "../Application/Scene.h"
#include "../Engine/Engine.h"

using namespace VecMath;

//ターゲットを追いかけるコンポーネント
class GameClearScene :public Scene
{
public:
	GameClearScene() = default;				//コンストラクタ
	virtual ~GameClearScene() = default;		//デストラクタ

	virtual bool Initialize(Engine& engine)override;				//初期化
	virtual void Update(Engine& engine, float deltaTime)override;	//更新処理


private:
	float Timer = 0;	//イージングで使うタイマー

	bool Click = false;	//一度クリックしたかどうか
	int Action = 1;		//どのアニメーションをするか
	GameObjectPtr uiPressSpace;	//テキストオブジェクト
	GameObjectPtr uiGameClear;	//イメージオブジェクト

	//乱数
	std::random_device rd;

};




#endif //GameClearScene_H_INCLUDED