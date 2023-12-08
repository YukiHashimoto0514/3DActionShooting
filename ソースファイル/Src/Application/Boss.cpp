#include "Boss.h"
#include "../Component/Lerp.h"
#include "../Component/MeshRenderer.h"
#include "../Component/Collider.h"
#include "../Component/BulletMove.h"
#include "../Component/BossCubeBullet.h"
#include "../Component/DamageSource.h"
#include "../Component/ARoundMove.h"
#include "../Application/GameClearScene.h"
#include "../Component/ParticleSystem.h"
#include "../Application/EasyAudio.h"
#include "../Engine/AudioSettings.h"
#include "../Application/DeviationEnemy.h"
#include "../Application/JumpingEnemy.h"
#include "../Component/Health.h"
#include "../Component/Explosion.h"
#include "../Component/HitEffect.h"
#include "../Component/DropPowerUp.h"

void Boss::Update(float deltaTime)
{
	//HP�������Ȃ�����
	if (this->GetHP() <= 0)
	{
		//���񂾎��Ԃ��v��
		DeadTimer += deltaTime;

		player->SetImmortalTime(10);

		//�t�F�[�h������������
		if (engine->GetFadeState() == Engine::FadeState::Closed)
		{
			//�N���A�I�I
			engine->SetNextScene<GameClearScene>();
		}
	}

	//�t�F�[�h���͂������牺�͏������Ȃ�
	if (engine->GetFadeState() == Engine::FadeState::Fading)
	{
		return;
	}

	//����ł���2�b��������
	if (DeadTimer >= 2.0f)
	{
		//�t�F�[�h�A�E�g���J�n
		engine->SetFadeRule("Res/Fade/fade_rule2.tga");
		engine->SetFadeColor("Res/UI/Boarder.tga");
		engine->StartFadeOut();
	}
	BattleTime += deltaTime;
	RoutineUpdate(State, deltaTime);
}

