#include "DeviationEnemy.h"
#include "../Component/BulletMove.h"
#include "../Component/MeshRenderer.h"
#include "../Component/Collider.h"
#include "../Component/DamageSource.h"

void DeviationEnemy::Update(float deltaTime)
{
	//それぞれのステートごとに処理
	switch (state)
	{
	case State::eChase:
		MoveTimer += deltaTime;

		//次のステートに移行
		if (MoveTimer >= CHASETIME)
		{
			ChangeState(State::eStop);
		}

		//正面方向を向く
		CalcForward();

		//移動する
		Move(deltaTime);
		break;

	case State::eStop:
		MoveTimer += deltaTime;

		//次のステートに移行
		if (MoveTimer >= CHARGETIME)
		{
			ChangeState(State::eShotWait);
			OnceFlg = true;
		}

		//正面を計算
		CalcForward();

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
	static const float DISTANCE = 20.0f;	//プレイヤーとの距離


	float dx = target->GetPos().x - this->GetPos().x;
	float dy = target->GetPos().z - this->GetPos().z;

	//ターゲットが指定されていなかったら
	if (target == NULL)
	{
		this->SetPos(this->GetPos() + vec3(0, 1, 0) * deltaTime);

		return;
	}
	//距離が既定値に達していたら
	else if (sqrt(dx* dx + dy* dy) <= DISTANCE)
	{
		//次の行動までの時間を加速させる
		MoveTimer += deltaTime;
		return;
	}
	//距離がある程度離れていたら詰める
	else
	{
		//正面に移動
		this->AddPosition(this->GetForward() * deltaTime * 5);
	}

}

void DeviationEnemy::Shot(float deltaTime)
{

	//弾の生成
	{
		auto enemybullet = this->engine->Create<GameObject>("enemybullet");
		 
		auto enemybulletRenderer = enemybullet->AddComponent<MeshRenderer>();
		enemybulletRenderer->mesh = this->engine->LoadOBJ("Sphere");
		enemybulletRenderer->scale = vec3(0.5f);

		//正面方向に進む
		auto enemymove = enemybullet->AddComponent<BulletMove>();
		enemymove->SetOwnerinfo(this, *enemybullet);

		//コライダーの設定
		auto enemybulletcollider = enemybullet->AddComponent<SphereCollider>();
		enemybulletcollider->sphere.Center = enemybulletRenderer->translate;
		enemybulletcollider->sphere.Radius = std::max({
			enemybulletRenderer->scale.x,
			enemybulletRenderer->scale.y,
			enemybulletRenderer->scale.z });

		//ダメージを与える先を指定
		auto da = enemybullet->AddComponent<DamageSource>();
		da->targetName = "player";
		da->isOnce = true;

		//ステートを変える
		ChangeState(State::eChaseWait);
	}

}

void DeviationEnemy::CalcForward()
{
	//向きベクトルを取得
	vec3 Forward = this->GetPos() - target->GetPos();

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
	}
	else
	{
		//タイマーをリセット
		MoveTimer = 0;
	}

}

