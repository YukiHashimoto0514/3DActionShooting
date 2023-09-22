//@file JumpingEnemy.h
#ifndef JUMPINGNENEMY_H_INCLUDED
#define JUMPINGENEMY_H_INCLUDED
#include "../Engine/Sprite.h"
#include "../Engine/Rect.h"
#include "../Engine/Component.h"
#include "../Engine/VecMath.h"
#include "../Application/EnemyBase.h"
#include "../Engine/Engine.h"

using namespace VecMath;

enum class JumpState
{
	Up,		//�㏸��
	Fall,	//������
	Ground,	//���n��
};

//�W�����v���Ȃ���ǂ������Ă���G
class JumpingEnemy :public EnemyBase
{
public:
	JumpingEnemy() = default;			//�R���X�g���N�^
	virtual ~JumpingEnemy() = default;	//�f�X�g���N�^

	//�C�x���g�̑���
	virtual void Update(float deltaTime)override;
	virtual void Move(float deltaTime)override;
	virtual void OnCollision(GameObject& other);


	//�^�[�Q�b�g��ݒ�
	void SetTarget(GameObjectPtr _target)
	{
		target = _target;
	}

private:

	//���ʕ������v�Z����
	void CalcForward();

	//�W�����v����֐�
	void Jump(float deltaTime);

	//�`��ς���
	void ChangeScale(JumpState _state);

	GameObjectPtr target;	//�G

	float MoveTimer = 0;		//���b�ړ����������L�^����
	float JumpCoolTime = 0.5f;	//�W�����v����܂ł̃N�[���^�C��
	float FallPower = 0;		//�������ɂ������
	bool OnceFlg = false;		//��x�����ʂ��Ăق����t���O

	
	JumpState state = JumpState::Ground;
};





#endif //JUMPINGENEMY_H_INCLUDED