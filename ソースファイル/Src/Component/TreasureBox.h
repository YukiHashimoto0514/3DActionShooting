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
		//�p�[�e�B�N���ɔh��ڂȃG�t�F�N�g
		ParticleObj = gameObject.engine->Create<GameObject>(
			"particle explosion", gameObject.GetPos());

		auto ps = ParticleObj->AddComponent<ParticleSystem>();
		ps->Emitters.resize(1);
		auto& emitter = ps->Emitters[0];
		
			emitter.ep.ImagePath = "Res/UI/star.tga";
			emitter.ep.Duration = 0.1f;				//���o����
			emitter.ep.RandomizeSize = 1;			//�傫���������_����
			emitter.ep.RandomizeRotation = 1;		//�p�x������
			emitter.ep.EmissionsPerSecond = 10;		//�b�ԕ��o��
			emitter.pp.LifeTime = 3.0f;				//��������
			emitter.pp.color.Set({ 5, 1, 0.5f, 1 }, { 1, 2, 1.5f, 0 });	//�F�t��
			emitter.pp.scale.Set({ 0.01f,0.01f }, { 0.005f,0.005f });	//�T�C�Y�����X�ɂ֕ύX������
			emitter.pp.velocity.Set({ 0,25,0 }, { 0,-1,0 });//������ɕ��o

			emitter.ep.Loop = true;



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
				gameObject.componentList.clear();	//�����񓖂���Ȃ��悤�ɂ���
			}
			else if (!Target->GetShotGunFlg())
			{
				Target->SetShotGunFlg(true);
				gameObject.componentList.clear();	//�����񓖂���Ȃ��悤�ɂ���
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
