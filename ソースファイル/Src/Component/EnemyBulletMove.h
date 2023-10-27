//@file EnemyBulletMove.h
#ifndef ENEMYBULLETMOVE_H_INCLUDED
#define ENEMYBULLETMOVE_H_INCLUDED

#include "../Engine/Component.h"
#include "../Application/GameObject.h"
#include "../Engine/VecMath.h"
#include "../Component/Player.h"
#include "../Component/ParticleSystem.h"
//#include "../Engine/VecMath.h"

using namespace VecMath;

//弾コンポーネント
class EnemyBulletMove :public Component
{
public:
	EnemyBulletMove() = default;				//コンストラクタ
	virtual ~EnemyBulletMove() = default;		//デストラクタ

	virtual void Start(GameObject& gameObject)override
	{
		RandomEngine.seed(rd());

		//乱数を使用するなら
		if (Randamness)
		{
			forward.x += std::clamp(
				std::normal_distribution<float>(0, 0.1f)(rd), -0.2f, 0.2f);	//大体０付近の乱数

			forward.y += std::clamp(
				std::normal_distribution<float>(0, 0.1f)(rd), -0.1f, 0.1f);	//大体０付近の乱数
		}

		////パーティクルによる爆発
		//ParticleObje = gameObject.engine->Create<GameObject>(
		//	"particle explosion", gameObject.GetPos());

		//auto ps = ParticleObje->AddComponent<ParticleSystem>();
		//ps->Emitters.resize(1);
		//auto& emitter = ps->Emitters[0];
		//emitter.ep.ImagePath = "Res/KP.tga";
		//emitter.ep.Duration = 0.1f;				//放出時間
		//emitter.ep.EmissionsPerSecond = 50;	//秒間放出数
		//emitter.ep.RandomizeRotation = 1;		//角度をつける
		//emitter.ep.RandomizeDirection = 1;
		//emitter.ep.RandomizeSize = 1;			//大きさをランダムに
		//emitter.pp.LifeTime = 0.4f;				//生存時間
		//emitter.pp.color.Set({ 5, 1, 0.5f, 1 }, { 1, 2, 1.5f, 0 });	//色付け
		//emitter.pp.scale.Set({ 0.01f,0.01f }, { 0.05f,0.1f });	//サイズを徐々にへ変更させる
		//emitter.ep.Loop = true;

	}

	//何に当たっても弾は壊れる
	virtual void OnCollision(GameObject& gameObject, GameObject& other)override
	{
		gameObject.SetDeadFlg(true);

	}

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{
		LiveTime -= deltaTime;

		//主人の正面方向に飛んでいく
		gameObject.AddPosition(forward * MoveSpeed * deltaTime);

		//5秒立ったら死ぬ
		if (LiveTime <= 0)
		{
			gameObject.SetDeadFlg(true);
		}
		//ParticleObje->SetPos(gameObject.GetPos());
	}

	void SetOwnerinfo(GameObject* _owner, GameObject& gameObject)
	{
		forward = _owner->GetForward();		//正面方向

		//自分より前から出現させる
		gameObject.SetPos(_owner->GetPos() + forward * 2);
		MoveSpeed = 30;
	}

	void SetPlayerinfo(Player* _player, GameObject& _obj)
	{
		forward = _player->GetForward();
		_obj.SetPos(_player->GetPos() + forward * 2);
		MoveSpeed = 50;
	}

	void SetRandom(bool _flg)
	{
		Randamness = _flg;
	}
private:

	VecMath::vec3 forward = VecMath::vec3(0, 0, 1);	//正面
	float MoveSpeed = 50;	//移動速度
	float LiveTime = 2;		//生存時間

	bool Randamness = false;//弾の飛び方に乱数を加える(true=乱れる　false=まっすぐ)

	//乱数
	std::random_device rd;
	std::mt19937 RandomEngine;

	GameObjectPtr ParticleObje;
};




#endif //BULLETMOVE_H_INCLUDED