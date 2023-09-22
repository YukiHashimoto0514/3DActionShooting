//@file MegaExplosion.h

#ifndef COMPONENT_MEGAEXPLOSION_H_INCLUDED
#define COMPONENT_MEGAEXPLOSION_H_INCLUDED
#include "../Engine/Component.h"
#include "../Engine/SpritePriority.h"
#include "../Application/GameObject.h"
#include "../Engine/Engine.h"
#include "Animator2D.h"

//�����R���|�[�l���g

class MegaExplosion :public Component
{
public:
	MegaExplosion() = default;			//�R���X�g���N�^
	virtual ~MegaExplosion() = default;	//�f�X�g���N�^

	virtual void OnDestroy(GameObject& gameObject)override
	{
		//�p�[�e�B�N���ɂ�锚��
		auto ParticleObject = gameObject.engine->Create<GameObject>(
			"particle explosion", gameObject.GetPos());

		auto ps = ParticleObject->AddComponent<ParticleSystem>();
		ps->Emitters.resize(1);
		auto& emitter = ps->Emitters[0];
		emitter.ep.ImagePath = "Res/KP.tga";
		emitter.ep.Duration = 0.5f;				//���o����
		emitter.ep.EmissionsPerSecond = 30;		//�b�ԕ��o��
		emitter.ep.RandomizeRotation = 1;		//�p�x�����_����������
		emitter.ep.RandomizeDirection = 1;		//�����Ƀ����_������
		emitter.ep.RandomizeSize = 1;			//�傫���������_����
		emitter.pp.rotation.Set(0, radians(720));	//���x�܂ŉ�]�����邩
		emitter.pp.LifeTime = 1.0f;				//��������
		emitter.pp.radial.Set(2, 0);			//����
		emitter.pp.color.Set({ 1, 1, 0.5f, 1 }, { 1, 2, 10.5f, 0 });	//�F�t��
		emitter.pp.scale.Set({ 0.05f,0.05f }, { 0.005f,0.01f });	//�T�C�Y�����X�ɂ֕ύX������
	}

};



#endif // !COMPONENT_EXPLOSION_H_INCLUDED
