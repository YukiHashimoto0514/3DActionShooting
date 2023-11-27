#include "TitleScene.h"
#include "../Component/Text.h"
#include "EasyAudio.h"
#include "../Engine/AudioSettings.h"
#include "GameObject.h"
#include "../Engine/Engine.h"
#include "../Component/Lerp.h"
#include "MainGameScene.h"
#include "LoadScene.h"
#include "TutorialScene.h"
#include "../Component/TextMove.h"

bool TitleScene::Initialize(Engine& engine)
{
	//ゲームオブジェクトを削除
	engine.ClearGameObjectList();
	engine.ClearUILayers();

	//UIレイヤーを作成
	const size_t bgLayer = engine.AddUILayer("Res/UI/Title_BG_2.tga", GL_LINEAR, 10);
	const size_t MonsterLayer = engine.AddUILayer("Res/UI/Monster.tga", GL_LINEAR, 10);
	const size_t PanicLayer = engine.AddUILayer("Res/UI/Panic.tga", GL_LINEAR, 10);
	const size_t textLayer = engine.AddUILayer("Res/UI/font_04_2.tga", GL_LINEAR, 10);

	const float viewX = engine.GetViewSize().x;
	const float viewY = engine.GetViewSize().y;

	//背景画像
	auto uiBackground = engine.CreateUI<GameObject>(
		bgLayer, "title_bg", viewX * 0.5f, viewY * 0.5f);
	uiBackground->AddSprite({ 0,0,1,1 });

	//タイトルロゴ画像
	uiMonster = engine.CreateUI<GameObject>(
		MonsterLayer, "title_logo", viewX * 0.5f, -600);
	uiMonster->AddSprite({ 0,0,1,1 }, 0, 0,0);
	uiMonster->AddComponent<TextMove>();

	//タイトルロゴをイージングさせるs
	auto move = uiMonster->AddComponent<Lerp>();
	move->TargetX = 460;
	move->TargetY = 600;
	move->MaxTime = 1;
	move->SetCalc = 0;

	//タイトルロゴ画像
	uiPanic = engine.CreateUI<GameObject>(
		PanicLayer, "title_logo", 660, -600);
	uiPanic->AddSprite({ 0,0,1,1 }, 0, 0,0);
	uiPanic->AddComponent<TextMove>();


	//ゲームの開始方法を示すテキスト
	const char strTitle[] = "Press Space";
	const float fontSizeX = 15;
	const float x =
		640 - static_cast<float>(std::size(strTitle) - 1) * 13;

	//スペースボタン
	uiPressSpace = engine.CreateUI<GameObject>(textLayer, "Press Space", x, 10);
	auto textTitle = uiPressSpace->AddComponent<Text>();
	textTitle->SetText(strTitle, 1.7f);
	
	uiPressSpace->SetAlpha(0);

	//チュートリアル開始テキスト
	const char strTutorial[] = "Tutorial";
	const float x2 =
		640 - static_cast<float>(std::size(strTutorial) - 1) * fontSizeX;

	uiTutorial = engine.CreateUI<GameObject>(textLayer, "Tutorial Start", x2 , 250);
	auto textTutorial = uiTutorial->AddComponent<Text>();
	textTutorial->SetText(strTutorial, 2);

	//メインゲームテキスト
	const char strGameStart[] = "Game Start";
	const float x3 =
		640 - static_cast<float>(std::size(strGameStart) - 1) * fontSizeX;

	uiGameStart = engine.CreateUI<GameObject>(textLayer, "Game Start", x3 , 150);
	auto textGameStart = uiGameStart->AddComponent<Text>();
	textGameStart->SetText(strGameStart, 2);

	uiTutorial->SetAlpha(0);
	uiGameStart->SetAlpha(0);

	Audio::PlayOneShot(SE::Wind, 1.0f);
	//BGMを再生
	Audio::Play(AudioPlayer::bgm, BGM::title, 1, true);

	//フェードインさせる
	engine.SetFadeRule("Res/Fade/fade_rule7.tga");
	engine.SetFadeColor("Res/UI/Boarder.tga");
	engine.StartFadeIn();

	engine.SlowSpeed = 1;
	return true;	//初期化成功
}

void TitleScene::Update(Engine& engine, float deltaTime)
{
	Timer += deltaTime * 0.7f;

	//フェードアウトが完了したら
	if (engine.GetFadeState() == Engine::FadeState::Closed)
	{
		if (select == Select::Tutorial)
		{
			engine.SetNextScene<TutorialScene>();	//チュートリアルシーンに移行
		}
		else if(select==Select::Game)
		{
			engine.SetNextScene<LoadScene>();		//ロードシーンへ移行
		}
	}
	//クリックした時にめちゃ点滅させる
	if (Click)
	{
		uiPressSpace->SetAlpha(Smooth(Timer * 10.0f));
	}


	if (Timer <= 1)
	{
		uiMonster->SetRotation({ 11720 + Circle(Timer) * -11720,0,0 });
		uiMonster->SetScale({ 100 + Circle(Timer) * (1 - 100),0,0 });
	}
	else if (Timer <= 2)
	{
		uiPanic->SetPos({ 850,-600 + Circle(Timer-1) * (450-(-600)),0 });
	}
	
	//フェードが完了していなかったら無視
	if (engine.GetFadeState() != Engine::FadeState::Open)
	{
		return;
	}

	//アニメーションが終っていいなかったら
	if (Timer <= 2.5f)
	{
		uiTutorial->SetAlpha(0);
		uiGameStart->SetAlpha(0);
		return;
	}
	else
	{
		uiTutorial->SetAlpha(1);
		uiGameStart->SetAlpha(1);

	}

	//スペースを押したらシーン遷移開始
	if (engine.GetKey(GLFW_KEY_SPACE))
	{
		if (!Click)
		{
			Audio::PlayOneShot(SE::enemyExplosionD, 0.5f);	//効果音を再生
			Click = true;
			engine.StartFadeOut();
		}
	}
	
	//選択している方を点滅させる
	if (select == Select::Tutorial)
	{
		uiTutorial->SetAlpha(Smooth(Timer));
	}
	else
	{
		uiGameStart->SetAlpha(Smooth(Timer));
	}

	//PressSpaceを魅せる＆回転させる
	uiPressSpace->SetRotation(vec3(Timer));
	uiPressSpace->SetAlpha(1);

	//左右矢印を押すと選択が切り替わる
	if (engine.GetKey(GLFW_KEY_UP)||engine.GetKey(GLFW_KEY_W)|| engine.GetKey(GLFW_KEY_A))
	{
		if (select != Select::Tutorial)
		{
			Audio::PlayOneShot(SE::Click, 0.5f);
			Timer = 3;
		}
		select = Select::Tutorial;
		uiGameStart->SetAlpha(1);
	}
	else if (engine.GetKey(GLFW_KEY_DOWN) || engine.GetKey(GLFW_KEY_S) | engine.GetKey(GLFW_KEY_D))
	{
		if (select != Select::Game)
		{
			Audio::PlayOneShot(SE::Click, 0.5f);
			Timer = 3;
		}

		select = Select::Game;
		uiTutorial->SetAlpha(1);

	}

}