void Boss::TakeDamage(GameObject& other, const Damage& damage)
{
	//�q�b�g�|�C���g�����炷
	this->SetHP(this->GetHP() - damage.amount);

	//���݂�HP�������ȉ��Ȃ�
	if (MaxHP * 0.5f >= this->GetHP())
	{
		auto& a = static_cast<MeshRenderer&>(*this->componentList[0]);
		a.materials[0]->baseColor = vec4(1, 0.2f, 0.2f, 1);


		//�U������e�̐��𑝂₷
		BoxCount = 7;
		BoxSize = 20.0f;
	}

	//�q�b�g�|�C���g�������Ȃ��Ă�����
	if (this->GetHP() <= 0)
	{
		//�p�[�e�B�N���ɂ�锚��
		auto ParticleObject = this->engine->Create<GameObject>(
			"particle explosion", this->GetPos());

		auto ps = ParticleObject->AddComponent<ParticleSystem>();
		ps->Emitters.resize(1);
		auto& emitter = ps->Emitters[0];
		emitter.ep.ImagePath = "Res/UI/exp.tga";
		emitter.ep.Duration = 0.1f;				//���o����
		emitter.ep.RandomizeSize = 1;			//�傫���������_����
		emitter.ep.RandomizeRotation = 1;		//�p�x������
		emitter.ep.RandomizeSize = 1;
		emitter.ep.EmissionsPerSecond = 30;		//�b�ԕ��o��
		emitter.pp.LifeTime = 3.0f;				//��������
		emitter.pp.color.Set({ 5, 5, 0.5f, 1 }, { 1, 2, 1.5f, 0 });	//�F�t��
		emitter.pp.scale.Set({ 0.06f,0.06f }, { 0.005f,0.005f });	//�T�C�Y�����X�ɂ֕ύX������
		emitter.pp.radial.Set(50, -20);
		emitter.ep.Loop = true;

		Audio::PlayOneShot(SE::BossExplosion, 0.3f);

		return;
	}

	//�G�t�F�N�g���o��
	CreateHitEffect(damage.amount);
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


		//�߁E�������U���Ɉڍs���鋗��
		const static float MaxDistance = 30;
		const static float MinDistance = 15;

		//MaxDistance�ȏ㗣��Ă�����
		if (Dis >= MaxDistance)
		{
			//�ő�HP�̔����ȏ�Ȃ�
			if (this->GetHP() >= MaxHP * 0.5f)
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
			
			if (this->GetHP() >= MaxHP * 0.5f)
			{
				//�������U��������
				ChangeState(Routine::FarAttack);
			}
			else
			{
				//�����𐶐�
				ChangeState(Routine::EnemyCreate);
			}
		}
		else
		{
			if (this->GetHP() >= MaxHP * 0.5f)
			{
				//�ߋ����U��
				ChangeState(Routine::NearAttack);
			}
			else
			{
				//�����𐶐�
				ChangeState(Routine::EnemyCreate);
			}
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
	case Routine::MoveRndPos:
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
			const float move = static_cast<float>(pow(MoveTimer / 3, 5));

			this->SetPos(vec3{ this->GetPos().x,30 + move * (targetPos.y - 30),this->GetPos().z });
		}
		else
		{
			ChangeState(Routine::Search);
		}
		break;
	}


	case Routine::NearAttack://�_������Ĉ��]
	{
		if (MoveTimer == 0)
		{
			GameObjectPtr AttackBox = this->engine->Create<GameObject>("Box");

			auto boxRenderer = AttackBox->AddComponent<MeshRenderer>();
			boxRenderer->mesh = engine->LoadOBJ("Box");
			boxRenderer->scale = vec3({ BoxSize,1.0f,1.0f });	//����
			AttackBox->SetPos(this->GetPos());
			AttackBox->AddPosition({ 0,10,0 });		//��������ďo��
			AttackBox->SetMoveFlg(false);			//��������

			//�ʂɐF��ς��邽�߂ɁA�}�e���A�����R�s�[
			boxRenderer->materials.push_back(
				std::make_shared<Mesh::Material>(*boxRenderer->mesh->materials[0]));

			boxRenderer->materials[0]->baseColor = vec4{ 3,1,0.2f,1 };
			boxRenderer->materials[0]->receiveShadows = false;

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
			static const float Center = BoxCount / 2.0f;	//���Ԗڂ����̐^�񒆉��𒲂ׂ�

			GameObjectPtr Box = this->engine->Create<GameObject>("Box");

			auto boxRenderer = Box->AddComponent<MeshRenderer>();
			boxRenderer->mesh = engine->LoadOBJ("Box");
			boxRenderer->scale = vec3(1);
			Box->SetPos(this->GetPos() + Right * (i - Center) * Margine + Forward * 2.0f);//�E���珇�Ԃɐ��������
			Box->AddPosition({ 0,7,0 });	//�ォ��o�Ă��ė~����
			Box->SetMoveFlg(true);			//��������

			//�ʂɐF��ς��邽�߂ɁA�}�e���A�����R�s�[
			boxRenderer->materials.push_back(
				std::make_shared<Mesh::Material>(*boxRenderer->mesh->materials[0]));

			boxRenderer->materials[0]->baseColor = vec4{ 3,1,0.2f,1 };
			boxRenderer->materials[0]->receiveShadows = false;
			//�R���C�_�[�����蓖�Ă�
			auto collider = Box->AddComponent<BoxCollider>();
			collider->box.Scale = boxRenderer->scale;

			//������ǉ�
			auto bullet = Box->AddComponent<BossCubeBullet>();
			bullet->SetPlayer(player);			//�v���C���[��ݒ�
			bullet->SetLiveTime(3.0f + i * 0.5f);	//�������Ԃ̐ݒ�
			bullet->SetMoveTime(i * 0.5f);		//�s�����J�n����܂ł̎��Ԃ�ݒ�

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

	case Routine::EnemyCreate:	//�G�𐶐�����
	{
		//���ʂ��v�Z
		vec3 Forward = player->GetPos() - this->GetPos();
		Forward = normalize(Forward);

		//�E�����v�Z
		vec3 Right = cross(Forward, vec3{ 0,1,0 });

		static const float Margine = 5.0f;		//����

		for (int i = 0; i < 5; ++i)
		{
			auto ene = engine->Create<DeviationEnemy>("enemy");
			ene->SetTarget(player);

			//��P�̐ݒ�
			const float hp = std::uniform_real_distribution<float>(1, 3)(engine->GetRandomGenerator());//1~3
			ene->SetHP(hp);

			//���f���ǂݍ���
			auto eneRenderer = ene->AddComponent<MeshRenderer>();
			eneRenderer->mesh = engine->LoadOBJ("Res/Model/Drone_01.obj");
			eneRenderer->scale = vec3(1);

			//�X�|�[���ʒu
			ene->SetPos(this->GetPos() + Right * (i - 2.0f) * Margine + Forward * 5.0f);

			//�ʂɐF��ς��邽�߂ɁA�}�e���A�����R�s�[
			for (const auto& e : eneRenderer->mesh->materials) {
				eneRenderer->materials.push_back(
					std::make_shared<Mesh::Material>(*e));
			}

			//�{�̂̐F��ς���
			eneRenderer->materials[0]->baseColor = vec4(0.1f, 0.1f, 5.0f, 1.0f);


			//�R���C�_�[�����蓖�Ă�
			auto enecollider = ene->AddComponent<SphereCollider>();
			enecollider->sphere.Center = eneRenderer->translate;
			enecollider->sphere.Radius = std::max({
				eneRenderer->scale.x,
				eneRenderer->scale.y,
				eneRenderer->scale.z });


			auto he = ene->AddComponent<HitEffect>();	//�G�t�F�N�g
			auto hh = ene->AddComponent<Health>();		//HP
			auto ee = ene->AddComponent<Explosion>();	//����

		}

		//�����_���Ȉʒu�Ɉړ�
		WaitTime = 2.0f;
		ChangeState(Routine::MoveRndPos);

		break;
	}

	case Routine::JumpAttack:
	{
		//�W�����v������
		const float Gravity = 9.8f;

		if (JumpTimer == 0)
		{
			//���������߂�
			const float dx = player->GetPos().x - this->GetPos().x;
			const float dz = player->GetPos().z - this->GetPos().z;
			const float dis = sqrt(dx * dx + dz * dz);

			//�������擾
			Direction = player->GetPos() - this->GetPos();
			Direction = normalize(Direction);

			//���x���v�Z
			float vel = sqrt(2 * 2 * dis);

			velosity = sqrt(dis * Gravity / sin(radians(60)));
			//���������ɑ��x���|����
			Direction *= velosity * deltaTime;

			LOG("%f  %f,%f", velosity, Direction.x,Direction.z);
		}

		float UpPower = velosity;
		float DownPower = Gravity * JumpTimer * JumpTimer;
		float FallPower = (UpPower - DownPower) * deltaTime;

		this->AddPosition(vec3(Direction.x, FallPower, Direction.z));
		if (this->GetAir())
		{
			JumpTimer += deltaTime;
		}
		else
		{
			if (JumpTimer > 0)
			{
				JumpTimer = 0;
				WaitTime = 1.0f;
				ChangeState(Routine::Wait);

				break;
			}
		}

		break;
	}
	default:

		ChangeState(Routine::Search);
		break;
	}
}

