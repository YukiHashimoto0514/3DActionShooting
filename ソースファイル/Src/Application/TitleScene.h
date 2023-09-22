//@file TitleScene.h
#ifndef TITLESCENE_H_INCLUDED
#define TITLESCENE_H_INCLUDED

#include "../Engine/VecMath.h"
#include "../Engine/Debug.h"
#include "../Application/Scene.h"
#include "../Engine/Engine.h"

using namespace VecMath;

//タイトルコンポーネント
class TitleScene :public Scene
{
public:
	TitleScene() = default;				//コンストラクタ
	virtual ~TitleScene() = default;		//デストラクタ

	virtual bool Initialize(Engine& engine)override;				//初期化
	virtual void Update(Engine& engine, float deltaTime)override;	//更新処理


private:

	bool Click = false;//ボタンを押したかどうか
	GameObjectPtr uiPressSpace;
	GameObjectPtr uiTitle;

};




#endif //TitleScene_H_INCLUDED