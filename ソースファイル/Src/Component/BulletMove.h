//@file BulletMove.h
#ifndef BULLETMOVE_H_INCLUDED
#define BULLETMOVE_H_INCLUDED

#include "../Engine/Component.h"
#include "../Application/GameObject.h"
#include "../Engine/VecMath.h"
#include "../Component/Player.h"
//#include "../Engine/VecMath.h"

using namespace VecMath;

//�e�R���|�[�l���g
class BulletMove :public Component
{
public:
	BulletMove() = default;				//�R���X�g���N�^
	virtual ~BulletMove() = default;		//�f�X�g���N�^

	virtual void Start(GameObject& gameObject)override
	{
		RandomEngine.seed(rd());

		//�������g�p����Ȃ�
		if (Randamness)
		{
			forward.x += std::clamp(
				std::normal_distribution<float>(0.0f, 0.05f)(rd), -0.1f, 0.1f);	//��̂O�t�߂̗���

			forward.y += std::clamp(
				std::normal_distribution<float>(0.0f, 0.05f)(rd), -0.1f, 0.1f);	//��̂O�t�߂̗���

			forward.z += std::clamp(
				std::normal_distribution<float>(0.0f, 0.05f)(rd), -0.1f, 0.1f);	//��̂O�t�߂̗���
		}


	}

	//���ɓ������Ă��e�͉���
	virtual void OnCollision(GameObject& gameObject, GameObject& other)override
	{
		//�ʓ����͖���
		if(other.name=="bullet")
		{
			return;
		}

		//���˂��邩�ǂ������`�F�b�N
		if (BoundFlg)
		{
			forward = gameObject.GetReflection();
		}
		else
		{
			gameObject.SetDeadFlg(true);
		}
	}

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{
		LiveTime -= deltaTime;

		//��l�̐��ʕ����ɔ��ł���
		gameObject.AddPosition(forward * MoveSpeed * deltaTime);
		gameObject.SetForward(forward);
		//5�b�������玀��
		if (LiveTime <= 0)
		{
			gameObject.SetDeadFlg(true);
		}
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

	void SetBound(bool _bound)
	{
		BoundFlg = _bound;
	}
private:

	VecMath::vec3 forward = VecMath::vec3(0,0,1);	//����
	float MoveSpeed = 50;	//�ړ����x
	float LiveTime = 2;		//��������
	bool Randamness = false;//�e�̔�ѕ��ɗ�����������(true=�����@false=�܂�����)
	bool BoundFlg = false;	//���˕Ԃ邩�ǂ���(true=���˂�@false=���˂Ȃ�)
	//����
	std::random_device rd;
	std::mt19937 RandomEngine;

};




#endif //BULLETMOVE_H_INCLUDED