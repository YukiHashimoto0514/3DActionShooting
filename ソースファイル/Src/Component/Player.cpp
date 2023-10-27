#include "Player.h"
#include "../Application/GameOverScene.h"
#include "../Component/BulletMove.h"
#include "../Component/MeshRenderer.h"
#include "../Component/Collider.h"
#include "../Component/DamageSource.h"
#include "../Application/EasyAudio.h"
#include "../Engine/AudioSettings.h"

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

	//���G���Ԃ����炷
	if (this->GetImmortalTime() > 0)
	{
		this->DecImmortalTime(deltaTime);
	}
	else
	{
		this->SetImmortalTime(0);
	}

	//~~�G����~~\\
	
	//�ʏ�U���ɂ���
	if (engine->GetKey(GLFW_KEY_1))
	{
		SelectShot(0);
	}

	if (engine->GetKey(GLFW_KEY_2))
	{
		SelectShot(1);
	}

	if (engine->GetKey(GLFW_KEY_3))
	{
		SelectShot(2);
	}


}

void Player::TakeDamage(GameObject& other, const Damage& damage)
{
	//���G���Ԃ��Ȃ��Ȃ�
	if (this->GetImmortalTime() <= 0)
	{
		//�q�b�g�|�C���g�����炷
		this->SetHP(this->GetHP() - damage.amount);

		//���G���Ԃ��Z�b�g
		this->SetImmortalTime(2.0f);

		//�q�b�g�|�C���g�������Ȃ��Ă�����
		if (this->GetHP() <= 0)
		{
			//�t�F�[�h�A�E�g���J�n
			engine->SetFadeRule("Res/Fade/fade_rule4.tga");
			engine->SetFadeColor("Res/Fade/fade_colorR.tga");
			engine->StartFadeOut(2.0f);	//2�b�����ăt�F�[�h����

			//���g�̓����蔻�������
			this->colliderList.clear();
		}
	}
}

void Player::OnCollision(GameObject& object)
{
	if (object.name == "Rock")
	{
		NearFlg = true;
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
			auto bulletCollider = bullet->AddComponent<SphereCollider>();
			bulletCollider->sphere.Center = bulletRenderer->translate;
			bulletCollider->sphere.Radius = std::max({
				bulletRenderer->scale.x,
				bulletRenderer->scale.y,
				bulletRenderer->scale.z });

			//�_���[�W��^���鑊��̐ݒ�
			auto da = bullet->AddComponent<DamageSource>();
			da->targetName = "enemy";
			da->amount = ShotDamage;	//�_���[�W����

		//�ˌ�����炷
			Audio::PlayOneShot(SE::playerShot, 0.2f);

			break;
		}
		case 1://�A�ˑ��x������
		{
			ShotCoolTime = this->ShotInterval - 0.3f;//�ˌ����o�̐ݒ�

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

			bulletRenderer->materials[0]->baseColor = vec4{ 0.2f,0.3f,4.0f,1 };

			//�����蔻��̐ݒ�
			auto bulletCollider = bullet->AddComponent<SphereCollider>();
			bulletCollider->sphere.Center = bulletRenderer->translate;
			bulletCollider->sphere.Radius = std::max({
				bulletRenderer->scale.x,
				bulletRenderer->scale.y,
				bulletRenderer->scale.z });

			//�_���[�W��^���鑊��̐ݒ�
			auto da = bullet->AddComponent<DamageSource>();
			da->targetName = "enemy";
			da->amount = ShotDamage * 0.6f;//�_���[�W����
			da->isOnce = true;

			//�ˌ�����炷
			Audio::PlayOneShot(SE::playerShot, 0.2f);

			break;
		}
		case 2://�V���b�g����I��
		{
			ShotCoolTime = this->ShotInterval + 0.5f;//�ˌ����o�̐ݒ�

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
				bulletRenderer->materials[0]->baseColor = vec4{ 3,2,0.2f,1 };

				//�����蔻��̐ݒ�
				auto bulletCollider = bullet->AddComponent<SphereCollider>();
				bulletCollider->sphere.Center = bulletRenderer->translate;
				bulletCollider->sphere.Radius = std::max({
					bulletRenderer->scale.x,
					bulletRenderer->scale.y,
					bulletRenderer->scale.z });

				//�_���[�W��^���鑊��̐ݒ�
				auto da = bullet->AddComponent<DamageSource>();
				da->targetName = "enemy";
				da->amount = ShotDamage * 0.5f;	//�_���[�W�𒲐�
				da->isOnce = true;


			}

			//�ˌ�����炷
			Audio::PlayOneShot(SE::ShotGun, 0.5f);
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
		this->AddPosition(vec3(0, -0.06f - FallTimer * FallTimer * 9.8f * deltaTime, 0));

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

void Player::SelectShot(int _shot)
{

	//�I�����Ă��镐��Ɠ����Ȃ疳��
	if (ShotStyle == _shot)
	{
		return;
	}

	//�����ꂽ���킪�g���邩�ǂ������m�F
	switch (_shot)
	{
	case 0:
		break;
	case 1:
		if (!ShooterFlg)return;
		break;

	case 2:
		if (!ShotGunFlg)return;

		break;

	default:
		break;
	}

	//���ʉ���炷
	Audio::PlayOneShot(SE::WeaponSelect, 0.2f);

	//�����؂�ւ���
	ShotStyle = _shot;

}

