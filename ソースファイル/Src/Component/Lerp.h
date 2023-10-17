//@file Lerp.h

#ifndef COMPONENT_LERP_H_INCLUDED

#define COMPONENT_LERP_H_INCLUDED

#include "../Engine/Component.h"
#include "../Application/GameObject.h"
#include <math.h>

//ゲームオブジェクトを移動する
class Lerp :public Component
{
public:
	Lerp() = default;			//コンストラクタ
	virtual ~Lerp() = default;	//デストラクタ

	virtual void Start(GameObject& gameObject)override
	{
		BeforeX = gameObject.GetPos().x;	//最初の位置を代入
		BeforeY = gameObject.GetPos().y;	//最初の位置を代入
		move = 0;				//移動量を初期化
		TakeTime = 0;			//かかる時間を初期化
	}

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{
		if (TakeTime < 1) 
		{
			//最大時間によって加算する値が変化する
			TakeTime += deltaTime / MaxTime;
		}
		else
		{
			TakeTime = 1;
		}

		//移動計算
		MoveCalc(SetCalc);

		//ゲームオブジェクトの移動
		gameObject.SetPos(vec3(BeforeX + move * (TargetX - BeforeX),
			BeforeY + move * (TargetY - BeforeY), 0));

		//gameObject.x = (1 - move) * BeforeX + TakeTime * TargetX;
		//gameObject.y = (1 - move) * BeforeY + TakeTime * TargetY;
	}


	//移動の計算を決める関数
	void  MoveCalc(int _num)
	{
		if (_num == 0)	//一回通り過ぎる
		{
			move = 1 + 2.7f * static_cast<float>(pow(TakeTime - 1, 3)) + 1.7f * static_cast<float>(pow(TakeTime - 1, 2));
		}
		else if (_num == 1)	//最初の位置に戻ってくる
		{
			if (TakeTime < 0.5)
			{
				move = static_cast<float>(pow(TakeTime, 2)) + 0.5f;
			}
			else if (TakeTime >= 0.5f && TakeTime < 1)
			{
				move = -static_cast<float>(pow(TakeTime, 2)) + 1;
			}
		}
		else if (_num == 2)	//めっちゃ加速してつく
		{
			move = static_cast<float>(pow(TakeTime, 5));
		}
	}


	float BeforeX = 0, BeforeY = 0;	//計算を始める前の座標
	float move = 0;		        //実行中に加算する値
	float TargetX = 400;	//目標地点
	float TargetY = 500;	//目標地点

	float MaxTime = 1;		//移動にかける時間
	float TakeTime = 0;		//実行中にかかかる時間
	int SetCalc = 0;

};

//角ばって0~1~0
inline float Scaling(float deltaTime,float val)
{
	if (deltaTime <= 0.5)
	{
		val = static_cast<float>(pow(deltaTime * 2, 2));
	}
	else if (deltaTime > 0.5 && deltaTime < 1)
	{
		val = 1 - static_cast<float>(pow(deltaTime * 2 - 1, 2));
	}
	return val;
}

//滑らかに0~1~0
inline float Smooth(float deltaTime)
{
	return  -(cos(3.14f * deltaTime * 2) - 1) / 2;
}

//一番速い
inline float Drop(float deltaTime)
{
	return static_cast<float>(pow(2, 10 * deltaTime - 10));
}
//最後に跳ねる（通り越す）
inline float Bound(float deltaTime)
{
	return 1 + 2.7f * static_cast<float>(pow(deltaTime - 1, 3)) + 
		1.7f * static_cast<float>(pow(deltaTime - 1, 2));
}

//最初に跳ねる（通り越す）
inline float Elastic(float deltaTime)
{
	return static_cast<float>(pow(2, -10 * deltaTime)) * 
		sin((deltaTime * 10 - 0.75f) * (2 * 3.14f) / 3) + 1;
}

//真ん中がすごく早い
inline float Circle(float deltaTime)
{
	if (deltaTime < 0.5)
	{
		return (1 - static_cast<float>(sqrt(1 - pow(2 * deltaTime, 2)))) / 2;
	}
	else
	{
		return (static_cast<float>(sqrt(1 - pow(-2 * deltaTime + 2, 2))) + 1) / 2;
	}
}

//ぼよんぼよん
inline float Soft(float deltaTime)
{
	if (deltaTime < 0.5)
	{
		return static_cast<float>(-(pow(2, 20 * deltaTime - 10) * (sin(20 * deltaTime - 11.125f) * 2 * 3.14f / 4.5f))) * 0.5f;
	}
	else
	{
		return static_cast<float>(pow(2, -20 * deltaTime + 10) * (sin(20 * deltaTime - 11.125f) * 2 * 3.14f / 4.5f)) / 2 + 1;
	}

}

#endif //COMPONENT_LERP_H_INCLUDED
