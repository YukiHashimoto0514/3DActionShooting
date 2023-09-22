//@file FallFloor.h
#ifndef FALLFLOOR_H_INCLUDED
#define FALLFLOOR_H_INCLUDED

#include "../Engine/Component.h"
#include "../Application/GameObject.h"
#include "../Engine/VecMath.h"
//#include "../Engine/VecMath.h"

using namespace VecMath;

//��莞�ԏ���Ă���Ɨ�������R���|�[�l���g
class FallFloor :public Component
{
public:
	FallFloor() = default;				//�R���X�g���N�^
	virtual ~FallFloor() = default;		//�f�X�g���N�^

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{
		//��������܂ł̎���
		const static float FallTime = 0.1f;

		//����������Ă�����
		if (OnFlg)
		{
			OnTime += deltaTime;
			OnFlg = false;
		}

		//��莞�Ԃ��߂�����
		if (OnTime >= FallTime)
		{
			//���ɂԂ�Ԃ�k����
			Radias += deltaTime*10;
			gameObject.AddPosition(vec3{ sin(Radias) * deltaTime,0,0 });

			//3��k�����痎��
			if (degrees(Radias) >= 360 * 3)
			{
				const float FallPower = -5 * deltaTime;
				gameObject.AddPosition(vec3{ 0,FallPower,0 });
			}
		}

		//��胉�C���������ɗ�����
		if (gameObject.GetPos().y <= -5)
		{
			//�Q�[���I�u�W�F�N�g���폜
			gameObject.SetDeadFlg(true);
		}
	}
	virtual void OnCollision(GameObject& gameObject, GameObject& other)override
	{
		//����������Ă���
		OnFlg = true;
	}

private:

	float OnTime = 0;	//����������Ă��鎞��
	float Radias = 0;	//�p�x
	bool OnFlg = false;	//����������Ă��邩�ǂ���(true=����Ă�@false=����ĂȂ�)
};




#endif //FALLFLOOR_H_INCLUDED