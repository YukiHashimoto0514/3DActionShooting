//@file DropPowerUp.h

#ifndef COMPONENT_DROPPOWERUP_H_INCLUDED
#define COMPONENT_DROPPOWERUP_H_INCLUDED
#include "../Engine/Component.h"
#include "../Application/GameObject.h"
#include "../Component/Player.h"
#include "../Component/Collider.h"
#include "../Engine/Engine.h"
#include "../Application/ItemObject.h"

//�v���C���[����������R���|�[�l���g

class DropPowerUp :public Component
{
public:

	//�R���X�g���N�^
	DropPowerUp() 
	{ 	
		//������������
		std::random_device rd;
		RandomEngine.seed(rd());
	};	

	virtual ~DropPowerUp() = default;	//�f�X�g���N�^

	virtual void OnDestroy(GameObject& gameObject)override
	{
		//�^�[�Q�b�g���ݒ肳��Ă��Ȃ��Ȃ�A����
		if (!target)
		{
			return;
		}


		CrateEffect(gameObject);

	}

	PlayerPtr target;	//�����Ώ�
	Engine engine;
	const int Red = 0;
	const int Green = 1;
	const int Blue = 2;

private:

	void CrateEffect(GameObject& gameObject)
	{
		//�O�`7�Ń����_���Ȑ��𐶐�
		const int rand = std::uniform_int_distribution<>(0, 7)(RandomEngine);

		//3���傫�������疳��
		if (rand >= 3)
		{
			return;
		}

		//�p�[�e�B�N���ɂ�锚��
		auto ParticleObject = gameObject.engine->Create<ItemObject>(
			"particle explosion", gameObject.GetPos());
		ParticleObject->player = target;	//�Ώۂ̐ݒ�
		ParticleObject->SetSpot(rand);		//�������镔�ʂ̐ݒ�

		auto ps = ParticleObject->AddComponent<ParticleSystem>();
		ps->Emitters.resize(1);
		auto& emitter = ps->Emitters[0];
		emitter.ep.ImagePath = "Res/face.tga";
		emitter.ep.Duration = 0.5f;				//���o����
		emitter.ep.EmissionsPerSecond = 10;	//�b�ԕ��o��
		emitter.ep.RandomizeRotation = 1;		//�p�x������
		emitter.ep.RandomizeDirection = 0;
		emitter.ep.RandomizeSize = 1;			//�傫���������_����
		emitter.pp.LifeTime = 0.4f;				//��������
		emitter.pp.rotation.Set(90.0f,0);

		//�����ɂ���ĐF��ς���
		if (rand == Red)
		{
			emitter.pp.color.Set({ 255, 1, 1, 1 }, { 1, 1, 1.5f, 0 });	//�F�t��
		}
		else if(rand == Green)
		{
			emitter.pp.color.Set({ 2, 255, 1, 1 }, { 1, 1, 1.5f, 0 });	//�F�t��
		}
		else if (rand == Blue)
		{
			emitter.pp.color.Set({ 2, 1, 255.5f, 1 }, { 1, 1, 1.5f, 0 });	//�F�t��
		}
		emitter.pp.scale.Set({ 0.01f,0.01f }, { 0.05f,0.05f });	//�T�C�Y�����X�ɂ֕ύX������
		emitter.pp.radial.Set(0, 10);			//�p�x�����ɑ��x������
		
		emitter.ep.Loop = true;	//���[�v�Đ�������


		//�R���C�_�[�����蓖�Ă�
		auto patcollider = ParticleObject->AddComponent<SphereCollider>();
		patcollider->sphere.Center = vec3(0);
		patcollider->sphere.Radius = 1.0f;
	}


	std::mt19937 RandomEngine;
};


#endif  //COMPONENT_POWERUP_H_INCLUDED