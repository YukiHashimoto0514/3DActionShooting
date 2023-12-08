#include "DeviationEnemy.h"
#include "../Component/EnemyBulletMove.h"
#include "../Component/MeshRenderer.h"
#include "../Component/Collider.h"
#include "../Component/DamageSource.h"

void DeviationEnemy::Update(float deltaTime)
{
	//それぞれのステートごとに処理
	switch (state)
	{
	case State::eChase:
	{
		MoveTimer += deltaTime;

		//次のステートに移行
		if (MoveTimer >= CHASETIME)
		{
			ChangeState(State::eStop);
		}

		static const float MINDISTANCE = 10.0f;	//プレイヤーとの最小距離
		static const float MAXDISTANCE = 80.0f;	//プレイヤーとの最大距離

		//プレイヤーの距離を計算
		float dx = target->GetPos().x - this->GetPos().x;
		float dy = target->GetPos().z - this->GetPos().z;
		const float distance = sqrt(dx * dx + dy * dy);

		//一定以内の距離だったら
		if (distance >= MINDISTANCE && distance <= MAXDISTANCE)
		{
			//プレイヤーの方向に移動
			Move(deltaTime);
		}
		//距離が既定値より小さかったら
		else if (distance <= MINDISTANCE)
		{
			//正面方向を向く
			CalcForward(target->GetPos());

			//次の行動までの時間を加速させる
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

		//次のステートに移行
		if (MoveTimer >= CHARGETIME)
		{
			ChangeState(State::eShotWait);
			OnceFlg = true;
		}

		//正面を計算
		CalcForward(target->GetPos());

		break;
	case State::eShotWait:

		if (OnceFlg)
		{
			//角度を加え、偏差をする
			AddDeviation();
			OnceFlg = false;
		}

		if (Wait(deltaTime))
		{
			//ステートを更新
			ChangeState(State::eShot);
		}
		break;

	case State::eShot:

		//射撃
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
	//正面方向を向く
	CalcForward(target->GetPos());

	//正面に移動
	this->AddPosition(this->GetForward() * deltaTime * 5);
}

void DeviationEnemy::Shot(float deltaTime)
{

	//弾の生成
	{
		auto enemyBullet = this->engine->Create<GameObject>("enemybullet");
		 
		auto enemyBulletRenderer = enemyBullet->AddComponent<MeshRenderer>();
		enemyBulletRenderer->mesh = this->engine->LoadOBJ("Sphere");
		enemyBulletRenderer->scale = vec3(0.5f);

		//個別に色を変更できるように、マテリアルのコピーを作成
		enemyBulletRenderer->materials.push_back(
			std::make_shared<Mesh::Material>(*enemyBulletRenderer->mesh->materials[0]));
		enemyBulletRenderer->materials[0]->baseColor = vec4{ 3,0.4f,1.2f,1 };
		enemyBulletRenderer->materials[0]->receiveShadows = false;

		//正面方向に進む
		auto enemyMove = enemyBullet->AddComponent<EnemyBulletMove>();
		enemyMove->SetOwnerinfo(this, *enemyBullet);

		//コライダーの設定
		auto enemyBulletCollider = enemyBullet->AddComponent<SphereCollider>();
		enemyBulletCollider->sphere.Center = enemyBulletRenderer->translate;
		enemyBulletCollider->sphere.Radius = std::max({
			enemyBulletRenderer->scale.x,
			enemyBulletRenderer->scale.y,
			enemyBulletRenderer->scale.z });

		//ダメージを与える先を指定
		auto da = enemyBullet->AddComponent<DamageSource>();
		da->targetName = "player";
		da->isOnce = true;

		//ステートを変える
		ChangeState(State::eChaseWait);
	}

}

void DeviationEnemy::CalcForward(vec3 target)
{
	//向きベクトルを取得
	vec3 Forward = this->GetPos() - target;

	//反転
	Forward *= vec3(-1);

	//正規化
	Forward = normalize(Forward);

	//外積で、右方向ベクトルを取得
	vec3 Right = cross(Forward, vec3{ 0,1,0 });

	//正規化する
	Right = normalize(Right);

	//設定する
 	this->SetForward(Forward);
	this->SetRight(Right);

	//角度を算出
	float rad = atan2(Forward.x, Forward.z);

	//正面方向に回転
	this->SetRotation(vec3(0, rad, 0));

}

//角度を加える
void DeviationEnemy::AddDeviation()
{
	vec3 Pos;

	int rnd = rand() % 3;
	//ランダムで左右どちらか正面を向く
	if (rnd == 0)
	{
		//左側をみる
		Pos = target->GetPos() - target->GetRight() * 2;
	}
	else if (rnd == 1)
	{
		//右側をみる
		Pos = target->GetPos() + target->GetRight() * 2;
	}
	else
	{
		//何もしない
		MoveTimer = 1.5f;
		return;
	}
	vec3 forward = this->GetPos() - Pos;

	//反転
	forward *= vec3(-1);

	//正規化
	forward = normalize(forward);

	//正面を再設定
	this->SetForward(forward);

	//角度を算出
	float rad = atan2(forward.x, forward.z);

	//正面方向に回転
	this->SetRotation(vec3(0, rad, 0));
	MoveTimer = 1.5f;
}

void DeviationEnemy::RandomMove(float deltaTime)
{
	//一回だけ入る
	if (!OnceFlg)
	{
		//ランダムの位置を生成
		const float x = std::uniform_real_distribution<float>(-10, 10)(engine->GetRandomGenerator());//-10~10
		const float z = std::uniform_real_distribution<float>(-10, 10)(engine->GetRandomGenerator());//-10~10
		RandomPos = { this->GetPos().x + x,this->GetPos().y,this->GetPos().z + z };
		OnceFlg = true;
	}
	CalcForward(RandomPos);

	//向いている方向に移動
	this->AddPosition(this->GetForward() * deltaTime * 5);
	MoveTimer -= deltaTime;		//攻撃に移行しない
	WaitTime += deltaTime;		//待機時間を追加

	//座標に到着したら、もう一度移動
	if (this->GetPos().x - RandomPos.x < 1 && this->GetPos().z - RandomPos.z < 1)
	{
		ChangeState(State::eChaseWait);
	}

	//範囲外の目標に走り続けた場合
	if (WaitTime >= CHASETIME)
	{
		//次に移動
		ChangeState(State::eChaseWait);
	}
}

bool DeviationEnemy::Wait(float deltaTime)
{
	MoveTimer -= deltaTime;

	if (MoveTimer <= 0)
	{
		//待ち時間が終ったよ
		return true;
	}

	//まだ待ってる
	return false;
}

void DeviationEnemy::ChangeState(State nextstate)
{
	state = nextstate;

	if (nextstate == State::eShotWait)
	{
		//待機時間を設定する
		MoveTimer = SHOTWAIT;
	}
	else if (nextstate == State::eChaseWait)
	{
		//待機時間を設定する
		MoveTimer = CHASEWAIT;
		WaitTime = 0;
		OnceFlg = false;

	}
	else
	{
		//タイマーをリセット
		MoveTimer = 0;
	}

}

