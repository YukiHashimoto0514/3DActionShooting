//@file Goal.h
#ifndef GOAL_H_INCLUDED
#define GOAL_H_INCLUDED

#include "../Engine/Component.h"
#include "../Engine/VecMath.h"
#include "../Engine/Debug.h"
#include "../Engine/Engine.h"
#include "../Application/GameObject.h"
#include "../Application/MainGameScene.h"

using namespace VecMath;

//�^�[�Q�b�g��ǂ�������R���|�[�l���g
class Goal :public Component
{
public:
	Goal() = default;				//�R���X�g���N�^
	virtual ~Goal() = default;		//�f�X�g���N�^

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{
		//�t�F�[�h������������
		if (e->GetFadeState() == Engine::FadeState::Closed)
		{
			//���C���Q�[���V�[���ɑJ�ڂ���
			e->SetNextScene<MainGameScene>();
		}
	}


	virtual void OnCollision(GameObject& gameObject, GameObject& other)override
	{
		//�^�[�Q�b�g���S�[���܂ł��ǂ蒅������
		if (other.name == target->name)
		{

			//�t�F�[�h�A�E�g���J�n
			e->SetFadeRule("Res/Fade/fade_rule.tga");
			e->SetFadeColor("Res/Fade/fade_color.tga");
			e->StartFadeOut();	//�t�F�[�h����

		}

	}

	void SetTarget(GameObjectPtr _target)
	{
		target = _target;
	}

	void SetEngine(Engine *_engine)
	{
		e = _engine;
	}

private:
	GameObjectPtr target;	//�ړI�n�܂œ��B����l
	Engine* e = NULL;
};




#endif //GOAL_H_INCLUDED