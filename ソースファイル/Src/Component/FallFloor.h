//@file FallFloor.h
#ifndef FALLFLOOR_H_INCLUDED
#define FALLFLOOR_H_INCLUDED

#include "../Engine/Component.h"
#include "../Application/GameObject.h"
#include "../Engine/VecMath.h"
//#include "../Engine/VecMath.h"

using namespace VecMath;

//一定時間乗っていると落下するコンポーネント
class FallFloor :public Component
{
public:
	FallFloor() = default;				//コンストラクタ
	virtual ~FallFloor() = default;		//デストラクタ

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{
		//落下するまでの時間
		const static float FallTime = 0.1f;

		//何かが乗っていたら
		if (OnFlg)
		{
			OnTime += deltaTime;
			OnFlg = false;
		}

		//一定時間が過ぎたら
		if (OnTime >= FallTime)
		{
			//横にぶるぶる震える
			Radias += deltaTime*10;
			gameObject.AddPosition(vec3{ sin(Radias) * deltaTime,0,0 });

			//3回震えたら落下
			if (degrees(Radias) >= 360 * 3)
			{
				const float FallPower = -5 * deltaTime;
				gameObject.AddPosition(vec3{ 0,FallPower,0 });
			}
		}

		//一定ラインよりも下に来たら
		if (gameObject.GetPos().y <= -5)
		{
			//ゲームオブジェクトを削除
			gameObject.SetDeadFlg(true);
		}
	}
	virtual void OnCollision(GameObject& gameObject, GameObject& other)override
	{
		//何かが乗っている
		OnFlg = true;
	}

private:

	float OnTime = 0;	//何かが乗っている時間
	float Radias = 0;	//角度
	bool OnFlg = false;	//何かが乗っているかどうか(true=乗ってる　false=乗ってない)
};




#endif //FALLFLOOR_H_INCLUDED