//@file ItemObject.h
#ifndef ITEMOBJECT_H_INCLUDED
#define ITEMOBJECT_H_INCLUDED

#include "../Application/GameObject.h"
#include "../Engine/Debug.h"
#include "../Engine/Engine.h"
#include "../Component/Player.h"

//アイテムクラス
class ItemObject :public GameObject
{
public:
	ItemObject() = default;				//コンストラクタ
	virtual ~ItemObject() = default;	//デストラクタ

	virtual void OnCollision(GameObject& object)override;

	void SetSpot(int _spot)
	{
		PowerSpot = _spot;
	}

	PlayerPtr player;
private:

	//強化する部位
	int PowerSpot = 0;
};




#endif //ITEMOBJECT_H_INCLUDED