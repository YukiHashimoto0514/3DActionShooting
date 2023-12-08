//@file BossCubeBullet.h
#ifndef BOSSCUBEBULLET_H_INCLUDED
#define BOSSCUBEBULLET_H_INCLUDED

#include "../Engine/Component.h"
#include "../Application/GameObject.h"
#include "../Engine/VecMath.h"
#include "../Component/Player.h"

using namespace VecMath;

//�{�X��������e�R���|�[�l���g
class BossCubeBullet :public Component
{
public:
	BossCubeBullet() = default;				//�R���X�g���N�^
	virtual ~BossCubeBullet() = default;	//�f�X�g���N�^

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{
		//�������
		LiveTime -= deltaTime;

		//�ړ��\���Ԃ܂őҋ@
		if (MoveTimer >= 0)
		{
			MoveTimer -= deltaTime;
			return;
		}

		//��]�w����
		Rot += deltaTime;

		//�ǂ������ɉ�]������
		const float Rotate = Rot * deltaTime;
		gameObject.AddRotaion(vec3(Rotate));

		//���ʕ������v�Z
		forward = player->GetPos() - gameObject.GetPos();
		forward = normalize(forward);

		//�v���C���[�����ɔ��ł���
		gameObject.AddPosition(forward * MoveSpeed * deltaTime);

		//���Ԃ������玀��
		if (LiveTime <= 0)
		{
			gameObject.SetDeadFlg(true);
		}
	}

	virtual void OnCollision(GameObject& gameObject, GameObject& other)override
	{
		gameObject.SetDeadFlg(true);
	}

	void SetLiveTime(float _time)
	{
		LiveTime = _time;
	}

	void SetMoveTime(float _timer)
	{
		MoveTimer = _timer;
	}

	void SetPlayer(GameObjectPtr _player)
	{
		player = _player;
	}

	void SetMoveSpeed(float _speed)
	{
		MoveSpeed = _speed;
	}
private:

	VecMath::vec3 forward = VecMath::vec3(0, 0, 1);	//����
	float MoveSpeed = 10;	//�ړ����x
	float LiveTime = 60;	//��������
	float MoveTimer = 0;	//�s�����J�n����܂ł̎���
	float Rot = 0;			//��]������

	GameObjectPtr player;
};




#endif //BOSSCUBEBULLET_H_INCLUDED