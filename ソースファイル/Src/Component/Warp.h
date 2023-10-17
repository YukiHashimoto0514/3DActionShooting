//@file Warp.h

#ifndef COMPONENT_WARP_H_INCLUDED
#define COMPONENT_WARP_H_INCLUDED
#include "../Engine/Component.h"
#include "../Engine/Engine.h"
#include <stdio.h>

//���[�v����R���|�[�l���g
class Warp :public Component
{
public:
	Warp() = default;			//�R���X�g���N�^
	virtual ~Warp() = default;	//�f�X�g���N�^

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{
		
	}


	virtual void OnCollision(GameObject& gameObject, GameObject& other)override
	{
		//�v���C���[��������
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
	GameObjectPtr WarpExit;		//�o���̃I�u�W�F
};

#endif // !COMPONENT_WARP_H_INCLUDED
