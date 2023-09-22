//@file TreasureBox.h

#ifndef COMPONENT_TREASUREBOX_H_INCLUDED
#define COMPONENT_TREASUREBOX_H_INCLUDED
#include "../Engine/Component.h"
#include "ParticleSystem.h"
#include "../Component/Player.h"
#include <stdio.h>

//�󔠃R���|�[�l���g
class TreasureBox :public Component
{
public:
	TreasureBox() = default;			//�R���X�g���N�^
	virtual ~TreasureBox() = default;	//�f�X�g���N�^

	virtual void Start(GameObject& gameObject)override
	{
		//�p�[�e�B�N���ɂ�锚��
		ParticleObj = gameObject.engine->Create<GameObject>(
			"particle explosion", gameObject.GetPos());

		auto ps = ParticleObj->AddComponent<ParticleSystem>();
		ps->Emitters.resize(1);
		auto& emitter = ps->Emitters[0];
		emitter.ep.ImagePath = "Res/Last.tga";

		emitter.ep.Duration = 0.5f;				//���o����
		emitter.ep.EmissionsPerSecond = 80;	//�b�ԕ��o��
		emitter.ep.RandomizeRotation = 1;		//�p�x������
		emitter.ep.RandomizeDirection = 1;
		emitter.ep.RandomizeSize = 1;			//�傫���������_����
		emitter.pp.LifeTime = 0.4f;				//��������
		emitter.pp.color.Set({ 5, 1, 5.5f, 1 }, { 1, 2, 1.5f, 0 });	//�F�t��
		emitter.pp.scale.Set({ 0.5f,0.1f }, { 0.1f,0.05f });	//�T�C�Y�����X�ɂ֕ύX������

		emitter.ep.Loop = true;



	}
	virtual void Update(GameObject& gameObject, float deltaTime)override
	{
	}

	virtual void OnCollision(GameObject& gameObject, GameObject& other)override
	{
		if (other.name == Target->name)
		{
			//�p�[�e�B�N��������
			ParticleObj->SetDeadFlg(true);

			//�V���b�g�K�����V���[�^�[���l���ł���
			if (!Target->GetShooterFlg())
			{
				Target->SetShooterFlg(true);
			}
			else if (!Target->GetShotGunFlg())
			{
				Target->SetShotGunFlg(true);
			}
		}
	}

	void SetTarget(PlayerPtr _p)
	{
		Target = _p;
	}

private:
	GameObjectPtr ParticleObj;
	PlayerPtr Target;
};


#endif // !COMPONENT_TREASUREBOX_H_INCLUDED
