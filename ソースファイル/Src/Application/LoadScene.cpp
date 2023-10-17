//@file LoadScene.cpp

#include "LoadScene.h"
#include "MainGameScene.h"
#include "TitleScene.h"
#include "../Component/Text.h"
#include "EasyAudio.h"
#include "../Engine/AudioSettings.h"
#include "GameObject.h"
#include "../Engine/Engine.h"
#include "../Component/Lerp.h"
#include "../Component/MeshRenderer.h"


//ロード画面画面を初期化する

//@retval true  初期化成功
//@retval false 初期化失敗

bool LoadScene::Initialize(Engine& engine)
{
	//ゲームオブジェクトを削除
	engine.ClearGameObjectList();
	engine.ClearUILayers();
	engine.RemoveParticleEmitterAll();

	//UIレイヤーを作成
	const size_t bgLayer = engine.AddUILayer("Res/title_bg.tga", GL_LINEAR, 10);
	const size_t LoadLayer = engine.AddUILayer("Res/LoadPlanet.tga", GL_LINEAR, 10);
	const size_t textLayer = engine.AddUILayer("Res/font_04_2.tga", GL_LINEAR, 10);

	////背景画像(真っ黒)
	auto uiBackground = engine.CreateUI<GameObject>(
		bgLayer, "gameover_bg", 640, 360);
	uiBackground->AddSprite({ 0,0,1,1 });

	//ロード中に動く惑星
	uiPlanet = engine.CreateUI<GameObject>(
		LoadLayer, "LoadPlanet", 640, 360);
	uiPlanet->AddSprite({ 0,0,1,1 });

	const char load[] = "Now Loading...";
	const float fontSizeX2 = 15;
	const float loadx =
		640 - static_cast<float>(std::size(load) - 1) * fontSizeX2;
	Loadstr = engine.CreateUI<GameObject>(textLayer, "text", loadx, 600);
	auto textLoad2 = Loadstr->AddComponent<Text>();
	textLoad2->SetText(load, 2);

	//ゲームの開始方法を示すテキスト
	const char strToPlay[] = "Press Space To Start";
	const float fontSizeX = 15;
	const float x =
		640 - static_cast<float>(std::size(strToPlay) - 1) * fontSizeX;

	//スペースボタン
	uiPressSpace = engine.CreateUI<GameObject>(textLayer, "Press Space", x, 100);
	auto textGameOver = uiPressSpace->AddComponent<Text>();
	textGameOver->SetText(strToPlay, 2);
	uiPressSpace->SetPos(vec3(0, -100, 0));

	//BGMを再生
	Audio::Play(AudioPlayer::bgm, BGM::load, 1, true);
	engine.SlowSpeed = 1;

	//ロードパーセントを表示
	const float loadX =
		640 - 2 * fontSizeX2;
	txtLoad = engine.CreateUI<GameObject>(textLayer, "Loadtxt", loadX, 500);

	char str[16];									//表示する文字列
	snprintf(str, std::size(str), "LoadPercent 0");		//文字表示
	auto txt = txtLoad->AddComponent<Text>();
	txt->SetText(str, 2);

	//フェードインさせる
	engine.StartFadeIn();


	return true;	//初期化成功
}

//3Dモデル（随時追加）
const char* Loadlist[] = {
	"Res/Model/Tree_03.obj",
	"Res/Model/Tree_02.obj",
	"Res/Model/Tree_1.obj",
	"Res/Model/Slime_02_blender.obj",
	"Res/Model/SlimeKing.obj",
	"Res/Model/box.obj",
	"Res/Model/Drone_01.obj",
	"Res/Model/FallGround_01.obj",
	"Res/Model/Golden_Chest.obj",
	"Res/Model/grass01.obj",
	"Res/Model/Ground_01.obj",
	"Res/Model/Ground_02.obj",
	"Res/Model/Mushroom_01.obj",
	"Res/Model/Plant003_V2.obj",
	"Res/Model/Rock.obj",
};

//配列の長さを取得
const float LoadLength = sizeof(Loadlist) / sizeof(Loadlist[0]);


//タイトル画面の状態を更新する
void LoadScene::Update(Engine& engine, float deltaTime)
{
	Timer += deltaTime;			//イージングをするためのタイマー

	//だんだん大きくする
	uiPlanet->SetScale(vec3(Timer));

	//フェードアウトが完了したら
	if (engine.GetFadeState() == Engine::FadeState::Closed)
	{
		engine.SetNextScene<MainGameScene>();		//タイトルシーンに（仮）
	}
	
	//オブジェクトのロードが終ってたら
	if (Loadflg)
	{
		//スペースキーが押されたらゲーム開始
		if (engine.GetKey(GLFW_KEY_SPACE))
		{
			if (!Push)
			{
				Audio::PlayOneShot(SE::Click2, 0.2f);		//効果音を再生
				engine.SetFadeRule("Res/fade_rule3.tga");
				engine.StartFadeOut();	//フェードをする
				Push = true;
			}
		}
	}


	//1.0秒後から読み込み開始
	if (Timer >= 1.0f)
	{
		if (!Loadflg)
		{
			if (i < LoadLength)
			{
				//読み込み具合を計算
				const float per = i / (LoadLength - 1.0f) * 100.0f;

				//読み込みパーセントを更新
				char str[16];									//表示する文字列
				snprintf(str, std::size(str), "LoadPercent %.f", per);		//文字表示
				auto txt = txtLoad->AddComponent<Text>();
				txt->SetText(str, 2);

				//３Ⅾモデルを先に読み込んでおく
				GameObjectPtr obj = engine.Create<GameObject>("enemy", vec3(0));

				const char* mod = Loadlist[i];			//読み込むファイル名
				auto objrender = obj->AddComponent<MeshRenderer>();
				objrender->mesh = engine.LoadOBJ(mod);	//ロード

				//次のモデルを読み込む
				i++;
			}
			else
			{
				//読み込み終了
				Loadflg = true;

				//uiPressを表示
				uiPressSpace->SetPos(vec3(180, 100, 0));

				//LoadStrを非表示に
				Loadstr->SetPos(vec3(0, -100, 0));

				//新しいテキストの追加
				const size_t textLayer = engine.AddUILayer("Res/font_04_2.tga", GL_LINEAR, 10);

				const char load[] = "Let's GO!!!";
				const float fontSizeX2 = 15;
				const float loadx =
					640 - static_cast<float>(std::size(load) - 1) * fontSizeX2;
				auto loadstr = engine.CreateUI<GameObject>(textLayer, "text", loadx, 600);
				auto textLoad2 = loadstr->AddComponent<Text>();

				textLoad2->SetText(load, 4);

			}
		}
	}
}

