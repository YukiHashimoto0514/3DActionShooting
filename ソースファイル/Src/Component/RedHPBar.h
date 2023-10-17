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
			LOG("�Q�Ƃ���I�u�W�F�N�g��ݒ肵�Ă�������");
			return;
		}

		//HP����r�Ώۂƈ������
		if (gameObject.GetHP() != GreenBar->GetHP())
		{
			//�C�[�W���O���������Ă��Ȃ�������
			if (TakeTime < 1)
			{
				//���߂̈�x��������
				if (!isOnce)
				{
					//�C�[�W���O�ɕK�v�ȏ������
					BeforePos = gameObject.GetPos();
					TargetPos = GreenBar->GetPos();
					isOnce = true;
				}

				//�C�[�W���O�𗘗p���Ĉʒu��ύX����
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
			//�O���[���o�[�Ɠ����ʒu�ɂ���
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
