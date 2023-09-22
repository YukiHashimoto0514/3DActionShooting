#include "JumpingEnemy.h"
#include "../Engine/Debug.h"
void JumpingEnemy::Update(float deltaTime)
{

	Move(deltaTime);

	Jump(deltaTime);

	if (!this->GetAir())
	{
		MoveTimer += deltaTime;
		CalcForward();	//�n��ɂ���ԂɃv���C���[��ǂ�������
	}

	//�ՂɂՂɎw����
	vec3 scale = GetScale();
	scale.y = std::sin(MoveTimer * 5) * 0.2f + 1;
	SetScale(scale);
	
}

void JumpingEnemy::Move(float deltaTime)
{
	const static float MOVESPEED = 3.0f;

	//�^�[�Q�b�g���w�肳��Ă��Ȃ�������
	if (target == NULL)
	{
		this->AddPosition(vec3(0, 1, 0) * deltaTime);

		return;
	}
	else
	{
		//���ʂɈړ�
		this->AddPosition(this->GetForward() * deltaTime * MOVESPEED);
	}

}

void JumpingEnemy::OnCollision(GameObject& other)
{
	//���������璅�n��Ԃ�
	if (other.name == "floor")
	{
		ChangeScale(JumpState::Ground);
	}
}

void JumpingEnemy::CalcForward()
{
	//�����x�N�g�����擾
	vec3 Forward = this->GetPos() - target->GetPos();

	//���]
	Forward *= vec3(-1);

	//���K��
	Forward = normalize(Forward);

	//�O�ςŁA�E�����x�N�g�����擾
	vec3 Right = cross(Forward, vec3{ 0,1,0 });

	//���K������
	Right = normalize(Right);

	//�ݒ肷��
	this->SetForward(Forward);
	this->SetRight(Right);

	//�p�x���Z�o
	float rad = atan2(Forward.x, Forward.z);

	//���ʕ����ɉ�]
	this->SetRotation(vec3(0, rad, 0));

}

void JumpingEnemy::Jump(float deltaTime)
{
	//�d��
	static const float Gravity = 9.8f;

	//�n��
	if (!this->GetAir())
	{
		//�W�����v���鎞������
		if (MoveTimer >= JumpCoolTime)
		{
			//�W�����v�\
			if (!this->GetJump())
			{
				SetJumpFlg(true);
				SetAir(true);
				v0 = rand() % 5 + 5.0f;			//�W�����v�̗�
				JumpCoolTime = rand() % 3 + 1.0f;	//����ׂ�܂ł̎���
				JumpTimer = 0;
				MoveTimer = 0;
				this->AddPosition(vec3(0, v0 * deltaTime, 0));	//�������ł���
				ChangeScale(JumpState::Up);//�㏸��Ԃ�
			}
		}

		//�d�͂���ɂ�����
		this->AddPosition(vec3(0, -1, 0) * deltaTime);
	}
	else//��
	{
		JumpTimer += deltaTime;

		//�ǂꂭ�炢�����邩��
		const float UpPower = this->v0;
		const float DownPower = Gravity * JumpTimer * JumpTimer;
		FallPower = (UpPower - DownPower) * deltaTime;

		//�ǉ�
		this->AddPosition(vec3(0, FallPower, 0));

		//�������Ȃ�
		if (FallPower <= 0)
		{
			if(state==JumpState::Up)
			ChangeScale(JumpState::Fall);
		}
	}
}

//���V�тŌ`��ό`
void JumpingEnemy::ChangeScale(JumpState _state)
{
	if (state == _state) {
		return;
	}

	state = _state;

	switch (_state)
	{
	case JumpState::Up:
		this->SetScale(vec3(0.5f, 1.5f, 0.5f));
		break;

	case JumpState::Fall:
		this->SetScale(vec3(1.0f, 1.0f, 1.0f));
		break;

	case JumpState::Ground:
		this->SetScale(vec3(1.5f, 0.5f, 1.5f));

		break;

	default:
		break;
	}
}

