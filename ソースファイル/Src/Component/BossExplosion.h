//@file BossExplosion.h

#ifndef COMPONENT_BOSSEXPLOSION_H_INCLUDED
#define COMPONENT_BOSSEXPLOSION_H_INCLUDED
#include "../Engine/Component.h"
#include "../Engine/SpritePriority.h"
#include "../Application/GameObject.h"
#include "../Engine/Engine.h"
#include "../Component/ParticleSystem.h"

//�����R���|�[�l���g

class BossExplosion :public Component
{
public:
	BossExplosion() = default;			//�R���X�g���N�^
	virtual ~BossExplosion() = default;	//�f�X�g���N�^

	virtual void OnDestroy(GameObject& gameObject)override
	{
		//�p�[�e�B�N���ɂ�锚��
		auto ParticleObject = gameObject.engine->Create<GameObject>(
			"particle explosion", gameObject.GetPos());

		auto ps = ParticleObject->AddComponent<ParticleSystem>();
		ps->Emitters.resize(1);
		auto& emitter = ps->Emitters[0];
		emitter.ep.ImagePath = "Res/star.tga";
		emitter.ep.Duration = 0.5f;				//���o����
		emitter.ep.EmissionsPerSecond = 30;		//�b�ԕ��o��
		emitter.ep.RandomizeRotation = 1;		//�p�x�����_����������
		emitter.ep.RandomizeDirection = 1;		//�����Ƀ����_������
		emitter.ep.RandomizeSize = 1;			//�傫���������_����
		emitter.pp.rotation.Set(0, radians(720));	//���x�܂ŉ�]�����邩
		emitter.pp.LifeTime = 1.0f;				//��������
		emitter.pp.radial.Set(10, 0);			//����
		emitter.pp.color.Set({ 1, 1, 0.5f, 1 }, { 1, 2, 10.5f, 0 });	//�F�t��
		emitter.pp.scale.Set({ 2,2 }, { 1,1 });	//�T�C�Y�����X�ɂ֕ύX������
	}


	virtual void OnTakeDamage(GameObject& gameObject, GameObject& other, const Damage& dammage)override
	{

		//�q�b�g�G�t�F�N�g�̐���
		auto ParticleObject = gameObject.engine->Create<GameObject>(
			"particle explosion", gameObject.GetPos());

		auto ps = ParticleObject->AddComponent<ParticleSystem>();
		ps->Emitters.resize(1);
		auto& emitter = ps->Emitters[0];
		emitter.ep.ImagePath = "Res/hit.tga";
		emitter.ep.tiles = { 3,2 };				//�摜����
		emitter.ep.Duration = 0.3f;				//���o����
		emitter.ep.RandomizeSize = 1;			//�傫���������_����
		emitter.ep.RandomizeRotation = 1;		//�p�x������
		emitter.ep.EmissionsPerSecond = 10;		//�b�ԕ��o��
		emitter.pp.LifeTime = 0.3f;				//��������
		emitter.pp.color.Set({ 5, 1, 0.5f, 1 }, { 1, 2, 1.5f, 0 });	//�F�t��
		emitter.pp.scale.Set({ 0.11f,0.11f }, { 0.005f,0.005f });	//�T�C�Y�����X�ɂ֕ύX������
		emitter.pp.velocity.Set({ 0,1,0 }, { 0,-10,0 });//������ɕ��o
	}

};



#endif // !COMPONENT_BossExplosion_H_INCLUDED
