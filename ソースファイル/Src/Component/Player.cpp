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
	//プレイヤーが死んでいたら
	if (this->GetHP() <= 0)
	{
		//フェードが完了したら
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

	//無敵時間を減らす
	if (this->GetImmortalTime() > 0)
	{
		this->DecImmortalTime(deltaTime);
	}
	else
	{
		this->SetImmortalTime(0);
	}

	//~~雑実装~~\\
	
	//通常攻撃にする
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
	//無敵時間がないなら
	if (this->GetImmortalTime() <= 0)
	{
		//ヒットポイントを減らす
		this->SetHP(this->GetHP() - damage.amount);

		//無敵時間をセット
		this->SetImmortalTime(2.0f);

		//ヒットポイントが無くなっていたら
		if (this->GetHP() <= 0)
		{
			//フェードアウトを開始
			engine->SetFadeRule("Res/Fade/fade_rule4.tga");
			engine->SetFadeColor("Res/Fade/fade_colorR.tga");
			engine->StartFadeOut(2.0f);	//2秒かけてフェードする

			//自身の当たり判定を消す
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
		//これを外すと、壁ジャンができるようになる
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
		//空中でジャンプボタンを押したら
		if (engine->GetKey(GLFW_KEY_SPACE))
		{
			JumpTimer -= deltaTime;
		}
	}

}

void Player::Move(float deltaTime)
{
	//正面方向を取得
	vec3 forward = this->GetForward();
	forward.y = 0;

	//右方向を取得
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

	//キー入力で移動させる
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
	//射撃クールタイムを減らす
	ShotCoolTime -= deltaTime;
	

	//マウスの左クリック
	int state = glfwGetMouseButton(engine->GetWindow(), GLFW_MOUSE_BUTTON_LEFT);
	if (state == GLFW_PRESS)
	{
		//射撃クールタイムが0より大きいなら
		if (ShotCoolTime > 0)
		{
			//何もせん
			return;
		}

		switch (ShotStyle)
		{
		case 0://一発が重いやつ(射程が長い)
		{
			ShotCoolTime = this->ShotInterval;//射撃感覚の設定

			//弾の生成
			auto bullet = engine->Create<GameObject>("bullet");
			auto move = bullet->AddComponent<BulletMove>();
			move->SetPlayerinfo(this, *bullet);

			//見た目の設定
			auto bulletRenderer = bullet->AddComponent<MeshRenderer>();
			bulletRenderer->mesh = engine->LoadOBJ("Sphere");
			bulletRenderer->scale = vec3(0.5f);

			//個別に色を変更できるように、マテリアルのコピーを作成
			bulletRenderer->materials.push_back(
				std::make_shared<Mesh::Material>(*bulletRenderer->mesh->materials[0]));

			//当たり判定の設定
			auto bulletCollider = bullet->AddComponent<SphereCollider>();
			bulletCollider->sphere.Center = bulletRenderer->translate;
			bulletCollider->sphere.Radius = std::max({
				bulletRenderer->scale.x,
				bulletRenderer->scale.y,
				bulletRenderer->scale.z });

			//ダメージを与える相手の設定
			auto da = bullet->AddComponent<DamageSource>();
			da->targetName = "enemy";
			da->amount = ShotDamage;	//ダメージ調整

		//射撃音を鳴らす
			Audio::PlayOneShot(SE::playerShot, 0.2f);

			break;
		}
		case 1://連射速度が速い
		{
			ShotCoolTime = this->ShotInterval - 0.3f;//射撃感覚の設定

			//弾の生成
			auto bullet = engine->Create<GameObject>("bullet");
			auto move = bullet->AddComponent<BulletMove>();
			move->SetPlayerinfo(this, *bullet);
			move->SetRandom(true);
			
			//見た目の設定
			auto bulletRenderer = bullet->AddComponent<MeshRenderer>();
			bulletRenderer->mesh = engine->LoadOBJ("Sphere");
			bulletRenderer->scale = vec3(0.5f);

			//個別に色を変更できるように、マテリアルのコピーを作成
			bulletRenderer->materials.push_back(
				std::make_shared<Mesh::Material>(*bulletRenderer->mesh->materials[0]));

			bulletRenderer->materials[0]->baseColor = vec4{ 0.2f,0.3f,4.0f,1 };

			//当たり判定の設定
			auto bulletCollider = bullet->AddComponent<SphereCollider>();
			bulletCollider->sphere.Center = bulletRenderer->translate;
			bulletCollider->sphere.Radius = std::max({
				bulletRenderer->scale.x,
				bulletRenderer->scale.y,
				bulletRenderer->scale.z });

			//ダメージを与える相手の設定
			auto da = bullet->AddComponent<DamageSource>();
			da->targetName = "enemy";
			da->amount = ShotDamage * 0.6f;//ダメージ調整
			da->isOnce = true;

			//射撃音を鳴らす
			Audio::PlayOneShot(SE::playerShot, 0.2f);

			break;
		}
		case 2://ショットがん的な
		{
			ShotCoolTime = this->ShotInterval + 0.5f;//射撃感覚の設定

			for (int i = 0; i < 10; ++i)
			{
				//弾の生成
				auto bullet = engine->Create<GameObject>("bullet");
				auto move = bullet->AddComponent<BulletMove>();
				move->SetPlayerinfo(this, *bullet);
				move->SetRandom(true);

				//見た目の設定
				auto bulletRenderer = bullet->AddComponent<MeshRenderer>();
				bulletRenderer->mesh = engine->LoadOBJ("Sphere");
				bulletRenderer->scale = vec3(0.5f);

				//個別に色を変更できるように、マテリアルのコピーを作成
				bulletRenderer->materials.push_back(
					std::make_shared<Mesh::Material>(*bulletRenderer->mesh->materials[0]));
				bulletRenderer->materials[0]->baseColor = vec4{ 3,2,0.2f,1 };

				//当たり判定の設定
				auto bulletCollider = bullet->AddComponent<SphereCollider>();
				bulletCollider->sphere.Center = bulletRenderer->translate;
				bulletCollider->sphere.Radius = std::max({
					bulletRenderer->scale.x,
					bulletRenderer->scale.y,
					bulletRenderer->scale.z });

				//ダメージを与える相手の設定
				auto da = bullet->AddComponent<DamageSource>();
				da->targetName = "enemy";
				da->amount = ShotDamage * 0.5f;	//ダメージを調整
				da->isOnce = true;


			}

			//射撃音を鳴らす
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
	//ジャンプ中じゃないなら(自然落下)
	if (!this->GetJump())
	{
		this->AddPosition(vec3(0, -0.06f - FallTimer * FallTimer * 9.8f * deltaTime, 0));

		this->v0 = 0;
		JumpTimer = 0;
	}
	else //ジャンプによる落下
	{
		JumpTimer += deltaTime * 2;

		float UpPower = this->v0;

		float DownPower = Gravity * JumpTimer * JumpTimer;

		float FallPower = (UpPower - DownPower) * deltaTime;
		this->AddPosition(vec3(0, FallPower, 0));

	}

	//空中なら
	if (this->GetAir())
	{
		FallTimer += deltaTime;
	}
	//空中じゃないなら
	else
	{
		FallTimer = 0;
		JumpTimer = 0;
	}

}

void Player::SelectShot(int _shot)
{

	//選択している武器と同じなら無視
	if (ShotStyle == _shot)
	{
		return;
	}

	//押された武器が使えるかどうかを確認
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

	//効果音を鳴らす
	Audio::PlayOneShot(SE::WeaponSelect, 0.2f);

	//武器を切り替える
	ShotStyle = _shot;

}

