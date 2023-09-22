//@file LoadScene.h

#ifndef LOADSCENE_H_INCLUDED
#define LOADSCENE_H_INCLUDED
#include "Scene.h"
#include "../Component/Text.h"

//ゲームオーバーシーン

class LoadScene :public Scene
{
public:
	LoadScene() = default;				//コンストラクタ
	virtual ~LoadScene() = default;		//デストラクタ

	virtual bool Initialize(Engine& engine)override;
	virtual void Update(Engine& engine, float deltaTime)override;


	GameObjectPtr uiGameOver;	//タイトルのゲームオブジェクト
	bool GameOveralpha = false;	//true=アルファを下げる　false=アルファを揚げる

	GameObjectPtr uiPressSpace;	//タイトルのゲームオブジェクト
	bool Spacealpha = false;	//true=アルファを下げる　false=アルファを揚げる

	GameObjectPtr uiPlanet;		//惑星のゲームオブジェクト

	GameObjectPtr Loadstr;		//文字のゲームオブジェクト

	float Timer = 0;	//Lerpの関数を使うための変数

	int i = 0;
	GameObjectPtr txtLoad;		//文字のゲームオブジェクト



	bool Loadflg = false;
	bool Push = false;//クリック下かどうか
};

#endif //LOADSCENE_H_INCLUDED
