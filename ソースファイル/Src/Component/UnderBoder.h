//@file UnderBoder.h

#ifndef COMPONENT_UNDERBORDER_H_INCLUDED
#define COMPONENT_UNDERBORDER_H_INCLUDED
#include "../Engine/Component.h"
#include "Text.h"
#include <stdio.h>

//�������Ă�������n��ɕԂ��R���|�[�l���g
class UnderBorder :public Component
{
public:
	UnderBorder() = default;			//�R���X�g���N�^
	virtual ~UnderBorder() = default;	//�f�X�g���N�^

	virtual void OnCollision(GameObject& gameObject, GameObject& other)override
	{
		//�������Ă���U�����l�ɕԂ�
		{
			other.SetPos(vec3{ 0,3,0 });
		}
	}

};

#endif // !COMPONENT_UNDERBORDER_H_INCLUDED
