//@file Fall.h
#ifndef FALL_H_INCLUDED
#define FALL_H_INCLUDED

#include "../Engine/Component.h"
#include "../Application/GameObject.h"
#include "../Engine/VecMath.h"
//#include "../Engine/VecMath.h"

using namespace VecMath;

//��������R���|�[�l���g
class Fall :public Component
{
public:
	Fall() = default;				//�R���X�g���N�^
	virtual ~Fall() = default;		//�f�X�g���N�^

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{
		if (gameObject.GetAir())
		{
			FallTimer += deltaTime;
			float fall = FallTimer * FallTimer * Gravity * 0.5f;
			vec3 DownPower = { 0,-fall,0 };
			gameObject.AddPosition(DownPower);
		}
		else
		{
			FallTimer = 0;
		}
	}



private:
	const float Gravity = 9.8f;
	float FallTimer = 0;	//�����Ă��鎞��
	float Radias = 0;	//�p�x
	bool OnFlg = false;	//����������Ă��邩�ǂ���(true=����Ă�@false=����ĂȂ�)
};




#endif //FALL_H_INCLUDED