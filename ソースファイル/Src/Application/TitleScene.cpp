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

bool TitleScene::Initialize(Engine& engine)
{
	//ゲームオブジェクトを削除
	engine.ClearGameObjectList();
	engine.ClearUILayers();

	//UIレイヤーを作成
	const size_t bgLayer = engine.AddUILayer("Res/Title_BG_2.tga", GL_LINEAR, 10);
	const size_t logoLayer = engine.AddUILayer("Res/title_logo.tga", GL_LINEAR, 10);
	const size_t textLayer = engine.AddUILayer("Res/font_04_2.tga", GL_LINEAR, 10);

	//背景画像
	auto uiBackground = engine.CreateUI<GameObject>(
		bgLayer, "title_bg", 640, 360);
	uiBackground->AddSprite({ 0,0,1,1 });

	//タイトルロゴ画像
	uiTitle = engine.CreateUI<GameObject>(
		logoLayer, "title_logo", 660, -600);
	uiTitle->AddSprite({ 0,0,1,1 }, 0, 0, 1.25f);

	//タイトルロゴをイージングさせるs
	auto move = uiTitle->AddComponent<Lerp>();
	move->TargetX = 660;
	move->TargetY = 600;
	move->MaxTime = 1;
	move->SetCalc = 0;



	//ゲームの開始方法を示すテキスト
	const char strTitle[] = "Press Space";
	const float fontSizeX = 15;
	const float x =
		640 - static_cast<float>(std::size(strTitle) - 1) * fontSizeX;

	//スペースボタン
	uiPressSpace = engine.CreateUI<GameObject>(textLayer, "Press Space", x, 10);
	auto textTitle = uiPressSpace->AddComponent<Text>();
	textTitle->SetText(strTitle, 2);
	
	//チュートリアル開始テキスト
	const char strTutorial[] = "Tutorial";
	uiTutorial = engine.CreateUI<GameObject>(textLayer, "Tutorial Start", x *0.5f, 100);
	auto textTutorial = uiTutorial->AddComponent<Text>();
	textTutorial->SetText(strTutorial, 2);

	//メインゲームテキスト
	const char strGameStart[] = "Game Start";

	uiGameStart = engine.CreateUI<GameObject>(textLayer, "Game Start", x * 1.5f, 100);
	auto textGameStart = uiGameStart->AddComponent<Text>();
	textGameStart->SetText(strGameStart, 2);


	//BGMを再生
	Audio::Play(AudioPlayer::bgm, BGM::title, 1, true);

	//フェードインさせる
	engine.SetFadeRule("Res/fade_rule7.tga");
	engine.SetFadeColor("Res/Boarder.tga");
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

	//選択している方を点滅させる
	if (select == Select::Tutorial)
	{
		uiTutorial->SetAlpha(Smooth(Timer));
	}
	else
	{
		uiGameStart->SetAlpha(Smooth(Timer));
	}
	
	//フェードが完了していなかったら無視
	if (engine.GetFadeState() != Engine::FadeState::Open)
	{
		return;
	}

	//スペースを押したらシーン遷移開始
	if (engine.GetKey(GLFW_KEY_SPACE))
	{
		if (!Click)
		{
			Audio::PlayOneShot(SE::Click2, 0.2f);	//効果音を再生
			Click = true;
			engine.StartFadeOut();
		}
	}
	
	uiPressSpace->SetRotation(vec3(Timer));

	//左右矢印を押すと選択が切り替わる
	if (engine.GetKey(GLFW_KEY_LEFT)||engine.GetKey(GLFW_KEY_A))
	{
		select = Select::Tutorial;
		uiGameStart->SetAlpha(1);
	}
	else if (engine.GetKey(GLFW_KEY_RIGHT) || engine.GetKey(GLFW_KEY_D))
	{
		select = Select::Game;
		uiTutorial->SetAlpha(1);

	}

}
