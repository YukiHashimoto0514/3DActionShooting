//@file Chase.h
#ifndef CHASE_H_INCLUDED
#define CHASE_H_INCLUDED

#include "../Engine/Component.h"
#include "../Application/GameObject.h"
#include "../Engine/VecMath.h"
//#include "../Engine/VecMath.h"

using namespace VecMath;

//�^�[�Q�b�g��ǂ�������R���|�[�l���g
class Chase :public Component
{
public:
	Chase() = default;				//�R���X�g���N�^
	virtual ~Chase() = default;		//�f�X�g���N�^

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{
		//�W�I��ݒ肵�Ă��Ȃ���A����
		if (target == NULL)
		{
			return;
		}
		//MoveSpeed += deltaTime;
		//�����x�N�g�����擾
		vec3 Look = gameObject.GetPos() - target->GetPos();

		//���K��
		normalize(Look);

		//�p�x���v�Z
		float theta = atan2(Look.x, Look.z);

		//�i�݂��������ɉ�]
		vec3 forward = vec3(sin(theta),
			0,
			cos(theta));

		//���]
		forward *= vec3(-1);

		//�ړ�����
		gameObject.AddPosition(forward * MoveSpeed * deltaTime);

	}

	void SetTarget(GameObjectPtr _target)
	{
		target = _target;
	}
private:
	GameObjectPtr target;	//�ǂ�������Ώ�

	float MoveSpeed = 5;	//�ړ����x
};




#endif //CHASE_H_INCLUDED