#include "DeviationEnemy.h"
#include "../Component/BulletMove.h"
#include "../Component/MeshRenderer.h"
#include "../Component/Collider.h"
#include "../Component/DamageSource.h"

void DeviationEnemy::Update(float deltaTime)
{
	//���ꂼ��̃X�e�[�g���Ƃɏ���
	switch (state)
	{
	case State::eChase:
		MoveTimer += deltaTime;

		//���̃X�e�[�g�Ɉڍs
		if (MoveTimer >= CHASETIME)
		{
			ChangeState(State::eStop);
		}

		//���ʕ���������
		CalcForward();

		//�ړ�����
		Move(deltaTime);
		break;

	case State::eStop:
		MoveTimer += deltaTime;

		//���̃X�e�[�g�Ɉڍs
		if (MoveTimer >= CHARGETIME)
		{
			ChangeState(State::eShotWait);
			OnceFlg = true;
		}

		//���ʂ��v�Z
		CalcForward();

		break;
	case State::eShotWait:

		if (OnceFlg)
		{
			//�p�x�������A�΍�������
			AddDeviation();
			OnceFlg = false;
		}

		if (Wait(deltaTime))
		{
			//�X�e�[�g���X�V
			ChangeState(State::eShot);
		}
		break;

	case State::eShot:

		//�ˌ�
		Shot(deltaTime);

		break;

	case State::eChaseWait:

		if (Wait(deltaTime))
		{
			ChangeState(State::eChase);
		}
		break;

	default:
		break;
	}
}

void DeviationEnemy::Move(float deltaTime)
{
	static const float DISTANCE = 20.0f;	//�v���C���[�Ƃ̋���


	float dx = target->GetPos().x - this->GetPos().x;
	float dy = target->GetPos().z - this->GetPos().z;

	//�^�[�Q�b�g���w�肳��Ă��Ȃ�������
	if (target == NULL)
	{
		this->SetPos(this->GetPos() + vec3(0, 1, 0) * deltaTime);

		return;
	}
	//����������l�ɒB���Ă�����
	else if (sqrt(dx* dx + dy* dy) <= DISTANCE)
	{
		//���̍s���܂ł̎��Ԃ�����������
		MoveTimer += deltaTime;
		return;
	}
	//������������x����Ă�����l�߂�
	else
	{
		//���ʂɈړ�
		this->AddPosition(this->GetForward() * deltaTime * 5);
	}

}

void DeviationEnemy::Shot(float deltaTime)
{

	//�e�̐���
	{
		auto enemybullet = this->engine->Create<GameObject>("enemybullet");
		 
		auto enemybulletRenderer = enemybullet->AddComponent<MeshRenderer>();
		enemybulletRenderer->mesh = this->engine->LoadOBJ("Sphere");
		enemybulletRenderer->scale = vec3(0.5f);

		//���ʕ����ɐi��
		auto enemymove = enemybullet->AddComponent<BulletMove>();
		enemymove->SetOwnerinfo(this, *enemybullet);

		//�R���C�_�[�̐ݒ�
		auto enemybulletcollider = enemybullet->AddComponent<SphereCollider>();
		enemybulletcollider->sphere.Center = enemybulletRenderer->translate;
		enemybulletcollider->sphere.Radius = std::max({
			enemybulletRenderer->scale.x,
			enemybulletRenderer->scale.y,
			enemybulletRenderer->scale.z });

		//�_���[�W��^�������w��
		auto da = enemybullet->AddComponent<DamageSource>();
		da->targetName = "player";
		da->isOnce = true;

		//�X�e�[�g��ς���
		ChangeState(State::eChaseWait);
	}

}

void DeviationEnemy::CalcForward()
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

//�p�x��������
void DeviationEnemy::AddDeviation()
{
	vec3 Pos;

	int rnd = rand() % 3;
	//�����_���ō��E�ǂ��炩���ʂ�����
	if (rnd == 0)
	{
		//�������݂�
		Pos = target->GetPos() - target->GetRight() * 2;
	}
	else if (rnd == 1)
	{
		//�E�����݂�
		Pos = target->GetPos() + target->GetRight() * 2;
	}
	else
	{
		//�������Ȃ�
		MoveTimer = 1.5f;
		return;
	}
	vec3 forward = this->GetPos() - Pos;

	//���]
	forward *= vec3(-1);

	//���K��
	forward = normalize(forward);

	//���ʂ��Đݒ�
	this->SetForward(forward);

	//�p�x���Z�o
	float rad = atan2(forward.x, forward.z);

	//���ʕ����ɉ�]
	this->SetRotation(vec3(0, rad, 0));
	MoveTimer = 1.5f;
}

bool DeviationEnemy::Wait(float deltaTime)
{
	MoveTimer -= deltaTime;

	if (MoveTimer <= 0)
	{
		//�҂����Ԃ��I������
		return true;
	}

	//�܂��҂��Ă�
	return false;
}

void DeviationEnemy::ChangeState(State nextstate)
{
	state = nextstate;

	if (nextstate == State::eShotWait)
	{
		//�ҋ@���Ԃ�ݒ肷��
		MoveTimer = SHOTWAIT;
	}
	else if (nextstate == State::eChaseWait)
	{
		//�ҋ@���Ԃ�ݒ肷��
		MoveTimer = CHASEWAIT;
	}
	else
	{
		//�^�C�}�[�����Z�b�g
		MoveTimer = 0;
	}

}

