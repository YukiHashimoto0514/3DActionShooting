//@file TextMove.h

#ifndef COMPONENT_TEXTMOVE_H_INCLUDED
#define COMPONENT_TEXTMOVE_H_INCLUDED

#include "../Engine/Component.h"
#include "../Application/GameObject.h"
#include "../Engine/Sprite.h"
#include "../Component/Lerp.h"


enum class State
{
	EaseBefore,
	EaseNow,
	EaseAfter
};

//�����̓���

class TextMove :public Component
{
public:
	TextMove() = default;			//�R���X�g���N�^
	virtual ~TextMove() = default;	//�f�X�g���N�^


	virtual void Start(GameObject& gameObject)override
	{
		//�����̏�����
		RandomEngine.seed(rd());
		WaitTime = std::uniform_real_distribution<float>(3.0f, 5.0f)(rd);//2~5

	}

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{

		//���Ԃ����Ă��Ȃ��Ȃ疳��
		if (WaitTime > 0)
		{
			WaitTime -= deltaTime;
			return;
		}

		//�C�[�W���O�O
		if (state == State::EaseBefore)
		{
			//�s����I������
			Action = std::uniform_int_distribution<>(0, 4)(rd);//0~4
			state = State::EaseNow;//�X�e�[�g���C�[�W���O����
		}
			 		
		//�ǂ�ȍs������邩
		switch (Action)
		{
		case 0://�����h���

			//�C�[�W���O���������Ă��Ȃ�������
			if (TakeTime < 1)
			{
				//���߂̈�x��������
				if (!isOnce)
				{
					//�C�[�W���O�ɕK�v�ȏ������
					Before = gameObject.GetRotation();
					Target = vec3(30);
					isOnce = true;
				}

				//�C�[�W���O�𗘗p
				gameObject.SetRotation(
					vec3(Before.x + Smooth(TakeTime) * (Target.x - Before.x),
						0, 0));
			}
			else
			{
				//�C�[�W���O���
				state = State::EaseAfter;
			}

			break;

		case 1://�傫���Ȃ�

			//�C�[�W���O���������Ă��Ȃ�������
			if (TakeTime < 1)
			{
				//���߂̈�x��������
				if (!isOnce)
				{
					//�C�[�W���O�ɕK�v�ȏ������
					Before = gameObject.GetScale();
					Target = vec3(1.5f);
					isOnce = true;
				}

				//�C�[�W���O
				gameObject.SetScale(
					vec3(Before.x + Smooth(TakeTime) * (Target.x - Before.x),
						0, 0));
			}
			else
			{
				//�C�[�W���O���
				state = State::EaseAfter;
			}

			break;

		case 2://�ړ�����i���j

			//�C�[�W���O���������Ă��Ȃ�������
			if (TakeTime < 1)
			{
				//���߂̈�x��������
				if (!isOnce)
				{
					//�C�[�W���O�ɕK�v�ȏ������
					Before = gameObject.GetPos();
					Target = vec3(gameObject.GetPos() - 100);
					isOnce = true;
				}

				//�C�[�W���O
				gameObject.SetPos(
					vec3(Before.x + Smooth(TakeTime) * (Target.x - Before.x),
						Before.y, 0));
			}
			else
			{
				//�C�[�W���O���
				state = State::EaseAfter;
			}


			break;

		case 3://�X���i�E�j

			//�C�[�W���O���������Ă��Ȃ�������
			if (TakeTime < 1)
			{
				//���߂̈�x��������
				if (!isOnce)
				{
					//�C�[�W���O�ɕK�v�ȏ������
					Before = gameObject.GetRotation();
					Target = vec3(-30);
					isOnce = true;
				}

				//�C�[�W���O
				gameObject.SetRotation(
					vec3(Before.x + Smooth(TakeTime) * (Target.x - Before.x),
						0, 0));
			}
			else
			{
				//�C�[�W���O���
				state = State::EaseAfter;
			}

			break;

		case 4://�ړ�����i�E�j

			//�C�[�W���O���������Ă��Ȃ�������
			if (TakeTime < 1)
			{
				//���߂̈�x��������
				if (!isOnce)
				{
					//�C�[�W���O�ɕK�v�ȏ������
					Before = gameObject.GetPos();
					Target = vec3(Before + 100);
					isOnce = true;
				}

				//�C�[�W���O�𗘗p���Ĉʒu��ύX����
				gameObject.SetPos(
					vec3(Before.x + Smooth(TakeTime) * (Target.x - Before.x),
						Before.y, 0));
			}
			else
			{
				//�C�[�W���O���
				state = State::EaseAfter;
			}

			break;
		default:
			break;
		}

		//�C�[�W���O���I������
		if (state == State::EaseAfter)
		{
			TakeTime = 0;	//���Ԃ����Z�b�g
			isOnce = false;
			WaitTime = std::uniform_real_distribution<float>(2.0f, 5.0f)(rd);//2~5
			state = State::EaseBefore;
		}

		//���Ԃ��o��
		TakeTime += deltaTime;
		

	}


private:


	float TakeTime = 0;		//�C�[�W���O�Ɏg���ϐ�
	float WaitTime = 5.0f;		//�����܂ł̑ҋ@����
	bool isOnce = false;	//��x��������

	int Action = 0;

	vec3 Before = vec3(1);	//�ړ��O�̂���
	vec3 Target = vec3(1);	//�ړ��������

	State state = State::EaseBefore;
		
	//����
	std::random_device rd;
	std::mt19937 RandomEngine;

};


#endif // !COMPONENT_TEXTMOVE_H_INCLUDED
