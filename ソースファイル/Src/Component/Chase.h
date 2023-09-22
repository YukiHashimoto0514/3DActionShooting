//@file Chase.h
#ifndef CHASE_H_INCLUDED
#define CHASE_H_INCLUDED

#include "../Engine/Component.h"
#include "../Application/GameObject.h"
#include "../Engine/VecMath.h"
//#include "../Engine/VecMath.h"

using namespace VecMath;

//ターゲットを追いかけるコンポーネント
class Chase :public Component
{
public:
	Chase() = default;				//コンストラクタ
	virtual ~Chase() = default;		//デストラクタ

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{
		//標的を設定していないら、無視
		if (target == NULL)
		{
			return;
		}
		//MoveSpeed += deltaTime;
		//向きベクトルを取得
		vec3 Look = gameObject.GetPos() - target->GetPos();

		//正規化
		normalize(Look);

		//角度を計算
		float theta = atan2(Look.x, Look.z);

		//進みたい方向に回転
		vec3 forward = vec3(sin(theta),
			0,
			cos(theta));

		//反転
		forward *= vec3(-1);

		//移動処理
		gameObject.AddPosition(forward * MoveSpeed * deltaTime);

	}

	void SetTarget(GameObjectPtr _target)
	{
		target = _target;
	}
private:
	GameObjectPtr target;	//追いかける対象

	float MoveSpeed = 5;	//移動速度
};




#endif //CHASE_H_INCLUDED