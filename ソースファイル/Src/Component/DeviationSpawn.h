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

//敵を生成するコンポーネント
class DeviationSpawn :public Component
{
public:
	DeviationSpawn() = default;				//コンストラクタ
	virtual ~DeviationSpawn() = default;	//デストラクタ

	virtual void Start(GameObject& gameObject)override
	{
		//敵のスポーンポイントを格納
		for (int i = 0; i < SpownCount; ++i)
		{
			std::string No = std::to_string(i);
			std::string Name = "SpawnPoint";	//名前
			Name += No;

			//スポ―ンオブジェを取得
			auto obj = gameObject.engine->SearchGameObject(Name);
			EnemySpawnPos[i] = obj->GetPos();

		}
	}

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{
		
		//タイマーを減らす
		CreateTimer -= deltaTime;

		//時間がきたら
		if (CreateTimer <= 0)
		{
			auto ene = gameObject.engine->Create<DeviationEnemy>("enemy");
			ene->SetTarget(PlayerObj);

			//ｈPの設定
			const float hp = std::uniform_real_distribution<float>(2, 4)(gameObject.engine->GetRandomGenerator());//2~4
			ene->SetHP(hp);

			//モデル読み込み
			auto eneRenderer = ene->AddComponent<MeshRenderer>();
			
			eneRenderer->mesh = gameObject.engine->LoadOBJ("Res/Model/Drone_01.obj");
			eneRenderer->scale = vec3(1);


			//スポーン位置
			const int rnd = std::uniform_int_distribution<>(0, SpownCount - 1)(gameObject.engine->GetRandomGenerator());//0~SpownCount-1
			ene->SetPos(vec3(EnemySpawnPos[rnd]));

			//個別に色を変えるために、マテリアルをコピー
			for (const auto& e : eneRenderer->mesh->materials) {
				eneRenderer->materials.push_back(
					std::make_shared<Mesh::Material>(*e));
			}

			//本体の色を変える
			const float r = std::uniform_real_distribution<float>(0, 1)(gameObject.engine->GetRandomGenerator());//0~1
			const float g = std::uniform_real_distribution<float>(0, 1)(gameObject.engine->GetRandomGenerator());//0~1
			const float b = std::uniform_real_distribution<float>(0, 1)(gameObject.engine->GetRandomGenerator());//0~1
			eneRenderer->materials[0]->baseColor = vec4(r, g, b, 1);


			//コライダーを割り当てる
			auto enecollider = ene->AddComponent<SphereCollider>();
			enecollider->sphere.Center = eneRenderer->translate;
			enecollider->sphere.Radius = std::max({
				eneRenderer->scale.x,
				eneRenderer->scale.y,
				eneRenderer->scale.z });


			auto he = ene->AddComponent<HitEffect>();	//エフェクト
			auto hh = ene->AddComponent<Health>();		//HP
			auto ee = ene->AddComponent<Explosion>();	//爆発
			auto dp = ene->AddComponent<DropPowerUp>();	//アイテム
			dp->target = PlayerObj;

			CreateTimer = std::uniform_real_distribution<float>(5, 30)(gameObject.engine->GetRandomGenerator());//30~0
		}
	}

	void SetPlayer(PlayerPtr player)
	{
		PlayerObj = player;
	}

private:
	float CreateTimer = 5;	//生成する時間
	PlayerPtr PlayerObj;	//プレイヤー

	const static int SpownCount = 2;	//スポーンの数
	vec3 EnemySpawnPos[SpownCount];		//スポーンポイントの座標

};




#endif //DEVIATIONSPAWN_H_INCLUDED