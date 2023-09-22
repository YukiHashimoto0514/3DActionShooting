//@file ARoundMove.h
#ifndef AROUNDMOVE_H_INCLUDED
#define AROUNDMOVE_H_INCLUDED

#include "../Engine/Component.h"
#include "../Application/GameObject.h"
#include "../Engine/VecMath.h"
#include "../Component/Player.h"

using namespace VecMath;

//対象の回転に合わせる
class ARoundMove :public Component
{
public:
	ARoundMove() = default;				//コンストラクタ
	virtual ~ARoundMove() = default;		//デストラクタ

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{


		gameObject.SetPos({ Target.GetPos().x + cos(Rot) * 11,
							Target.GetPos().y ,
							Target.GetPos().z + sin(Rot) * 11 });

		gameObject.SetRotation({ 0,-Rot,0 });

		//動けるようになるまで動かない
		if (MoveTimer >= 0)
		{
			MoveTimer -= deltaTime;
			return;
		}
		Rot += deltaTime;

		//一周したら死ぬ
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

	VecMath::vec3 Rotation = VecMath::vec3(0, 0, 1);	//正面
	float MoveSpeed = 10;	//移動速度
	float MoveTimer = 0;	//行動を開始するまでの時間
	float Rot = 0;			//回転角度（弧度法）
	float Scale = 0;
	Boss Target;	//回る対象
};




#endif //AROUNDMOVE_H_INCLUDED