//_state�ɕς��
//�l�X�ȕϐ������Z�b�g����
void Boss::ChangeState(Routine _state)
{

	if (old == _state)
	{
		Same++;
	}

	if (Same >= 3)
	{
		_state = Routine::JumpAttack;
		Same = 0;
		old = Routine::None;
	}
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
	case Routine::MoveRndPos:
	{		
		vec3 pos= player->GetPos();
		pos.x *= -1;
		pos.z *= -1;

		//�v���C���[�̌��Ɉړ�
		targetPos = vec3{ pos };
		beforePos = this->GetPos();

		MoveTimer = 0;
		TakeTime = 0;
	}
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

	case Routine::EnemyCreate:
		break;

	case Routine::JumpAttack:
		targetPos = player->GetPos();
		beforePos = this->GetPos();
		MoveTimer = 0;
		TakeTime = 0;

		break;
	default:
		break;
	}

	if (_state == Routine::MovePlayerPos || _state == Routine::FarAttack || _state == Routine::NearAttack)
	{
		old = _state;
	}
	//���̃X�e�[�g�ɂ���
	State = _state;
	
}

void Boss::CreateHitEffect(float _count)
{

	const float Time = std::clamp(_count, 1.0f, 3.0f);

	//�q�b�g�G�t�F�N�g�̐���
	auto ParticleObject = this->engine->Create<GameObject>(
		"particle explosion", this->GetPos());

	auto ps = ParticleObject->AddComponent<ParticleSystem>();
	ps->Emitters.resize(1);
	auto& emitter = ps->Emitters[0];
	emitter.ep.ImagePath = "Res/UI/hit.tga";
	emitter.ep.tiles = { 3,2 };				//�摜����
	emitter.ep.Duration = Time * 0.1f;	//���o����
	emitter.ep.RandomizeSize = 1;			//�傫���������_����
	emitter.ep.RandomizeRotation = 1;		//�p�x������
	emitter.ep.EmissionsPerSecond = 10;		//�b�ԕ��o��
	emitter.pp.LifeTime = 0.3f;				//��������
	emitter.pp.color.Set({ 5, 1, 0.5f, 1 }, { 1, 2, 1.5f, 0 });	//�F�t��
	emitter.pp.scale.Set({ 0.05f,0.05f }, { 0.005f,0.005f });	//�T�C�Y�����X�ɂ֕ύX������
	emitter.pp.velocity.Set({ 0,1,0 }, { 0,-10,0 });//������ɕ��o

}

