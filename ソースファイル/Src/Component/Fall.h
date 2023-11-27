//@file Fall.h
#ifndef FALL_H_INCLUDED
#define FALL_H_INCLUDED

#include "../Engine/Component.h"
#include "../Application/GameObject.h"
#include "../Engine/VecMath.h"
//#include "../Engine/VecMath.h"

using namespace VecMath;

//落下するコンポーネント
class Fall :public Component
{
public:
	Fall() = default;				//コンストラクタ
	virtual ~Fall() = default;		//デストラクタ

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
	float FallTimer = 0;	//浮いている時間
	float Radias = 0;	//角度
	bool OnFlg = false;	//何かが乗っているかどうか(true=乗ってる　false=乗ってない)
};




#endif //FALL_H_INCLUDED