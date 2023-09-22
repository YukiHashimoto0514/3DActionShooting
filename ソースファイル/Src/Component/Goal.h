//@file Goal.h
#ifndef GOAL_H_INCLUDED
#define GOAL_H_INCLUDED

#include "../Engine/Component.h"
#include "../Engine/VecMath.h"
#include "../Engine/Debug.h"
#include "../Engine/Engine.h"
#include "../Application/GameObject.h"
#include "../Application/GameClearScene.h"

using namespace VecMath;

//ターゲットを追いかけるコンポーネント
class Goal :public Component
{
public:
	Goal() = default;				//コンストラクタ
	virtual ~Goal() = default;		//デストラクタ

	virtual void OnCollision(GameObject& gameObject, GameObject& other)override
	{
		//ターゲットがゴールまでたどり着いたら
		if (other.name == target->name)
		{
			e->SetNextScene<GameClearScene>();
		}

	}

	void SetTarget(GameObjectPtr _target)
	{
		target = _target;
	}

	void SetEngine(Engine *_engine)
	{
		e = _engine;
	}

private:
	GameObjectPtr target;	//目的地まで到達する人
	Engine* e = NULL;
};




#endif //GOAL_H_INCLUDED