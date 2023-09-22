//@file ARoundMove.h
#ifndef AROUNDMOVE_H_INCLUDED
#define AROUNDMOVE_H_INCLUDED

#include "../Engine/Component.h"
#include "../Application/GameObject.h"
#include "../Engine/VecMath.h"
#include "../Component/Player.h"

using namespace VecMath;

//�Ώۂ̉�]�ɍ��킹��
class ARoundMove :public Component
{
public:
	ARoundMove() = default;				//�R���X�g���N�^
	virtual ~ARoundMove() = default;		//�f�X�g���N�^

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{


		gameObject.SetPos({ Target.GetPos().x + cos(Rot) * 11,
							Target.GetPos().y ,
							Target.GetPos().z + sin(Rot) * 11 });

		gameObject.SetRotation({ 0,-Rot,0 });

		//������悤�ɂȂ�܂œ����Ȃ�
		if (MoveTimer >= 0)
		{
			MoveTimer -= deltaTime;
			return;
		}
		Rot += deltaTime;

		//��������玀��
		if (Rot >= 2 * pi)
		{
			gameObject.SetDeadFlg(true);
		}

	}


	//void SetLiveTime(float _time)
	//{
	//	LiveTime = _time;
	//}

	void SetMoveTime(float _timer)
	{
		MoveTimer = _timer;
	}

	void SetTarget(Boss _target)
	{
		Target = _target;
	}


private:

	VecMath::vec3 Rotation = VecMath::vec3(0, 0, 1);	//����
	float MoveSpeed = 10;	//�ړ����x
	float MoveTimer = 0;	//�s�����J�n����܂ł̎���
	float Rot = 0;			//��]�p�x�i�ʓx�@�j
	float Scale = 0;
	Boss Target;	//���Ώ�
};




#endif //AROUNDMOVE_H_INCLUDED