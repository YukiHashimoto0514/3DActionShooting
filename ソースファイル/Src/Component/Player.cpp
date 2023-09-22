#include "Player.h"
#include "../Application/GameOverScene.h"
#include "../Component/BulletMove.h"
#include "../Component/MeshRenderer.h"
#include "../Component/Collider.h"
#include "../Component/DamageSource.h"
//
//void Player::Start()
//{
//	LOG("doumo");
//}

void Player::Update(float deltaTime)
{
	//�v���C���[������ł�����
	if (this->GetHP() <= 0)
	{
		//�t�F�[�h������������
		if (engine->GetFadeState() == Engine::FadeState::Closed)
		{
			engine->SetNextScene<GameOverScene>();
		}
		return;
	}
	Jump(deltaTime);

	Move(deltaTime);

	Shot(deltaTime);

	Fall(deltaTime);

	//�ʏ�U���ɂ���
	if (engine->GetKey(GLFW_KEY_1))
	{
		ShotStyle = 1;
	}

	//�V���[�^�[���g����悤�ɂȂ��Ă�����
	if (ShooterFlg)
	{
		if (engine->GetKey(GLFW_KEY_2))
		{
			ShotStyle = 1;
		}
	}

	//�V���b�g�K�����g����悤�ɂȂ��Ă�����
	if (ShotGunFlg)
	{
		if (engine->GetKey(GLFW_KEY_3))
		{
			ShotStyle = 2;
		}
	}


}

void Player::TakeDamage(GameObject& other, const Damage& damage)
{

	//�q�b�g�|�C���g�����炷
	this->SetHP(this->GetHP() - damage.amount);

	//�q�b�g�|�C���g�������Ȃ��Ă�����
	if (this->GetHP() <= 0)
	{
		//�t�F�[�h�A�E�g���J�n
		engine->SetFadeRule("Res/fade_rule4.tga");
		engine->SetFadeColor("Res/fade_colorR.tga");
		engine->StartFadeOut(2.0f);	//2�b�����ăt�F�[�h����
		
		//���g�̓����蔻�������
		this->colliderList.clear();
	}
}

void Player::Jump(float deltaTime)
{
	if (!this->GetJump())
	{
		//������O���ƁA�ǃW�������ł���悤�ɂȂ�
		if (!this->GetAir())
		{
			if (engine->GetKey(GLFW_KEY_SPACE))
			{

				this->v0 = 10;
				this->SetJumpFlg(true);
			}
		}
	}
	else
	{
		//�󒆂ŃW�����v�{�^������������
		if (engine->GetKey(GLFW_KEY_SPACE))
		{
			JumpTimer -= deltaTime;
		}
	}

}

void Player::Move(float deltaTime)
{
	//���ʕ������擾
	vec3 forward = this->GetForward();
	forward.y = 0;

	//�E�������擾
	vec3 right = this->GetRight();
	right.y = 0;

	static float speed = 0;

	if (!this->GetJump())
	{
		speed = MoveSpeed;
	}
	else
	{
		speed = MoveSpeed * 0.7f;
	}


	//�L�[���͂ňړ�������
	if (engine->GetKey(GLFW_KEY_W))
	{
		this->AddPosition(forward * speed * deltaTime);
	}
	if (engine->GetKey(GLFW_KEY_S))
	{
		this->AddPosition(-forward * speed * deltaTime);
	}
	if (engine->GetKey(GLFW_KEY_D))
	{
		this->AddPosition(right * speed * deltaTime);
	}
	if (engine->GetKey(GLFW_KEY_A))
	{
		this->AddPosition(-right * speed * deltaTime);
	}
}

