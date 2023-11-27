//@file HitEffect.h
#ifndef HITEFFECT_H_INCLUDED
#define HITEFFECT_H_INCLUDED

#include "../Engine/Component.h"
#include "../Application/GameObject.h"
#include "../Engine/VecMath.h"
#include "../Component/MeshRenderer.h"
#include "../Component/ParticleSystem.h"
//#include "../Engine/VecMath.h"

using namespace VecMath;

//��e�����p�[�e�B�N�����o���R���|�[�l���g
class HitEffect :public Component
{
public:

	HitEffect() = default;				//�R���X�g���N�^
	virtual ~HitEffect() = default;	//�f�X�g���N�^

	virtual void OnTakeDamage(GameObject& gameObject, GameObject& other, const Damage& dammage)override
	{

		const float Time = std::clamp(dammage.amount, 1.0f, 3.0f);

		//�q�b�g�G�t�F�N�g�̐���
		auto ParticleObject = gameObject.engine->Create<GameObject>(
			"particle explosion", gameObject.GetPos());

		auto ps = ParticleObject->AddComponent<ParticleSystem>();
		ps->Emitters.resize(1);
		auto& emitter = ps->Emitters[0];
		emitter.ep.ImagePath = "Res/UI/hit.tga";
		emitter.ep.tiles = { 3,2 };				//�摜����
		emitter.ep.Duration = Time * 0.1f;	//���o����
		emitter.ep.RandomizeSize = 1;			//�傫���������_����
		emitter.ep.RandomizeRotation = 1;		//�p�x������
		emitter.ep.EmissionsPerSecond = 10;		//�b�ԕ��o��
		emitter.pp.LifeTime = 0.3f;				//��������
		emitter.pp.color.Set({ 5, 1, 0.5f, 1 }, { 1, 2, 1.5f, 0 });	//�F�t��
		emitter.pp.scale.Set({ 0.01f,0.01f }, { 0.005f,0.005f });	//�T�C�Y�����X�ɂ֕ύX������
		emitter.pp.velocity.Set({ 0,1,0 }, { 0,-10,0 });//������ɕ��o
	}

};




#endif //HITEFFECT_H_INCLUDED