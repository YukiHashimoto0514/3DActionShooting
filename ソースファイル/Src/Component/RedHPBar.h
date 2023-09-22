//@file RedHPBar.h

#ifndef COMPONENT_REDHPBAR_H_INCLUDED
#define COMPONENT_REDHPBAR_H_INCLUDED

#include "../Engine/Component.h"
#include "../Application/GameObject.h"
#include "../Engine/Sprite.h"
#include "../Component/Lerp.h"

//�Ԃ�HP�o�[�𓮂����R���|�[�l���g

class RedHPBar :public Component
{
public:
	RedHPBar() = default;			//�R���X�g���N�^
	virtual ~RedHPBar() = default;	//�f�X�g���N�^

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{
		//�ݒ肳��Ă��Ȃ��Ȃ疳��
		if (GreenBar == NULL)
		{
			return;
		}

		if (gameObject.GetHP() != GreenBar->GetHP())
		{
			if (TakeTime < 1)
			{
				if (!isOnce)
				{
					BeforePos = gameObject.GetPos();
					TargetPos = GreenBar->GetPos();
					isOnce = true;
				}
				gameObject.SetPos(
					vec3(BeforePos.x + Circle(TakeTime) * (TargetPos.x - BeforePos.x),
						BeforePos.y + Circle(TakeTime) * (TargetPos.y - BeforePos.y), 0));
			}
			else
			{
				//HP�𓯂��ɂ���
				gameObject.SetHP(GreenBar->GetHP());
				TakeTime = 0;	//���Ԃ����Z�b�g
				isOnce = false;
			}

			//���Ԃ��o��
			TakeTime += deltaTime;
		}
		else
		{
			gameObject.SetPos(GreenBar->GetPos());
		}

	}

	void SetGreenBar(GameObjectPtr _bar)
	{
		GreenBar = _bar;
	}

private:

	GameObjectPtr GreenBar;	//�t������ΐF�̃o�[

	float TakeTime = 0;		//�C�[�W���O�Ɏg���ϐ�
	bool isOnce = false;	//��x��������
	vec3 BeforePos = vec3(1);	//�ړ��O�̍��W
	vec3 TargetPos = vec3(1);	//�ړ�������W
};


#endif // !COMPONENT_REDHPBAR_H_INCLUDED
