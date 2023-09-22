//@file ParticleSystem.h

#ifndef COMPONENT_PARTICLESYSTEM_H_INCLUDED
#define COMPONENT_PARTICLESYSTEM_H_INCLUDED

#include "../Engine/Component.h"
#include "../Engine/Engine.h"

//�p�[�e�B�N���Ǘ��R���|�[�l���g

//�R���|�[�l���g�쐬����Emitter�z��̃T�C�Y��ݒ肵�Aep,pp��ݒ肵�Ă����B
//�����āAUpdate�֐��ŃG�~�b�^�[���쐬

class ParticleSystem :public Component
{
public:
	//�R���X�g���N�^
	ParticleSystem() = default;

	//�f�X�g���N�^
	~ParticleSystem() = default;

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{
		for (auto& e : Emitters)
		{
			//�G�~�b�^�[���쐬
			if (!e.emitter)
			{
				e.emitter = gameObject.engine->AddParticleEmitter(e.ep, e.pp);
			}

			//�p�����[�^���擾���A���W���X�V
			auto& parameter = e.emitter->GetEmitterParameter();
			parameter.position = e.ep.position + gameObject.GetPos();
		}

		auto itr = std::partition(Emitters.begin(), Emitters.end(),
			[](const Parameter& p) {return !p.emitter->IsDead(); });

		Emitters.erase(itr, Emitters.end());

		//���ׂẴG�~�b�^�[�����񂾂�Q�[���I�u�W�F�N�g������
		if (Emitters.empty() && KillWhenEmpty)
		{
			gameObject.SetDeadFlg(true);
		}
	}

	//�j�󏈗�
	virtual void OnDestroy(GameObject& gameObject) override
	{
		for (auto& e : Emitters)
		{
			e.emitter->Die();
		}
	}

	//ep��pp�Ƀp�����[�^�[��ݒ肵�Ă����ƁA�G�~�b�^�[�����������
	struct Parameter
	{
		ParticleEmitterParameter ep;
		ParticleParameter pp;
		ParticleEmitterPtr emitter;
	};



	std::vector<Parameter> Emitters;

	//���ׂẴG�~�b�^�[�����񂾂�Q�[���I�u�W�F�N�g������
	bool KillWhenEmpty = true;
};


#endif //COMPONENT_PARTICLESYSTEM_H_INCLUDED
