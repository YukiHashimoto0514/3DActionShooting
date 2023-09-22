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
	//HPが無くなったら
	if (this->GetHP() <= 0)
	{
		//フェードが完了したら
		if (engine->GetFadeState() == Engine::FadeState::Closed)
		{
			//クリア！！
			engine->SetNextScene<GameClearScene>();
		}
		return;
	}

	RoutineUpdate(State, deltaTime);
}

void Boss::TakeDamage(GameObject& other, const Damage& damage)
{
	//ヒットポイントを減らす
	this->SetHP(this->GetHP() - damage.amount);

	//ヒットポイントが無くなっていたら
	if (this->GetHP() <= 0)
	{
		//フェードアウトを開始
		engine->StartFadeOut();
	}

}


//stateのアップデートを各々処理
void Boss::RoutineUpdate(Routine _state,float deltaTime)
{
	switch (_state)
	{
	case Routine::Search:
	{
		if (player == NULL)
		{
			//プレイヤーオブジェクトを検索して取得
			player = engine->SearchGameObject("player");
		}

		ChangeState(Routine::Think);
		break;
	}
	case Routine::Think://距離やHPを考慮して行動を決める
	{
		//プレイヤーと距離を計算
		const float dx = player->GetPos().x - this->GetPos().x;
		const float dz = player->GetPos().z - this->GetPos().z;

		//プレイヤーとの距離
		const float Dis = sqrt(dx * dx + dz * dz);


		//近距離攻撃に移行する距離
		const static float MaxDistance = 30;
		const static float MinDistance = 15;

		//MaxDistance以上離れていたら
		if (Dis >= MaxDistance)
		{
			//最大HPの半分以上なら
			if (this->GetHP() >= MaxHP / 2)
			{
				//近づく
				ChangeState(Routine::MovePlayerPos);
			}
			else//半分未満なら
			{
				//遠距離攻撃をする
				ChangeState(Routine::FarAttack);
			}
		}
		else if(Dis >= MinDistance)
		{
			//遠距離攻撃をする
			ChangeState(Routine::FarAttack);
		}
		else
		{
			//近距離攻撃
			ChangeState(Routine::NearAttack);
		}
		break;
	}
	case Routine::MovePlayerPos://上から襲ってくる
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

	case Routine::NearAttack://棒を作って一回転
	{
		if (MoveTimer == 0)
		{
			GameObjectPtr AttackBox = this->engine->Create<GameObject>("Box");

			auto boxRenderer = AttackBox->AddComponent<MeshRenderer>();
			boxRenderer->mesh = engine->LoadOBJ("Box");
			boxRenderer->scale = vec3({ 10,1,1 });	//横長
			AttackBox->SetPos(this->GetPos());
			AttackBox->AddPosition({ 0,10,0 });		//少しずれて出現
			AttackBox->SetMoveFlg(true);			//動く物体

			//個別に色を変えるために、マテリアルをコピー
			boxRenderer->materials.push_back(
				std::make_shared<Mesh::Material>(*boxRenderer->mesh->materials[0]));

			//コライダーを割り当てる
			auto collider = AttackBox->AddComponent<BoxCollider>();
			collider->box.Scale = boxRenderer->scale;

			//体の周りをぐるっと回転指せる
			auto attack = AttackBox->AddComponent<ARoundMove>();
			attack->SetTarget(*this);

			//ダメージを与える相手を指定
			auto damage = AttackBox->AddComponent<DamageSource>();
			damage->targetName = "player";
		}
		else if (MoveTimer <= 2 * pi)//一周するまで
		{
			this->AddRotaion(vec3(0, -deltaTime, 0));
		}
		else//一周したら
		{
			WaitTime = 2.0f;
			ChangeState(Routine::Wait);//Waiに移行
		}
		MoveTimer += deltaTime;
		break;
	}

	case Routine::FarAttack://箱を作って吹っ飛ばしてくる
	{

		//プレイヤーに対する正面と右向きを計算
		vec3 Forward = player->GetPos() - this->GetPos();
		Forward = normalize(Forward);
		vec3 Right = cross(Forward, vec3{ 0,1,0 });

		//BoxCount分の箱を作ってぶんなげる
		for (int i = 0; i < BoxCount; i++)
		{
			static const float Margine = 5.0f;		//箱どおしの隙間
			static const int Center = BoxCount / 2;	//何番目が箱の真ん中化を調べる

			GameObjectPtr Box = this->engine->Create<GameObject>("Box");

			auto boxRenderer = Box->AddComponent<MeshRenderer>();
			boxRenderer->mesh = engine->LoadOBJ("Box");
			boxRenderer->scale = vec3(1);
			Box->SetPos(this->GetPos() + Right * (i - Center) * Margine + Forward * 2);//右から順番に生成される
			Box->AddPosition({ 0,7,0 });	//上から出てきて欲しい
			Box->SetMoveFlg(true);			//動く物体

			//個別に色を変えるために、マテリアルをコピー
			boxRenderer->materials.push_back(
				std::make_shared<Mesh::Material>(*boxRenderer->mesh->materials[0]));

			//コライダーを割り当てる
			auto collider = Box->AddComponent<BoxCollider>();
			collider->box.Scale = boxRenderer->scale;

			//動きを追加
			auto bullet = Box->AddComponent<BossCubeBullet>();
			bullet->SetPlayer(player);			//プレイヤーを設定
			bullet->SetLiveTime(3.0 + i * 0.5);	//生存時間の設定
			bullet->SetMoveTime(i * 0.5);		//行動を開始するまでの時間を設定

			auto damage = Box->AddComponent<DamageSource>();
			damage->targetName = "player";
			damage->isOnce = true;

		}
		//５秒待って次に移動
		WaitTime = 5.0f;
		ChangeState(Routine::Wait);

		break;
	}
	case Routine::Wait:	//設定された時間分待機
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

//_stateに変わる
//様々な変数をリセットする
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
