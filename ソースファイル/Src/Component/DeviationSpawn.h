//@file DeviationSpawn.h
#ifndef DEVIATIONSPAWN_H_INCLUDED
#define DEVIATIONSPAWN_H_INCLUDED

#include "../Engine/Component.h"
#include "../Engine/VecMath.h"
#include "../Engine/Debug.h"
#include "../Engine/Engine.h"
#include "../Component/MeshRenderer.h"
#include "../Application/GameObject.h"
#include "../Application/DeviationEnemy.h"
#include "../Component/Health.h"
#include "../Component/HitEffect.h"
#include "../Component/Explosion.h"
#include "../Component/DropPowerUp.h"

using namespace VecMath;

//�G�𐶐�����R���|�[�l���g
class DeviationSpawn :public Component
{
public:
	DeviationSpawn() = default;				//�R���X�g���N�^
	virtual ~DeviationSpawn() = default;	//�f�X�g���N�^

	virtual void Start(GameObject& gameObject)override
	{
		//�G�̃X�|�[���|�C���g���i�[
		for (int i = 0; i < SpownCount; ++i)
		{
			std::string No = std::to_string(i);
			std::string Name = "SpawnPoint";	//���O
			Name += No;

			//�X�|�\���I�u�W�F���擾
			auto obj = gameObject.engine->SearchGameObject(Name);
			EnemySpawnPos[i] = obj->GetPos();

		}
	}

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{
		
		//�^�C�}�[�����炷
		CreateTimer -= deltaTime;

		//���Ԃ�������
		if (CreateTimer <= 0)
		{
			auto ene = gameObject.engine->Create<DeviationEnemy>("enemy");
			ene->SetTarget(PlayerObj);

			//��P�̐ݒ�
			const float hp = std::uniform_real_distribution<float>(2, 4)(gameObject.engine->GetRandomGenerator());//2~4
			ene->SetHP(hp);

			//���f���ǂݍ���
			auto eneRenderer = ene->AddComponent<MeshRenderer>();
			
			eneRenderer->mesh = gameObject.engine->LoadOBJ("Res/Model/Drone_01.obj");
			eneRenderer->scale = vec3(1);


			//�X�|�[���ʒu
			const int rnd = std::uniform_int_distribution<>(0, SpownCount - 1)(gameObject.engine->GetRandomGenerator());//0~SpownCount-1
			ene->SetPos(vec3(EnemySpawnPos[rnd]));

			//�ʂɐF��ς��邽�߂ɁA�}�e���A�����R�s�[
			for (const auto& e : eneRenderer->mesh->materials) {
				eneRenderer->materials.push_back(
					std::make_shared<Mesh::Material>(*e));
			}

			//�{�̂̐F��ς���
			const float r = std::uniform_real_distribution<float>(0, 1)(gameObject.engine->GetRandomGenerator());//0~1
			const float g = std::uniform_real_distribution<float>(0, 1)(gameObject.engine->GetRandomGenerator());//0~1
			const float b = std::uniform_real_distribution<float>(0, 1)(gameObject.engine->GetRandomGenerator());//0~1
			eneRenderer->materials[0]->baseColor = vec4(r, g, b, 1);


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
			auto dp = ene->AddComponent<DropPowerUp>();	//�A�C�e��
			dp->target = PlayerObj;

			CreateTimer = std::uniform_real_distribution<float>(5, 30)(gameObject.engine->GetRandomGenerator());//30~0
		}
	}

	void SetPlayer(PlayerPtr player)
	{
		PlayerObj = player;
	}

private:
	float CreateTimer = 5;	//�������鎞��
	PlayerPtr PlayerObj;	//�v���C���[

	const static int SpownCount = 2;	//�X�|�[���̐�
	vec3 EnemySpawnPos[SpownCount];		//�X�|�[���|�C���g�̍��W

};




#endif //DEVIATIONSPAWN_H_INCLUDED