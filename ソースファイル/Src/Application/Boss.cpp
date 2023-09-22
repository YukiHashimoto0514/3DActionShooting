#include "Boss.h"
#include "../Component/Lerp.h"
#include "../Component/MeshRenderer.h"
#include "../Component/Collider.h"
#include "../Component/BulletMove.h"
#include "../Component/BossCubeBullet.h"
#include "../Component/DamageSource.h"
#include "../Component/ARoundMove.h"
#include "../Application/GameClearScene.h"

void Boss::Update(float deltaTime)
{
	//HP�������Ȃ�����
	if (this->GetHP() <= 0)
	{
		//�t�F�[�h������������
		if (engine->GetFadeState() == Engine::FadeState::Closed)
		{
			//�N���A�I�I
			engine->SetNextScene<GameClearScene>();
		}
		return;
	}

	RoutineUpdate(State, deltaTime);
}

void Boss::TakeDamage(GameObject& other, const Damage& damage)
{
	//�q�b�g�|�C���g�����炷
	this->SetHP(this->GetHP() - damage.amount);

	//�q�b�g�|�C���g�������Ȃ��Ă�����
	if (this->GetHP() <= 0)
	{
		//�t�F�[�h�A�E�g���J�n
		engine->StartFadeOut();
	}

}


//state�̃A�b�v�f�[�g���e�X����
void Boss::RoutineUpdate(Routine _state,float deltaTime)
{
	switch (_state)
	{
	case Routine::Search:
	{
		if (player == NULL)
		{
			//�v���C���[�I�u�W�F�N�g���������Ď擾
			player = engine->SearchGameObject("player");
		}

		ChangeState(Routine::Think);
		break;
	}
	case Routine::Think://������HP���l�����čs�������߂�
	{
		//�v���C���[�Ƌ������v�Z
		const float dx = player->GetPos().x - this->GetPos().x;
		const float dz = player->GetPos().z - this->GetPos().z;

		//�v���C���[�Ƃ̋���
		const float Dis = sqrt(dx * dx + dz * dz);


		//�ߋ����U���Ɉڍs���鋗��
		const static float MaxDistance = 30;
		const static float MinDistance = 15;

		//MaxDistance�ȏ㗣��Ă�����
		if (Dis >= MaxDistance)
		{
			//�ő�HP�̔����ȏ�Ȃ�
			if (this->GetHP() >= MaxHP / 2)
			{
				//�߂Â�
				ChangeState(Routine::MovePlayerPos);
			}
			else//���������Ȃ�
			{
				//�������U��������
				ChangeState(Routine::FarAttack);
			}
		}
		else if(Dis >= MinDistance)
		{
			//�������U��������
			ChangeState(Routine::FarAttack);
		}
		else
		{
			//�ߋ����U��
			ChangeState(Routine::NearAttack);
		}
		break;
	}
	case Routine::MovePlayerPos://�ォ��P���Ă���
	{
		MoveTimer += deltaTime;

		if (MoveTimer <= 1)
		{
			const float move = static_cast<float>(pow(MoveTimer, 5));

			this->SetPos(vec3{ this->GetPos().x,beforePos.y + move * (30 - beforePos.y),this->GetPos().z });


		}
		else if (MoveTimer <= 2)
		{
			const float move = static_cast<float>(pow(MoveTimer / 2, 5));

			this->SetPos(vec3{ beforePos.x + move * (targetPos.x - beforePos.x),this->GetPos().y,beforePos.z + move * (targetPos.z - beforePos.z) });

		}
		else if (MoveTimer <= 3)
		{
			const float move = static_cast<float>(pow(MoveTimer /3, 5));

			this->SetPos(vec3{ this->GetPos().x,30 + move * (targetPos.y - 30),this->GetPos().z });
		}
		else
		{
			ChangeState(Routine::Search);
		}
		break;
	}
	case Routine::MoveHomePos:
	{
		const float move = static_cast<float>(pow(TakeTime, 5));

		this->SetPos(beforePos + move * (targetPos - beforePos));


		if (TakeTime < 1)
		{
			TakeTime += deltaTime;
		}
		else
		{
			WaitTime = 1;
			ChangeState(Routine::Wait);
		}
	}

		break;

	case Routine::NearAttack://�_������Ĉ��]
	{
		if (MoveTimer == 0)
		{
			GameObjectPtr AttackBox = this->engine->Create<GameObject>("Box");

			auto boxRenderer = AttackBox->AddComponent<MeshRenderer>();
			boxRenderer->mesh = engine->LoadOBJ("Box");
			boxRenderer->scale = vec3({ 10,1,1 });	//����
			AttackBox->SetPos(this->GetPos());
			AttackBox->AddPosition({ 0,10,0 });		//��������ďo��
			AttackBox->SetMoveFlg(true);			//��������

			//�ʂɐF��ς��邽�߂ɁA�}�e���A�����R�s�[
			boxRenderer->materials.push_back(
				std::make_shared<Mesh::Material>(*boxRenderer->mesh->materials[0]));

			//�R���C�_�[�����蓖�Ă�
			auto collider = AttackBox->AddComponent<BoxCollider>();
			collider->box.Scale = boxRenderer->scale;

			//�̂̎����������Ɖ�]�w����
			auto attack = AttackBox->AddComponent<ARoundMove>();
			attack->SetTarget(*this);

			//�_���[�W��^���鑊����w��
			auto damage = AttackBox->AddComponent<DamageSource>();
			damage->targetName = "player";
		}
		else if (MoveTimer <= 2 * pi)//�������܂�
		{
			this->AddRotaion(vec3(0, -deltaTime, 0));
		}
		else//���������
		{
			WaitTime = 2.0f;
			ChangeState(Routine::Wait);//Wai�Ɉڍs
		}
		MoveTimer += deltaTime;
		break;
	}

	case Routine::FarAttack://��������Đ�����΂��Ă���
	{

		//�v���C���[�ɑ΂��鐳�ʂƉE�������v�Z
		vec3 Forward = player->GetPos() - this->GetPos();
		Forward = normalize(Forward);
		vec3 Right = cross(Forward, vec3{ 0,1,0 });

		//BoxCount���̔�������ĂԂ�Ȃ���
		for (int i = 0; i < BoxCount; i++)
		{
			static const float Margine = 5.0f;		//���ǂ����̌���
			static const int Center = BoxCount / 2;	//���Ԗڂ����̐^�񒆉��𒲂ׂ�

			GameObjectPtr Box = this->engine->Create<GameObject>("Box");

			auto boxRenderer = Box->AddComponent<MeshRenderer>();
			boxRenderer->mesh = engine->LoadOBJ("Box");
			boxRenderer->scale = vec3(1);
			Box->SetPos(this->GetPos() + Right * (i - Center) * Margine + Forward * 2);//�E���珇�Ԃɐ��������
			Box->AddPosition({ 0,7,0 });	//�ォ��o�Ă��ė~����
			Box->SetMoveFlg(true);			//��������

			//�ʂɐF��ς��邽�߂ɁA�}�e���A�����R�s�[
			boxRenderer->materials.push_back(
				std::make_shared<Mesh::Material>(*boxRenderer->mesh->materials[0]));

			//�R���C�_�[�����蓖�Ă�
			auto collider = Box->AddComponent<BoxCollider>();
			collider->box.Scale = boxRenderer->scale;

			//������ǉ�
			auto bullet = Box->AddComponent<BossCubeBullet>();
			bullet->SetPlayer(player);			//�v���C���[��ݒ�
			bullet->SetLiveTime(3.0 + i * 0.5);	//�������Ԃ̐ݒ�
			bullet->SetMoveTime(i * 0.5);		//�s�����J�n����܂ł̎��Ԃ�ݒ�

			auto damage = Box->AddComponent<DamageSource>();
			damage->targetName = "player";
			damage->isOnce = true;

		}
		//�T�b�҂��Ď��Ɉړ�
		WaitTime = 5.0f;
		ChangeState(Routine::Wait);

		break;
	}
	case Routine::Wait:	//�ݒ肳�ꂽ���ԕ��ҋ@
		MoveTimer += deltaTime;

		if (MoveTimer >= WaitTime)
		{
			ChangeState(Routine::Search);
		}
		break;

	default:

		ChangeState(Routine::Search);
		break;
	}
}

//_state�ɕς��
//�l�X�ȕϐ������Z�b�g����
void Boss::ChangeState(Routine _state)
{
	switch (_state)
	{
	case Routine::Search:
		break;
	case Routine::Think:
		break;
	case Routine::MovePlayerPos:
		targetPos = player->GetPos();
		beforePos = this->GetPos();
		
		MoveTimer = 0;
		TakeTime = 0;
		break;
	case Routine::MoveHomePos:
		break;
	case Routine::NearAttack:
		MoveTimer = 0;
		TakeTime = 0;

		break;
	case Routine::FarAttack:
		break;
	case Routine::Wait:
		MoveTimer = 0;
		break;
	default:
		break;
	}

	State = _state;

}
