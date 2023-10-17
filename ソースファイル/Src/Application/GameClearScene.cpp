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

//�Q�[���N���A��ʂ�����������

//@retval true  ����������
//@retval false ���������s

bool GameClearScene::Initialize(Engine& engine)
{
	//�Q�[���I�u�W�F�N�g���폜
	engine.ClearGameObjectList();
	engine.ClearUILayers();
	engine.RemoveParticleEmitterAll();

	//UI���C���[���쐬
	const size_t bgLayer = engine.AddUILayer("Res/title_bg.tga", GL_LINEAR, 10);
	const size_t logoLayer = engine.AddUILayer("Res/gameclear_logo.tga", GL_LINEAR, 10);
	const size_t textLayer = engine.AddUILayer("Res/font_04_2.tga", GL_LINEAR, 10);

	//�w�i�摜
	auto uiBackground = engine.CreateUI<GameObject>(
		bgLayer, "gameclear_bg", 640,360);
	uiBackground->AddSprite({ 0,0,1,1 });

	//�Q�[���N���A���S�摜
	uiGameClear = engine.CreateUI<GameObject>(
		logoLayer, "clear_logo", 660, -600);
	uiGameClear->AddSprite({ 0,0,1,1 }, 0, 0, 1.25f);

	//���`�ۊǂŉ�ʂɓo�ꂳ����
	auto move = uiGameClear->AddComponent<Lerp>();
	move->TargetX = 660;
	move->TargetY = 600;
	move->MaxTime = 1;
	move->SetCalc = 0;



	//�Q�[���̊J�n���@�������e�L�X�g
	const char strGameClear[] = "Press Space";
	const float fontSizeX = 15;
	const float x =
		640 - static_cast<float>(std::size(strGameClear) - 1) * fontSizeX;

	//�X�y�[�X�{�^��
	uiPressSpace = engine.CreateUI<GameObject>(textLayer, "Press Space", x, 0);
	auto textGameClear = uiPressSpace->AddComponent<Text>();
	textGameClear->SetText(strGameClear, 2);

	//�t�F�[�h�C��������
	engine.StartFadeIn();

	//BGM���Đ�
	Audio::Play(AudioPlayer::bgm, BGM::stageclear, 1, true);

	engine.SlowSpeed = 1;
	return true;	//����������
}

//�^�C�g����ʂ̏�Ԃ��X�V����
void GameClearScene::Update(Engine& engine, float deltaTime)
{
	//�o�ߎ��Ԃ��v��
	Timer += deltaTime;

	//�X�y�[�X����������
	if (engine.GetKey(GLFW_KEY_SPACE))
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
		engine.SetNextScene<TitleScene>();		//�^�C�g���V�[����
	}


	if (Timer < 2)
	{
		//�T�C�Y��ς���
		uiGameClear->SetScale(vec3(1 + Smooth(Timer), 1 + Smooth(Timer), 1));
	}
	else if (Timer > 3)
	{
		Timer = 0;				//���Ԃ����Z�b�g����
	}


}