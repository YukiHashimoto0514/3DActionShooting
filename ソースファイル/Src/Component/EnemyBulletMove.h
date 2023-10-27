//@file EnemyBulletMove.h
#ifndef ENEMYBULLETMOVE_H_INCLUDED
#define ENEMYBULLETMOVE_H_INCLUDED

#include "../Engine/Component.h"
#include "../Application/GameObject.h"
#include "../Engine/VecMath.h"
#include "../Component/Player.h"
#include "../Component/ParticleSystem.h"
//#include "../Engine/VecMath.h"

using namespace VecMath;

//�e�R���|�[�l���g
class EnemyBulletMove :public Component
{
public:
	EnemyBulletMove() = default;				//�R���X�g���N�^
	virtual ~EnemyBulletMove() = default;		//�f�X�g���N�^

	virtual void Start(GameObject& gameObject)override
	{
		RandomEngine.seed(rd());

		//�������g�p����Ȃ�
		if (Randamness)
		{
			forward.x += std::clamp(
				std::normal_distribution<float>(0, 0.1f)(rd), -0.2f, 0.2f);	//��̂O�t�߂̗���

			forward.y += std::clamp(
				std::normal_distribution<float>(0, 0.1f)(rd), -0.1f, 0.1f);	//��̂O�t�߂̗���
		}

		////�p�[�e�B�N���ɂ�锚��
		//ParticleObje = gameObject.engine->Create<GameObject>(
		//	"particle explosion", gameObject.GetPos());

		//auto ps = ParticleObje->AddComponent<ParticleSystem>();
		//ps->Emitters.resize(1);
		//auto& emitter = ps->Emitters[0];
		//emitter.ep.ImagePath = "Res/KP.tga";
		//emitter.ep.Duration = 0.1f;				//���o����
		//emitter.ep.EmissionsPerSecond = 50;	//�b�ԕ��o��
		//emitter.ep.RandomizeRotation = 1;		//�p�x������
		//emitter.ep.RandomizeDirection = 1;
		//emitter.ep.RandomizeSize = 1;			//�傫���������_����
		//emitter.pp.LifeTime = 0.4f;				//��������
		//emitter.pp.color.Set({ 5, 1, 0.5f, 1 }, { 1, 2, 1.5f, 0 });	//�F�t��
		//emitter.pp.scale.Set({ 0.01f,0.01f }, { 0.05f,0.1f });	//�T�C�Y�����X�ɂ֕ύX������
		//emitter.ep.Loop = true;

	}

	//���ɓ������Ă��e�͉���
	virtual void OnCollision(GameObject& gameObject, GameObject& other)override
	{
		gameObject.SetDeadFlg(true);

	}

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{
		LiveTime -= deltaTime;

		//��l�̐��ʕ����ɔ��ł���
		gameObject.AddPosition(forward * MoveSpeed * deltaTime);

		//5�b�������玀��
		if (LiveTime <= 0)
		{
			gameObject.SetDeadFlg(true);
		}
		//ParticleObje->SetPos(gameObject.GetPos());
	}

	void SetOwnerinfo(GameObject* _owner, GameObject& gameObject)
	{
		forward = _owner->GetForward();		//���ʕ���

		//�������O����o��������
		gameObject.SetPos(_owner->GetPos() + forward * 2);
		MoveSpeed = 30;
	}

	void SetPlayerinfo(Player* _player, GameObject& _obj)
	{
		forward = _player->GetForward();
		_obj.SetPos(_player->GetPos() + forward * 2);
		MoveSpeed = 50;
	}

	void SetRandom(bool _flg)
	{
		Randamness = _flg;
	}
private:

	VecMath::vec3 forward = VecMath::vec3(0, 0, 1);	//����
	float MoveSpeed = 50;	//�ړ����x
	float LiveTime = 2;		//��������

	bool Randamness = false;//�e�̔�ѕ��ɗ�����������(true=�����@false=�܂�����)

	//����
	std::random_device rd;
	std::mt19937 RandomEngine;

	GameObjectPtr ParticleObje;
};




#endif //BULLETMOVE_H_INCLUDED