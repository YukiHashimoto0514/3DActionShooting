//@file GameClearScene.cpp

#include "GameClearScene.h"
#include "MainGameScene.h"
#include "../Component/Text.h"
#include "EasyAudio.h"
#include "../Engine/AudioSettings.h"
#include "GameObject.h"
#include "../Engine/Engine.h"
#include "../Component/Lerp.h"

//ゲームクリア画面を初期化する

//@retval true  初期化成功
//@retval false 初期化失敗

bool GameClearScene::Initialize(Engine& engine)
{
	//ゲームオブジェクトを削除
	engine.ClearGameObjectList();
	engine.ClearUILayers();

	//UIレイヤーを作成
	const size_t bgLayer = engine.AddUILayer("Res/title_bg.tga", GL_LINEAR, 10);
	const size_t logoLayer = engine.AddUILayer("Res/gameclear_logo.tga", GL_LINEAR, 10);
	const size_t textLayer = engine.AddUILayer("Res/font_04_2.tga", GL_LINEAR, 10);

	//背景画像
	auto uiBackground = engine.CreateUI<GameObject>(
		bgLayer, "gameclear_bg", 640,360);
	uiBackground->AddSprite({ 0,0,1,1 });

	//ゲームクリアロゴ画像
	uiGameClear = engine.CreateUI<GameObject>(
		logoLayer, "clear_logo", 660, -600);
	uiGameClear->AddSprite({ 0,0,1,1 }, 0, 0, 1.25f);

	auto move = uiGameClear->AddComponent<Lerp>();
	move->TargetX = 660;
	move->TargetY = 600;
	move->MaxTime = 1;
	move->SetCalc = 0;



	//ゲームの開始方法を示すテキスト
	const char strGameClear[] = "Press Space";
	const float fontSizeX = 15;
	const float x =
		640 - static_cast<float>(std::size(strGameClear) - 1) * fontSizeX;

	//スペースボタン
	uiPressSpace = engine.CreateUI<GameObject>(textLayer, "Press Space", x, 0);
	auto textGameClear = uiPressSpace->AddComponent<Text>();
	textGameClear->SetText(strGameClear, 2);

	//フェードインさせる
	engine.StartFadeIn();
	//BGMを再生
	Audio::Play(AudioPlayer::bgm, BGM::stageclear, 1, true);

	engine.SlowSpeed = 1;
	return true;	//初期化成功
}

//タイトル画面の状態を更新する
void GameClearScene::Update(Engine& engine, float deltaTime)
{
	//経過時間を計測
	Timer += deltaTime;


		if (engine.GetKey(GLFW_KEY_SPACE))
		{
			Audio::PlayOneShot(SE::Click2, 0.2f);	//効果音を再生
			engine.SetNextScene<MainGameScene>();		//タイトルシーンに（仮）
		}

	if (Timer < 2)
	{
		//switch (Action)
		//{
		//case 0://サイズを変える
		//	uiGameClear->SetScale(vec3(1 + Smooth(Timer), 1 + Smooth(Timer),1));
		//	break;

		//case 1://回転させる
		//	uiGameClear->SetRotation();
		//	break;

		//case 2://回転しながらサイズを変える
		//	uiGameClear->Scale = 1 + Smooth(Timer);
		//	uiGameClear->radias = Smooth(Timer) * 360;
		//	break;

		//case 3://発光させる
		//	uiGameClear->Red = 1 + Smooth(Timer);
		//	uiGameClear->Green = 1 + Smooth(Timer);
		//	uiGameClear->Blue = 1 + Smooth(Timer);
		//	break;
		//}
	}
	else if (Timer > 3)
	{
		//NextFlg = true;			//次のシーンに移行できるようにする
		//Timer = 0;				//時間をリセットする
		//Action = rand() % 4;	//ランダムで動きをきめる
	}


}