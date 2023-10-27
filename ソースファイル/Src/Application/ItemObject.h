//@file ItemObject.h
#ifndef ITEMOBJECT_H_INCLUDED
#define ITEMOBJECT_H_INCLUDED

#include "../Application/GameObject.h"
#include "../Engine/Debug.h"
#include "../Engine/Engine.h"
#include "../Component/Player.h"

//�A�C�e���N���X
class ItemObject :public GameObject
{
public:
	ItemObject() = default;				//�R���X�g���N�^
	virtual ~ItemObject() = default;	//�f�X�g���N�^

	virtual void OnCollision(GameObject& object)override;

	void SetSpot(int _spot)
	{
		PowerSpot = _spot;
	}

	PlayerPtr player;
private:

	//�������镔��
	int PowerSpot = 0;
};




#endif //ITEMOBJECT_H_INCLUDED