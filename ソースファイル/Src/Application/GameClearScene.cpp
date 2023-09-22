//@file GameClearScene.cpp

#include "GameClearScene.h"
#include "MainGameScene.h"
#include "../Component/Text.h"
#include "EasyAudio.h"
#include "../Engine/AudioSettings.h"
#include "GameObject.h"
#include "../Engine/Engine.h"
#include "../Component/Lerp.h"

//�Q�[���N���A��ʂ�����������

//@retval true  ����������
//@retval false ���������s

bool GameClearScene::Initialize(Engine& engine)
{
	//�Q�[���I�u�W�F�N�g���폜
	engine.ClearGameObjectList();
	engine.ClearUILayers();

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


		if (engine.GetKey(GLFW_KEY_SPACE))
		{
			Audio::PlayOneShot(SE::Click2, 0.2f);	//���ʉ����Đ�
			engine.SetNextScene<MainGameScene>();		//�^�C�g���V�[���Ɂi���j
		}

	if (Timer < 2)
	{
		//switch (Action)
		//{
		//case 0://�T�C�Y��ς���
		//	uiGameClear->SetScale(vec3(1 + Smooth(Timer), 1 + Smooth(Timer),1));
		//	break;

		//case 1://��]������
		//	uiGameClear->SetRotation();
		//	break;

		//case 2://��]���Ȃ���T�C�Y��ς���
		//	uiGameClear->Scale = 1 + Smooth(Timer);
		//	uiGameClear->radias = Smooth(Timer) * 360;
		//	break;

		//case 3://����������
		//	uiGameClear->Red = 1 + Smooth(Timer);
		//	uiGameClear->Green = 1 + Smooth(Timer);
		//	uiGameClear->Blue = 1 + Smooth(Timer);
		//	break;
		//}
	}
	else if (Timer > 3)
	{
		//NextFlg = true;			//���̃V�[���Ɉڍs�ł���悤�ɂ���
		//Timer = 0;				//���Ԃ����Z�b�g����
		//Action = rand() % 4;	//�����_���œ��������߂�
	}


}