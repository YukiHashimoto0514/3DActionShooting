#include "GameOverScene.h"
#include "../Component/Text.h"
#include "EasyAudio.h"
#include "../Engine/AudioSettings.h"
#include "GameObject.h"
#include "../Engine/Engine.h"
#include "../Component/Lerp.h"
#include "TitleScene.h"
bool GameOverScene::Initialize(Engine& engine)
{
	//�Q�[���I�u�W�F�N�g���폜
	engine.ClearGameObjectList();
	engine.ClearUILayers();

	//UI���C���[���쐬
	const size_t bgLayer = engine.AddUILayer("Res/gameover_bg.tga", GL_LINEAR, 10);
	const size_t logoLayer = engine.AddUILayer("Res/gameover_logo.tga", GL_LINEAR, 10);
	const size_t textLayer = engine.AddUILayer("Res/font_04_2.tga", GL_LINEAR, 10);

	//�w�i�摜
	auto uiBackground = engine.CreateUI<GameObject>(
		bgLayer, "gameclear_bg", 640, 360);
	uiBackground->AddSprite({ 0,0,1,1 });

	//�Q�[���N���A���S�摜
	uiGameover = engine.CreateUI<GameObject>(
		logoLayer, "over_logo", 660, -600);
	uiGameover->AddSprite({ 0,0,1,1 }, 0, 0, 1.25f);

	auto move = uiGameover->AddComponent<Lerp>();
	move->TargetX = 660;
	move->TargetY = 600;
	move->MaxTime = 1;
	move->SetCalc = 0;



	//�Q�[���̊J�n���@�������e�L�X�g
	const char strGameOver[] = "Press Enter";
	const float fontSizeX = 15;
	const float x =
		640 - static_cast<float>(std::size(strGameOver) - 1) * fontSizeX;

	//�X�y�[�X�{�^��
	uiPressEnter = engine.CreateUI<GameObject>(textLayer, "Press Enter", x, 0);
	auto textGameClear = uiPressEnter->AddComponent<Text>();
	textGameClear->SetText(strGameOver, 2);


	//BGM���Đ�
	Audio::Play(AudioPlayer::bgm, BGM::gameover, 1, true);
	engine.SlowSpeed = 1;
	ClickEnter = false;
	engine.StartFadeIn();
	return true;	//����������
}

void GameOverScene::Update(Engine& engine, float deltaTime)
{
	if (engine.GetKey(GLFW_KEY_ENTER))
	{
		//�����N���b�N���Ă��Ȃ�������
		if (!ClickEnter)
		{
			Audio::PlayOneShot(SE::Click2, 0.2f);	//���ʉ����Đ�
			engine.SetFadeRule("Res/fade_rule2.tga");
			engine.StartFadeOut();
			ClickEnter = true;
		}
	}

	//�t�F�[�h�A�E�g������������
	if (engine.GetFadeState() == Engine::FadeState::Closed && ClickEnter)
	{
		engine.SetNextScene<TitleScene>();		//�^�C�g���V�[���Ɂi���j
	}

}
