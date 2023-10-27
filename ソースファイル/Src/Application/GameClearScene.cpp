//@file GameClearScene.cpp

#include "EasyAudio.h"
#include "GameObject.h"
#include "MainGameScene.h"
#include "GameClearScene.h"
#include "../Engine/Engine.h"
#include "../Engine/AudioSettings.h"
#include "../Application/TitleScene.h"
#include "../Component/Text.h"
#include "../Component/Lerp.h"

//ゲームクリア画面を初期化する

//@retval true  初期化成功
//@retval false 初期化失敗

bool GameClearScene::Initialize(Engine& engine)
{
	//ゲームオブジェクトを削除
	engine.ClearGameObjectList();
	engine.ClearUILayers();
	engine.RemoveParticleEmitterAll();

	//UIレイヤーを作成
	const size_t bgLayer = engine.AddUILayer("Res/UI/title_bg.tga", GL_LINEAR, 10);
	const size_t logoLayer = engine.AddUILayer("Res/UI/gameclear_logo.tga", GL_LINEAR, 10);
	const size_t textLayer = engine.AddUILayer("Res/UI/font_04_2.tga", GL_LINEAR, 10);

	//背景画像
	auto uiBackground = engine.CreateUI<GameObject>(
		bgLayer, "gameclear_bg", 640,360);
	uiBackground->AddSprite({ 0,0,1,1 });

	//ゲームクリアロゴ画像
	uiGameClear = engine.CreateUI<GameObject>(
		logoLayer, "clear_logo", 660, -600);
	uiGameClear->AddSprite({ 0,0,1,1 }, 0, 0, 1.25f);

	//線形保管で画面に登場させる
	auto move = uiGameClear->AddComponent<Lerp>();
	move->TargetX = 660;
	move->TargetY = 400;
	move->MaxTime = 1;
	move->SetCalc = 0;



	//ゲームの開始方法を示すテキスト
	const char strGameClear[] = "Press Space";
	const float fontSizeX = 13;
	const float x =
		640 - static_cast<float>(std::size(strGameClear) - 1) * fontSizeX;

	//スペースボタン
	uiPressSpace = engine.CreateUI<GameObject>(textLayer, "Press Space", x, 10);
	auto textGameClear = uiPressSpace->AddComponent<Text>();
	textGameClear->SetText(strGameClear, 1.7f);

	//フェードインさせる
	engine.StartFadeIn();

	//BGMを再生
	Audio::Play(AudioPlayer::bgm, BGM::stageclear, 1, true);

	engine.GetRandomGenerator().seed(rd());

	engine.SlowSpeed = 1;
	return true;	//初期化成功
}

//タイトル画面の状態を更新する
void GameClearScene::Update(Engine& engine, float deltaTime)
{
	//経過時間を計測
	Timer += deltaTime;


	//フェードアウトが完了したら
	if (engine.GetFadeState() == Engine::FadeState::Closed && Click)
	{
		engine.SetNextScene<TitleScene>();		//タイトルシーンに
	}

	switch (Action)
	{
	case 0:	//拡縮
	{
		if (Timer < 2)
		{
			//サイズを変える
			uiGameClear->SetScale(vec3(1 + Smooth(Timer), 1 + Smooth(Timer), 1));
		}
		else if (Timer > 3)
		{
			Timer = 0;				//時間をリセットする
			Action = std::uniform_int_distribution(0, 2)(engine.GetRandomGenerator());
		}

		break;
	}
	case 1:	//光る
	{
		if (Timer < 2)
		{
			uiGameClear->SetRGB({ 1 + Smooth(Timer),1 + Smooth(Timer),1 + Smooth(Timer) });
		}
		else if (Timer > 3)
		{
			Timer = 0;				//時間をリセットする
			Action = std::uniform_int_distribution(0, 2)(engine.GetRandomGenerator());
		}
		break;
	}
	case 2:
		if (Timer < 2)
		{
			//バリ回転する
			uiGameClear->SetRotation(vec3(Smooth(Timer) * 720));
		}
		else if (Timer > 3)
		{
			Timer = 0;				//時間をリセットする
			Action = std::uniform_int_distribution(0, 2)(engine.GetRandomGenerator());
		}

		break;
	}


	//フェード中は押せない
	if (engine.GetFadeState() != Engine::FadeState::Open)
	{
		return;
	}

	//スペースを押したら
	if (engine.GetKey(GLFW_KEY_SPACE))
	{
		//一回もクリックしていなかったら
		if (!Click)
		{
			Audio::PlayOneShot(SE::Click2, 0.2f);	//効果音を再生
			engine.SetFadeRule("Res/Fade/fade_rule2.tga");
			engine.StartFadeOut();
			Click = true;
		}
	}

}