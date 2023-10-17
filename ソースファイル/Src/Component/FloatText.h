//@file FloatText.h

#ifndef COMPONENT_FLOATTEXT_H_INCLUDED
#define COMPONENT_FLOATTEXT_H_INCLUDED
#include "../Engine/Component.h"
#include "Text.h"
#include <stdio.h>
#include "../Component/Lerp.h"

//�c�莞�Ԃ��Ǘ�����R���|�[�l���g
class FloatText :public Component
{
public:
	FloatText() = default;			//�R���X�g���N�^
	virtual ~FloatText() = default;	//�f�X�g���N�^

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{
		//�o�ߎ��Ԃ����Z
		SpendTime += deltaTime / LifeTime;	//�������Ԃɍ��킹��

		//�C�[�W���O�𗘗p���Ĉʒu��ύX����
		gameObject.SetPos(
			vec3(BeforePos.x + Drop(SpendTime) * (TargetPos.x - BeforePos.x),
				BeforePos.y + Drop(SpendTime) * (TargetPos.y - BeforePos.y), 0));

		//�����x�ɕϊ�
		const float alpha = 1 - SpendTime;
		gameObject.SetAlpha(alpha);	//�����x�𔽉f

		//�����ɂȂ�����
		if (alpha <= 0)
		{
			//�ŃQ�[���I�u�W�F�N�g
			gameObject.SetDeadFlg(true);
		}
	}

	void SetBeforePos(vec3 _pos)
	{
		BeforePos = _pos;
	}

private:

	const float LifeTime = 1.5f;	//������܂ł̎���

	float MoveSpeed = 20;	//�ړ����x
	float Alpha = 1;		//�����x
	float SpendTime = 0;	//�o�ߎ���
		
	vec3 TargetPos = { 10,500,0 };	//�����n�_
	vec3 BeforePos = { 10,400,0 };	//�J�n�n�_
};

#endif // !COMPONENT_FLOATTEXT_H_INCLUDED
