//@file UnderBoder.h

#ifndef COMPONENT_UNDERBORDER_H_INCLUDED
#define COMPONENT_UNDERBORDER_H_INCLUDED
#include "../Engine/Component.h"
#include "Text.h"
#include <stdio.h>

//落下してきた物を地上に返すコンポーネント
class UnderBorder :public Component
{
public:
	UnderBorder() = default;			//コンストラクタ
	virtual ~UnderBorder() = default;	//デストラクタ

	virtual void OnCollision(GameObject& gameObject, GameObject& other)override
	{
		//何が来ても一旦初期値に返す
		{
			other.SetPos(vec3{ 0,3,0 });
		}
	}

};

#endif // !COMPONENT_UNDERBORDER_H_INCLUDED
