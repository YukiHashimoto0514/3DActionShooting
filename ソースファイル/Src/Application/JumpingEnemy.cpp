#include "JumpingEnemy.h"
#include "../Engine/Debug.h"
void JumpingEnemy::Update(float deltaTime)
{

	Move(deltaTime);

	Jump(deltaTime);

	if (!this->GetAir())
	{
		MoveTimer += deltaTime;
		CalcForward();	//地上にいる間にプレイヤーを追いかける
	}

	//ぷにぷに指せる
	vec3 scale = GetScale();
	scale.y = std::sin(MoveTimer * 5) * 0.2f + 1;
	SetScale(scale);
	
}

void JumpingEnemy::Move(float deltaTime)
{
	const static float MOVESPEED = 3.0f;

	//ターゲットが指定されていなかったら
	if (target == NULL)
	{
		this->AddPosition(vec3(0, 1, 0) * deltaTime);

		return;
	}
	else
	{
		//正面に移動
		this->AddPosition(this->GetForward() * deltaTime * MOVESPEED);
	}

}

void JumpingEnemy::OnCollision(GameObject& other)
{
	//床だったら着地状態へ
	if (other.name == "floor")
	{
		ChangeScale(JumpState::Ground);
	}
}

void JumpingEnemy::CalcForward()
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

void JumpingEnemy::Jump(float deltaTime)
{
	//重力
	static const float Gravity = 9.8f;

	//地上
	if (!this->GetAir())
	{
		//ジャンプする時がきた
		if (MoveTimer >= JumpCoolTime)
		{
			//ジャンプ可能
			if (!this->GetJump())
			{
				SetJumpFlg(true);
				SetAir(true);
				v0 = rand() % 5 + 5.0f;			//ジャンプの力
				JumpCoolTime = rand() % 3 + 1.0f;	//次飛べるまでの時間
				JumpTimer = 0;
				MoveTimer = 0;
				this->AddPosition(vec3(0, v0 * deltaTime, 0));	//少し飛んでおく
				ChangeScale(JumpState::Up);//上昇状態へ
			}
		}

		//重力を常にかける
		this->AddPosition(vec3(0, -1, 0) * deltaTime);
	}
	else//空中
	{
		JumpTimer += deltaTime;

		//どれくらい落ちるかを
		const float UpPower = this->v0;
		const float DownPower = Gravity * JumpTimer * JumpTimer;
		FallPower = (UpPower - DownPower) * deltaTime;

		//追加
		this->AddPosition(vec3(0, FallPower, 0));

		//落下中なら
		if (FallPower <= 0)
		{
			if(state==JumpState::Up)
			ChangeScale(JumpState::Fall);
		}
	}
}

//お遊びで形を変形
void JumpingEnemy::ChangeScale(JumpState _state)
{
	if (state == _state) {
		return;
	}

	state = _state;

	switch (_state)
	{
	case JumpState::Up:
		this->SetScale(vec3(0.5f, 1.5f, 0.5f));
		break;

	case JumpState::Fall:
		this->SetScale(vec3(1.0f, 1.0f, 1.0f));
		break;

	case JumpState::Ground:
		this->SetScale(vec3(1.5f, 0.5f, 1.5f));

		break;

	default:
		break;
	}
}

