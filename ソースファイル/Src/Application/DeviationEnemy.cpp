#include "DeviationEnemy.h"
#include "../Component/EnemyBulletMove.h"
#include "../Component/MeshRenderer.h"
#include "../Component/Collider.h"
#include "../Component/DamageSource.h"

void DeviationEnemy::Update(float deltaTime)
{
	//���ꂼ��̃X�e�[�g���Ƃɏ���
	switch (state)
	{
	case State::eChase:
	{
		MoveTimer += deltaTime;

		//���̃X�e�[�g�Ɉڍs
		if (MoveTimer >= CHASETIME)
		{
			ChangeState(State::eStop);
		}

		static const float MINDISTANCE = 10.0f;	//�v���C���[�Ƃ̍ŏ�����
		static const float MAXDISTANCE = 80.0f;	//�v���C���[�Ƃ̍ő勗��

		//�v���C���[�̋������v�Z
		float dx = target->GetPos().x - this->GetPos().x;
		float dy = target->GetPos().z - this->GetPos().z;
		const float distance = sqrt(dx * dx + dy * dy);

		//���ȓ��̋�����������
		if (distance >= MINDISTANCE && distance <= MAXDISTANCE)
		{
			//�v���C���[�̕����Ɉړ�
			Move(deltaTime);
		}
		//����������l��菬����������
		else if (distance <= MINDISTANCE)
		{
			//���ʕ���������
			CalcForward(target->GetPos());

			//���̍s���܂ł̎��Ԃ�����������
			MoveTimer += deltaTime;
		}
		else
		{
			RandomMove(deltaTime);
		}

		break;
	}
	case State::eStop:
		MoveTimer += deltaTime;

		//���̃X�e�[�g�Ɉڍs
		if (MoveTimer >= CHARGETIME)
		{
			ChangeState(State::eShotWait);
			OnceFlg = true;
		}

		//���ʂ��v�Z
		CalcForward(target->GetPos());

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
	//���ʕ���������
	CalcForward(target->GetPos());

	//���ʂɈړ�
	this->AddPosition(this->GetForward() * deltaTime * 5);
}

void DeviationEnemy::Shot(float deltaTime)
{

	//�e�̐���
	{
		auto enemyBullet = this->engine->Create<GameObject>("enemybullet");
		 
		auto enemyBulletRenderer = enemyBullet->AddComponent<MeshRenderer>();
		enemyBulletRenderer->mesh = this->engine->LoadOBJ("Sphere");
		enemyBulletRenderer->scale = vec3(0.5f);

		//�ʂɐF��ύX�ł���悤�ɁA�}�e���A���̃R�s�[���쐬
		enemyBulletRenderer->materials.push_back(
			std::make_shared<Mesh::Material>(*enemyBulletRenderer->mesh->materials[0]));
		enemyBulletRenderer->materials[0]->baseColor = vec4{ 3,0.4f,1.2f,1 };
		enemyBulletRenderer->materials[0]->receiveShadows = false;

		//���ʕ����ɐi��
		auto enemyMove = enemyBullet->AddComponent<EnemyBulletMove>();
		enemyMove->SetOwnerinfo(this, *enemyBullet);

		//�R���C�_�[�̐ݒ�
		auto enemyBulletCollider = enemyBullet->AddComponent<SphereCollider>();
		enemyBulletCollider->sphere.Center = enemyBulletRenderer->translate;
		enemyBulletCollider->sphere.Radius = std::max({
			enemyBulletRenderer->scale.x,
			enemyBulletRenderer->scale.y,
			enemyBulletRenderer->scale.z });

		//�_���[�W��^�������w��
		auto da = enemyBullet->AddComponent<DamageSource>();
		da->targetName = "player";
		da->isOnce = true;

		//�X�e�[�g��ς���
		ChangeState(State::eChaseWait);
	}

}

void DeviationEnemy::CalcForward(vec3 target)
{
	//�����x�N�g�����擾
	vec3 Forward = this->GetPos() - target;

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

void DeviationEnemy::RandomMove(float deltaTime)
{
	//��񂾂�����
	if (!OnceFlg)
	{
		//�����_���̈ʒu�𐶐�
		const float x = std::uniform_real_distribution<float>(-10, 10)(engine->GetRandomGenerator());//-10~10
		const float z = std::uniform_real_distribution<float>(-10, 10)(engine->GetRandomGenerator());//-10~10
		RandomPos = { this->GetPos().x + x,this->GetPos().y,this->GetPos().z + z };
		OnceFlg = true;
	}
	CalcForward(RandomPos);

	//�����Ă�������Ɉړ�
	this->AddPosition(this->GetForward() * deltaTime * 5);
	MoveTimer -= deltaTime;		//�U���Ɉڍs���Ȃ�
	WaitTime += deltaTime;		//�ҋ@���Ԃ�ǉ�

	//���W�ɓ���������A������x�ړ�
	if (this->GetPos().x - RandomPos.x < 1 && this->GetPos().z - RandomPos.z < 1)
	{
		ChangeState(State::eChaseWait);
	}

	//�͈͊O�̖ڕW�ɑ��葱�����ꍇ
	if (WaitTime >= CHASETIME)
	{
		//���Ɉړ�
		ChangeState(State::eChaseWait);
	}
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
		WaitTime = 0;
		OnceFlg = false;

	}
	else
	{
		//�^�C�}�[�����Z�b�g
		MoveTimer = 0;
	}

}

