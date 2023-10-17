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
	//�Q�[���I�u�W�F�N�g���폜
	engine.ClearGameObjectList();
	engine.ClearUILayers();

	//UI���C���[���쐬
	const size_t bgLayer = engine.AddUILayer("Res/Title_BG_2.tga", GL_LINEAR, 10);
	const size_t logoLayer = engine.AddUILayer("Res/title_logo.tga", GL_LINEAR, 10);
	const size_t textLayer = engine.AddUILayer("Res/font_04_2.tga", GL_LINEAR, 10);

	//�w�i�摜
	auto uiBackground = engine.CreateUI<GameObject>(
		bgLayer, "title_bg", 640, 360);
	uiBackground->AddSprite({ 0,0,1,1 });

	//�^�C�g�����S�摜
	uiTitle = engine.CreateUI<GameObject>(
		logoLayer, "title_logo", 660, -600);
	uiTitle->AddSprite({ 0,0,1,1 }, 0, 0, 1.25f);

	//�^�C�g�����S���C�[�W���O������s
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
	uiPressSpace = engine.CreateUI<GameObject>(textLayer, "Press Space", x, 10);
	auto textTitle = uiPressSpace->AddComponent<Text>();
	textTitle->SetText(strTitle, 2);
	
	//�`���[�g���A���J�n�e�L�X�g
	const char strTutorial[] = "Tutorial";
	uiTutorial = engine.CreateUI<GameObject>(textLayer, "Tutorial Start", x *0.5f, 100);
	auto textTutorial = uiTutorial->AddComponent<Text>();
	textTutorial->SetText(strTutorial, 2);

	//���C���Q�[���e�L�X�g
	const char strGameStart[] = "Game Start";

	uiGameStart = engine.CreateUI<GameObject>(textLayer, "Game Start", x * 1.5f, 100);
	auto textGameStart = uiGameStart->AddComponent<Text>();
	textGameStart->SetText(strGameStart, 2);


	//BGM���Đ�
	Audio::Play(AudioPlayer::bgm, BGM::title, 1, true);

	//�t�F�[�h�C��������
	engine.SetFadeRule("Res/fade_rule7.tga");
	engine.SetFadeColor("Res/Boarder.tga");
	engine.StartFadeIn();

	engine.SlowSpeed = 1;
	return true;	//����������
}

void TitleScene::Update(Engine& engine, float deltaTime)
{
	Timer += deltaTime * 0.7f;

	//�t�F�[�h�A�E�g������������
	if (engine.GetFadeState() == Engine::FadeState::Closed)
	{
		if (select == Select::Tutorial)
		{
			engine.SetNextScene<TutorialScene>();	//�`���[�g���A���V�[���Ɉڍs
		}
		else if(select==Select::Game)
		{
			engine.SetNextScene<LoadScene>();		//���[�h�V�[���ֈڍs
		}
	}

	//�N���b�N�������ɂ߂���_�ł�����
	if (Click)
	{
		uiPressSpace->SetAlpha(Smooth(Timer * 10.0f));
	}

	//�I�����Ă������_�ł�����
	if (select == Select::Tutorial)
	{
		uiTutorial->SetAlpha(Smooth(Timer));
	}
	else
	{
		uiGameStart->SetAlpha(Smooth(Timer));
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
	
	uiPressSpace->SetRotation(vec3(Timer));

	//���E���������ƑI�����؂�ւ��
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
