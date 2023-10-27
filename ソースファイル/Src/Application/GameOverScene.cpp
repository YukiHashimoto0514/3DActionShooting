#include "GameOverScene.h"
#include "../Component/Text.h"
#include "EasyAudio.h"
#include "../Engine/AudioSettings.h"
#include "GameObject.h"
#include "../Engine/Engine.h"
#include "../Component/Lerp.h"
#include "TitleScene.h"
#include "MainGameScene.h"

bool GameOverScene::Initialize(Engine& engine)
{
	//ゲームオブジェクトを削除
	engine.ClearGameObjectList();
	engine.ClearUILayers();
	engine.RemoveParticleEmitterAll();

	//UIレイヤーを作成
	const size_t bgLayer = engine.AddUILayer("Res/UI/gameover_bg.tga", GL_LINEAR, 10);
	const size_t logoLayer = engine.AddUILayer("Res/UI/gameover_logo.tga", GL_LINEAR, 10);
	const size_t textLayer = engine.AddUILayer("Res/UI/font_04_2.tga", GL_LINEAR, 10);

	//背景画像
	auto uiBackground = engine.CreateUI<GameObject>(
		bgLayer, "gameclear_bg", 640, 360);
	uiBackground->AddSprite({ 0,0,1,1 });

	//ゲームクリアロゴ画像
	uiGameover = engine.CreateUI<GameObject>(
		logoLayer, "over_logo", 660, -600);
	uiGameover->AddSprite({ 0,0,1,1 }, 0, 0, 1.25f);
	uiGameover->SetAlpha(0);
	auto move = uiGameover->AddComponent<Lerp>();
	move->TargetX = 660;
	move->TargetY = 450;
	move->MaxTime = 2;
	move->SetCalc = 0;



	//ゲームの開始方法を示すテキスト
	const char strGameOver[] = "Press Space";
	const float fontSizeX = 15;
	const float x =
		640 - static_cast<float>(std::size(strGameOver) - 1) * 13;

	//スペースボタン
	uiPressSpace = engine.CreateUI<GameObject>(textLayer, "Press Space", x, 10);
	auto textGameClear = uiPressSpace->AddComponent<Text>();
	textGameClear->SetText(strGameOver, 1.7f);

	const char strTutorial[] = "Retry";
	const float x2 =
		640 - static_cast<float>(std::size(strTutorial) - 1) * fontSizeX;

	uiGameStart = engine.CreateUI<GameObject>(textLayer, "Tutorial Start", x2 - 300, 150);
	auto textTutorial = uiGameStart->AddComponent<Text>();
	textTutorial->SetText(strTutorial, 2);

	//メインゲームテキスト
	const char strGameStart[] = "Title";
	const float x3 =
		640 - static_cast<float>(std::size(strGameStart) - 1) * fontSizeX;
	uiTitle = engine.CreateUI<GameObject>(textLayer, "Game Start", x3 + 300, 150);
	auto textGameStart = uiTitle->AddComponent<Text>();
	textGameStart->SetText(strGameStart, 2);

	uiPressSpace->SetAlpha(0);
	//BGMを再生
	Audio::Play(AudioPlayer::bgm, BGM::gameover, 1, true);
	engine.SlowSpeed = 1;

	ClickEnter = false;
	uiGameStart->SetAlpha(0);
	uiTitle->SetAlpha(0);

	engine.StartFadeIn();
	return true;	//初期化成功
}

void GameOverScene::Update(Engine& engine, float deltaTime)
{

	Timer += deltaTime;


	uiPressSpace->SetRGB({1,0,0});
	//選択している方を点滅させる


	if (Timer <= 1)
	{
		uiGameover->SetAlpha(0);
		return;

	}
	else if (Timer <= 2)
	{
		uiGameover->SetScale({ 100 + Circle(Timer-1) * (1.3f - 100),0,0 });
		uiGameover->SetAlpha(1);
		return;

	}
	else
	{
		uiPressSpace->SetAlpha(1);
		uiGameStart->SetAlpha(1);
		uiTitle->SetAlpha(1);
	}

	//クリックした時にめちゃ点滅させる
	if (ClickEnter)
	{
		uiPressSpace->SetAlpha(Smooth(Timer * 10.0f));
	}

	if (select == Choose::Game)
	{
		uiGameStart->SetAlpha(Smooth(Timer));
	}
	else
	{
		uiTitle->SetAlpha(Smooth(Timer));
	}


	//フェードアウトが完了したら
	if (engine.GetFadeState() == Engine::FadeState::Closed && ClickEnter)
	{
		if (select == Choose::Game)
		{
			engine.SetNextScene<MainGameScene>();	//チュートリアルシーンに移行
		}
		else if (select == Choose::Title)
		{
			engine.SetNextScene<TitleScene>();		//ロードシーンへ移行
		}
	}

	//押されたら変更出来ないようにする
	if (ClickEnter)
	{
		return;
	}
	//左右矢印を押すと選択が切り替わる
	if (engine.GetKey(GLFW_KEY_LEFT) || engine.GetKey(GLFW_KEY_A))
	{
		select = Choose::Game;
		uiTitle->SetAlpha(1);

	}
	else if (engine.GetKey(GLFW_KEY_RIGHT) || engine.GetKey(GLFW_KEY_D))
	{
		select = Choose::Title;
		uiGameStart->SetAlpha(1);

	}

	
	if (engine.GetKey(GLFW_KEY_SPACE))
	{
		//一回もクリックしていなかったら
		if (!ClickEnter)
		{
			Audio::PlayOneShot(SE::Click2, 0.2f);	//効果音を再生
			engine.SetFadeRule("Res/Fade/fade_rule2.tga");
			engine.StartFadeOut();
			ClickEnter = true;
		}
	}

}
