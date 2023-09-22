//@file anotherExplosion.h

#ifndef COMPONENT_ANOTHEREXPLOSION_H_INCLUDED
#define COMPONENT_ANOTHEREXPLOSION_H_INCLUDED
#include "../Engine/Component.h"
#include "../Engine/SpritePriority.h"
#include "../Application/GameObject.h"
#include "../Engine/Engine.h"
#include "Animator2D.h"

//�����R���|�[�l���g

class anotherExplosion :public Component
{
public:
	anotherExplosion() = default;			//�R���X�g���N�^
	virtual ~anotherExplosion() = default;	//�f�X�g���N�^

	virtual void OnDestroy(GameObject& gameObject)override
	{
		//�����A�j���[�V�����f�[�^
		const Texcoord tc[] = {
			MakeTexcoord(0,520,120,120),
			MakeTexcoord(120,520,120,120),
			MakeTexcoord(240,520,120,120),
			MakeTexcoord(360,520,120,120),
			MakeTexcoord(480,520,120,120),
			MakeTexcoord(600,520,120,120),
			MakeTexcoord(720,520,120,120),
		};

		//�����̕\�����W�f�[�^
		const float x[] = { 0,-40,60,-50,70 };
		const float y[] = { 0,50,70,-60,-40 };

		//������\��
		int i = static_cast<int>(std::size(x));

		if (count < i)
		{
			i = count;
		}

		for (--i; i >= 0; --i)
		{
			auto explosion = gameObject.engine->Create<GameObject>(
				"explosion", gameObject.GetPos());

			explosion->AddSprite(tc[0]);			//�����摜
			explosion->SetPriority(prioExplosion);	//�D�揇��
			explosion->SetScale(vec3(scale,scale,scale));				//�傫��

			auto anim = explosion->AddComponent<Animator2D>();	//�A�j���[�V��������������
			anim->loopTime = false;		//���[�v�Đ����Ȃ�
			anim->killWhenEnd = true;	//�Đ��������ɃQ�[���I�u�W�F�N�g���폜
			anim->clip.resize(std::size(tc));
			anim->interval = 0.07;		//�A�j���[�V�����̍Đ����x
			for (size_t frameNo = 0; frameNo < std::size(tc); ++frameNo)
			{
				anim->AddSprite(frameNo, tc[frameNo], x[i], y[i]);
			}
		}
	}

	int count = 4;		//������
	float scale = 1;	//�����摜�̑傫��
	float mx = 0; float my = 0;	//���炵�ĕ\�����邽�߂̈ړ��l
};



#endif // !COMPONENT_ANOTHEREXPLOSION_H_INCLUDED
