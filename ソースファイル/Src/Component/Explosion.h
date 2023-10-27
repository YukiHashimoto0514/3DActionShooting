//@file Explosion.h

#ifndef COMPONENT_EXPLOSION_H_INCLUDED
#define COMPONENT_EXPLOSION_H_INCLUDED
#include "../Engine/Component.h"
#include "../Engine/SpritePriority.h"
#include "../Application/GameObject.h"
#include "../Engine/Engine.h"
#include "../Application/EasyAudio.h"
#include "../Engine/AudioSettings.h"

#include "ParticleSystem.h"

//�����R���|�[�l���g

class Explosion :public Component
{
public: 
	Explosion() = default;			//�R���X�g���N�^
	virtual ~Explosion() = default;	//�f�X�g���N�^

	virtual void OnDestroy(GameObject& gameObject)override
	{
		//�p�[�e�B�N���ɂ�锚��
		auto ParticleObject = gameObject.engine->Create<GameObject>(
			"particle explosion", gameObject.GetPos());

		auto ps = ParticleObject->AddComponent<ParticleSystem>();
		ps->Emitters.resize(1);
		auto& emitter = ps->Emitters[0];
		emitter.ep.ImagePath = "Res/UI/Exp.tga";
		emitter.ep.tiles = { 1,1 };
		emitter.ep.Duration = 0.5f;				//���o����
		emitter.ep.EmissionsPerSecond = 50;		//�b�ԕ��o��
		emitter.ep.RandomizeRotation = 1;		//�p�x������
		emitter.ep.RandomizeDirection = 1;
		emitter.ep.RandomizeSize = 1;			//�傫���������_����
		emitter.pp.LifeTime = 0.4f;				//��������
		emitter.pp.color.Set({ 5, 1, 0.5f, 1 }, { 1, 2, 1.5f, 0 });	//�F�t��
		emitter.pp.scale.Set({ 0.05f,0.05f }, { 0.001f,0.001f });	//�T�C�Y�����X�ɂ֕ύX������
		//emitter.ep.Shape = ParticleEmitterShape::box;]

		//���ʉ���炷
		Audio::PlayOneShot(SE::enemyExplosionD, 0.2f);

	}

};



#endif // !COMPONENT_EXPLOSION_H_INCLUDED
