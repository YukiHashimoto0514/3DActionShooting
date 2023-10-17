//@file Warp.h

#ifndef COMPONENT_WARP_H_INCLUDED
#define COMPONENT_WARP_H_INCLUDED
#include "../Engine/Component.h"
#include "../Engine/Engine.h"
#include <stdio.h>

//ワープするコンポーネント
class Warp :public Component
{
public:
	Warp() = default;			//コンストラクタ
	virtual ~Warp() = default;	//デストラクタ

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{
		
	}


	virtual void OnCollision(GameObject& gameObject, GameObject& other)override
	{
		//プレイヤーだったら
		if (other.name == "player")
		{
			other.SetPos(WarpExit->GetPos());
		}
	}


	void SetExit(GameObjectPtr _obj)
	{
		WarpExit = _obj;
	}
private:
	GameObjectPtr WarpExit;		//出口のオブジェ
};

#endif // !COMPONENT_WARP_H_INCLUDED
