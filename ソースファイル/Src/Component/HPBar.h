//@file HPBar.h

#ifndef COMPONENT_HPBAR_H_INCLUDED
#define COMPONENT_HPBAR_H_INCLUDED

#include "../Engine/Component.h"
#include "../Application/GameObject.h"
#include "../Engine/Sprite.h"

//HP�o�[�𓮂����R���|�[�l���g

class HPBar :public Component
{
public:
	HPBar() = default;			//�R���X�g���N�^
	virtual ~HPBar() = default;	//�f�X�g���N�^

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{
		//�ݒ肳��Ă��Ȃ��Ȃ疳��
		if (player == NULL)
		{
			LOG("�v���C���[��ݒ肵�Ă�������");
			return;
		}

		//�Q�[���I�u�W�F�N�g�ɐݒ�
		gameObject.SetHP(player->GetHP());

		//�ϐ��ɑ��
		NowHP = gameObject.GetHP();

		//�������v�Z
		float diff = NowHP / MaxHP;

		//�����̈ʒu��ݒ�
		gameObject.SetPos({ 460 * diff,646,0 });
	}

	//�ő�HP�̐ݒ�(���łɌ��݂�HP�����킹�Ă���)
	void SetMaxHP(float _max)
	{
		MaxHP = _max;
		NowHP = _max;
	}

	void SetNowHP(float _hp)
	{
		NowHP = _hp;
	}

	void SetPlayer(GameObjectPtr _player)
	{
		player = _player;
	}

private:

	float MaxHP = 0;		//�ő�HP
	float NowHP = 0;		//���݂�HP
	
	GameObjectPtr player;	//�Q�Ƃ���HP

};


#endif // !COMPONENT_HPBAR_H_INCLUDED
