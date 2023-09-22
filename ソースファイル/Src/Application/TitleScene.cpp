#include "TitleScene.h"
#include "../Component/Text.h"
#include "EasyAudio.h"
#include "../Engine/AudioSettings.h"
#include "GameObject.h"
#include "../Engine/Engine.h"
#include "../Component/Lerp.h"
#include "MainGameScene.h"
#include "LoadScene.h"

bool TitleScene::Initialize(Engine& engine)
{
	//�Q�[���I�u�W�F�N�g���폜
	engine.ClearGameObjectList();
	engine.ClearUILayers();

	//UI���C���[���쐬
	const size_t bgLayer = engine.AddUILayer("Res/title_bg.tga", GL_LINEAR, 10);
	const size_t logoLayer = engine.AddUILayer("Res/title_logo.tga", GL_LINEAR, 10);
	const size_t textLayer = engine.AddUILayer("Res/font_04_2.tga", GL_LINEAR, 10);

	//�w�i�摜
	auto uiBackground = engine.CreateUI<GameObject>(
		bgLayer, "title_bg", 640, 360);
	uiBackground->AddSprite({ 0,0,1,1 });

	//�Q�[���N���A���S�摜
	uiTitle = engine.CreateUI<GameObject>(
		logoLayer, "clear_logo", 660, -600);
	uiTitle->AddSprite({ 0,0,1,1 }, 0, 0, 1.25f);

	auto move = uiTitle->AddComponent<Lerp>();
	move->TargetX = 660;
	move->TargetY = 600;
	move->MaxTime = 1;
	move->SetCalc = 0;



	//�Q�[���̊J�n���@�������e�L�X�g
	const char strTitle[] = "Press Space";
	const float fontSizeX = 15;
	const float x =
		640 - static_cast<float>(std::size(strTitle) - 1) * fontSizeX;

	//�X�y�[�X�{�^��
	uiPressSpace = engine.CreateUI<GameObject>(textLayer, "Press Space", x, 0);
	auto textTitle = uiPressSpace->AddComponent<Text>();
	textTitle->SetText(strTitle, 2);


	//BGM���Đ�
	Audio::Play(AudioPlayer::bgm, BGM::title, 1, true);

	//�t�F�[�h�C��������
	engine.SetFadeRule("Res/fade_rule2.tga");
	engine.SetFadeColor("Res/Boarder.tga");
	engine.StartFadeIn();

	engine.SlowSpeed = 1;
	return true;	//����������
}

void TitleScene::Update(Engine& engine, float deltaTime)
{
	//�t�F�[�h�A�E�g������������
	if (engine.GetFadeState() == Engine::FadeState::Closed)
	{
		engine.SetNextScene<LoadScene>();		//�^�C�g���V�[���Ɂi���j
	}

	//�t�F�[�h���������Ă��Ȃ������疳��
	if (engine.GetFadeState() != Engine::FadeState::Open)
	{
		return;
	}

	//�X�y�[�X����������V�[���J�ڊJ�n
	if (engine.GetKey(GLFW_KEY_SPACE))
	{
		if (!Click)
		{
			Audio::PlayOneShot(SE::Click2, 0.2f);	//���ʉ����Đ�
			Click = true;
			engine.StartFadeOut();
		}
	}


}