void Player::Shot(float deltaTime)
{
	//�ˌ��N�[���^�C�������炷
	ShotCoolTime -= deltaTime;

	//�}�E�X�̍��N���b�N
	int state = glfwGetMouseButton(engine->GetWindow(), GLFW_MOUSE_BUTTON_LEFT);
	if (state == GLFW_PRESS)
	{
		//�ˌ��N�[���^�C����0���傫���Ȃ�
		if (ShotCoolTime > 0)
		{
			//��������
			return;
		}

		switch (ShotStyle)
		{
		case 0://�ꔭ���d�����(�˒�������)
		{
			ShotCoolTime = this->ShotInterval;//�ˌ����o�̐ݒ�

			//�e�̐���
			auto bullet = engine->Create<GameObject>("bullet");
			auto move = bullet->AddComponent<BulletMove>();
			move->SetPlayerinfo(this, *bullet);

			//�����ڂ̐ݒ�
			auto bulletRenderer = bullet->AddComponent<MeshRenderer>();
			bulletRenderer->mesh = engine->LoadOBJ("Sphere");
			bulletRenderer->scale = vec3(0.5f);

			//�ʂɐF��ύX�ł���悤�ɁA�}�e���A���̃R�s�[���쐬
			bulletRenderer->materials.push_back(
				std::make_shared<Mesh::Material>(*bulletRenderer->mesh->materials[0]));

			//�����蔻��̐ݒ�
			auto bulletcollider = bullet->AddComponent<SphereCollider>();
			bulletcollider->sphere.Center = bulletRenderer->translate;
			bulletcollider->sphere.Radius = std::max({
				bulletRenderer->scale.x,
				bulletRenderer->scale.y,
				bulletRenderer->scale.z });

			//�_���[�W��^���鑊��̐ݒ�
			auto da = bullet->AddComponent<DamageSource>();
			da->targetName = "enemy";
			da->amount = ShotDamage;
			da->isOnce = true;

			break;
		}
		case 1://�A�ˑ��x������
		{
			ShotCoolTime = this->ShotInterval - 1.5f;//�ˌ����o�̐ݒ�

			//�e�̐���
			auto bullet = engine->Create<GameObject>("bullet");
			auto move = bullet->AddComponent<BulletMove>();
			move->SetPlayerinfo(this, *bullet);
			move->SetRandom(true);
			//�����ڂ̐ݒ�
			auto bulletRenderer = bullet->AddComponent<MeshRenderer>();
			bulletRenderer->mesh = engine->LoadOBJ("Sphere");
			bulletRenderer->scale = vec3(0.5f);

			//�ʂɐF��ύX�ł���悤�ɁA�}�e���A���̃R�s�[���쐬
			bulletRenderer->materials.push_back(
				std::make_shared<Mesh::Material>(*bulletRenderer->mesh->materials[0]));

			//�����蔻��̐ݒ�
			auto bulletcollider = bullet->AddComponent<SphereCollider>();
			bulletcollider->sphere.Center = bulletRenderer->translate;
			bulletcollider->sphere.Radius = std::max({
				bulletRenderer->scale.x,
				bulletRenderer->scale.y,
				bulletRenderer->scale.z });

			//�_���[�W��^���鑊��̐ݒ�
			auto da = bullet->AddComponent<DamageSource>();
			da->targetName = "enemy";
			da->amount = ShotDamage;
			da->isOnce = true;

			break;
		}
		case 2://�V���b�g����I��
		{
			ShotCoolTime = this->ShotInterval;//�ˌ����o�̐ݒ�

			for (int i = 0; i < 10; ++i)
			{
				//�e�̐���
				auto bullet = engine->Create<GameObject>("bullet");
				auto move = bullet->AddComponent<BulletMove>();
				move->SetPlayerinfo(this, *bullet);
				move->SetRandom(true);

				//�����ڂ̐ݒ�
				auto bulletRenderer = bullet->AddComponent<MeshRenderer>();
				bulletRenderer->mesh = engine->LoadOBJ("Sphere");
				bulletRenderer->scale = vec3(0.5f);

				//�ʂɐF��ύX�ł���悤�ɁA�}�e���A���̃R�s�[���쐬
				bulletRenderer->materials.push_back(
					std::make_shared<Mesh::Material>(*bulletRenderer->mesh->materials[0]));

				//�����蔻��̐ݒ�
				auto bulletcollider = bullet->AddComponent<SphereCollider>();
				bulletcollider->sphere.Center = bulletRenderer->translate;
				bulletcollider->sphere.Radius = std::max({
					bulletRenderer->scale.x,
					bulletRenderer->scale.y,
					bulletRenderer->scale.z });

				//�_���[�W��^���鑊��̐ݒ�
				auto da = bullet->AddComponent<DamageSource>();
				da->targetName = "enemy";
				da->amount = ShotDamage;
				da->isOnce = true;
			}
			break;
		}
		default:
			break;

		}
	}

}

void Player::Fall(float deltaTime)
{
	//�W�����v������Ȃ��Ȃ�(���R����)
	if (!this->GetJump())
	{
		this->AddPosition(vec3(0, -0.05f - FallTimer * FallTimer * 9.8f * deltaTime, 0));

		this->v0 = 0;
		JumpTimer = 0;
	}
	else //�W�����v�ɂ�闎��
	{
		JumpTimer += deltaTime * 2;

		float UpPower = this->v0;

		float DownPower = Gravity * JumpTimer * JumpTimer;

		float FallPower = (UpPower - DownPower) * deltaTime;
		this->AddPosition(vec3(0, FallPower, 0));

	}

	//�󒆂Ȃ�
	if (this->GetAir())
	{
		FallTimer += deltaTime;
	}
	//�󒆂���Ȃ��Ȃ�
	else
	{
		FallTimer = 0;
		JumpTimer = 0;
	}

}

