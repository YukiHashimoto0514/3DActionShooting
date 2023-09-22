//@file Health.h

#ifndef COMPONENT_HEALTH_H_INCLUDED
#define COMPONENT_HEALTH_H_INCLUDED
#include "../Engine/Component.h"
#include "../Application/GameObject.h"

//�ϋv�l�R���|�[�l���g

class Health :public Component
{
public: 
	Health() = default;				//�R���X�g���N�^
	virtual ~Health() = default;	//�f�X�g���N�^

	virtual void OnTakeDamage(GameObject& gameObject, GameObject& other, const Damage& damage)override
	{
		//�_���[�W��H�炤
		gameObject.SetHP(gameObject.GetHP() - damage.amount);

		//HP���O�ȉ��ɂȂ����玀
		if (gameObject.GetHP() <= 0)
		{
			gameObject.SetDeadFlg(true);
		}		
	}
	float health = 3;	//�ϋv�l
	float MaxHealth = 5;//�ő�ϋv�l
};


#endif  COMPONENT_HEALTH_H_INCLUDED