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
	//�Q�[���I�u�W�F�N�g���폜
	engine.ClearGameObjectList();
	engine.ClearUILayers();

	//UI���C���[���쐬
	const size_t bgLayer = engine.AddUILayer("Res/UI/Title_BG_2.tga", GL_LINEAR, 10);
	const size_t MonsterLayer = engine.AddUILayer("Res/UI/Monster.tga", GL_LINEAR, 10);
	const size_t PanicLayer = engine.AddUILayer("Res/UI/Panic.tga", GL_LINEAR, 10);
	const size_t textLayer = engine.AddUILayer("Res/UI/font_04_2.tga", GL_LINEAR, 10);

	const float viewX = engine.GetViewSize().x;
	const float viewY = engine.GetViewSize().y;

	//�w�i�摜
	auto uiBackground = engine.CreateUI<GameObject>(
		bgLayer, "title_bg", viewX * 0.5f, viewY * 0.5f);
	uiBackground->AddSprite({ 0,0,1,1 });

	//�^�C�g�����S�摜
	uiMonster = engine.CreateUI<GameObject>(
		MonsterLayer, "title_logo", viewX * 0.5f, -600);
	uiMonster->AddSprite({ 0,0,1,1 }, 0, 0,0);
	uiMonster->AddComponent<TextMove>();

	//�^�C�g�����S���C�[�W���O������s
	auto move = uiMonster->AddComponent<Lerp>();
	move->TargetX = 460;
	move->TargetY = 600;
	move->MaxTime = 1;
	move->SetCalc = 0;

	//�^�C�g�����S�摜
	uiPanic = engine.CreateUI<GameObject>(
		PanicLayer, "title_logo", 660, -600);
	uiPanic->AddSprite({ 0,0,1,1 }, 0, 0,0);
	uiPanic->AddComponent<TextMove>();


	//�Q�[���̊J�n���@�������e�L�X�g
	const char strTitle[] = "Press Space";
	const float fontSizeX = 15;
	const float x =
		640 - static_cast<float>(std::size(strTitle) - 1) * 13;

	//�X�y�[�X�{�^��
	uiPressSpace = engine.CreateUI<GameObject>(textLayer, "Press Space", x, 10);
	auto textTitle = uiPressSpace->AddComponent<Text>();
	textTitle->SetText(strTitle, 1.7f);
	
	uiPressSpace->SetAlpha(0);

	//�`���[�g���A���J�n�e�L�X�g
	const char strTutorial[] = "Tutorial";
	const float x2 =
		640 - static_cast<float>(std::size(strTutorial) - 1) * fontSizeX;

	uiTutorial = engine.CreateUI<GameObject>(textLayer, "Tutorial Start", x2 , 250);
	auto textTutorial = uiTutorial->AddComponent<Text>();
	textTutorial->SetText(strTutorial, 2);

	//���C���Q�[���e�L�X�g
	const char strGameStart[] = "Game Start";
	const float x3 =
		640 - static_cast<float>(std::size(strGameStart) - 1) * fontSizeX;

	uiGameStart = engine.CreateUI<GameObject>(textLayer, "Game Start", x3 , 150);
	auto textGameStart = uiGameStart->AddComponent<Text>();
	textGameStart->SetText(strGameStart, 2);

	uiTutorial->SetAlpha(0);
	uiGameStart->SetAlpha(0);

	Audio::PlayOneShot(SE::Wind, 1.0f);
	//BGM���Đ�
	Audio::Play(AudioPlayer::bgm, BGM::title, 1, true);

	//�t�F�[�h�C��������
	engine.SetFadeRule("Res/Fade/fade_rule7.tga");
	engine.SetFadeColor("Res/UI/Boarder.tga");
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


	if (Timer <= 1)
	{
		uiMonster->SetRotation({ 11720 + Circle(Timer) * -11720,0,0 });
		uiMonster->SetScale({ 100 + Circle(Timer) * (1 - 100),0,0 });
	}
	else if (Timer <= 2)
	{
		uiPanic->SetPos({ 850,-600 + Circle(Timer-1) * (450-(-600)),0 });
	}
	
	//�t�F�[�h���������Ă��Ȃ������疳��
	if (engine.GetFadeState() != Engine::FadeState::Open)
	{
		return;
	}

	//�A�j���[�V�������I���Ă����Ȃ�������
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

	//�X�y�[�X����������V�[���J�ڊJ�n
	if (engine.GetKey(GLFW_KEY_SPACE))
	{
		if (!Click)
		{
			Audio::PlayOneShot(SE::enemyExplosionD, 0.5f);	//���ʉ����Đ�
			Click = true;
			engine.StartFadeOut();
		}
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

	//PressSpace�𖣂��違��]������
	uiPressSpace->SetRotation(vec3(Timer));
	uiPressSpace->SetAlpha(1);

	//���E���������ƑI�����؂�ւ��
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
