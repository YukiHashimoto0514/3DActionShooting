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

void Boss::Update(float deltaTime)
{
	//HPが無くなったら
	if (this->GetHP() <= 0)
	{
		//死んだ時間を計測
		DeadTimer += deltaTime;

		player->SetImmortalTime(10);

		//フェードが完了したら
		if (engine->GetFadeState() == Engine::FadeState::Closed)
		{
			//クリア！！
			engine->SetNextScene<GameClearScene>();
		}
	}

	//フェード中はここから下は処理しない
	if (engine->GetFadeState() == Engine::FadeState::Fading)
	{
		return;
	}

	//死んでから2秒たったら
	if (DeadTimer >= 2.0f)
	{
		//フェードアウトを開始
		engine->SetFadeRule("Res/Fade/fade_rule2.tga");
		engine->SetFadeColor("Res/UI/Boarder.tga");
		engine->StartFadeOut();
	}

	RoutineUpdate(State, deltaTime);
}

void Boss::TakeDamage(GameObject& other, const Damage& damage)
{
	//ヒットポイントを減らす
	this->SetHP(this->GetHP() - damage.amount);

	//現在のHPが半分以下なら
	if (MaxHP * 0.5f >= this->GetHP())
	{
		auto& a = static_cast<MeshRenderer&>(*this->componentList[0]);
		a.materials[0]->baseColor = vec4(1, 0.2f, 0.2f, 1);


		//攻撃する弾の数を増やす
		BoxCount = 7;
	}

	//ヒットポイントが無くなっていたら
	if (this->GetHP() <= 0)
	{
		//パーティクルによる爆発
		auto ParticleObject = this->engine->Create<GameObject>(
			"particle explosion", this->GetPos());

		auto ps = ParticleObject->AddComponent<ParticleSystem>();
		ps->Emitters.resize(1);
		auto& emitter = ps->Emitters[0];
		emitter.ep.ImagePath = "Res/UI/exp.tga";
		emitter.ep.Duration = 0.1f;				//放出時間
		emitter.ep.RandomizeSize = 1;			//大きさをランダムに
		emitter.ep.RandomizeRotation = 1;		//角度をつける
		emitter.ep.RandomizeSize = 1;
		emitter.ep.EmissionsPerSecond = 30;		//秒間放出数
		emitter.pp.LifeTime = 3.0f;				//生存時間
		emitter.pp.color.Set({ 5, 5, 0.5f, 1 }, { 1, 2, 1.5f, 0 });	//色付け
		emitter.pp.scale.Set({ 0.06f,0.06f }, { 0.005f,0.005f });	//サイズを徐々にへ変更させる
		emitter.pp.radial.Set(50, -20);
		emitter.ep.Loop = true;

		Audio::PlayOneShot(SE::BossExplosion, 0.3f);

		return;
	}

	//エフェクトを出す
	CreateHitEffect(damage.amount);
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
			if (this->GetHP() >= MaxHP * 0.5f)
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
			static const float Center = BoxCount / 2.0f;	//何番目が箱の真ん中化を調べる

			GameObjectPtr Box = this->engine->Create<GameObject>("Box");

			auto boxRenderer = Box->AddComponent<MeshRenderer>();
			boxRenderer->mesh = engine->LoadOBJ("Box");
			boxRenderer->scale = vec3(1);
			Box->SetPos(this->GetPos() + Right * (i - Center) * Margine + Forward * 2.0f);//右から順番に生成される
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
			bullet->SetLiveTime(3.0f + i * 0.5f);	//生存時間の設定
			bullet->SetMoveTime(i * 0.5f);		//行動を開始するまでの時間を設定

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

void Boss::CreateHitEffect(float _count)
{

	const float Time = std::clamp(_count, 1.0f, _count);

	//ヒットエフェクトの生成
	auto ParticleObject = this->engine->Create<GameObject>(
		"particle explosion", this->GetPos());

	auto ps = ParticleObject->AddComponent<ParticleSystem>();
	ps->Emitters.resize(1);
	auto& emitter = ps->Emitters[0];
	emitter.ep.ImagePath = "Res/UI/hit.tga";
	emitter.ep.tiles = { 3,2 };				//画像枚数
	emitter.ep.Duration = Time * 0.1f;	//放出時間
	emitter.ep.RandomizeSize = 1;			//大きさをランダムに
	emitter.ep.RandomizeRotation = 1;		//角度をつける
	emitter.ep.EmissionsPerSecond = 10;		//秒間放出数
	emitter.pp.LifeTime = 0.3f;				//生存時間
	emitter.pp.color.Set({ 5, 1, 0.5f, 1 }, { 1, 2, 1.5f, 0 });	//色付け
	emitter.pp.scale.Set({ 0.05f,0.05f }, { 0.005f,0.005f });	//サイズを徐々にへ変更させる
	emitter.pp.velocity.Set({ 0,1,0 }, { 0,-10,0 });//上方向に放出

}
