//@file DropPowerUp.h

#ifndef COMPONENT_DROPPOWERUP_H_INCLUDED
#define COMPONENT_DROPPOWERUP_H_INCLUDED
#include "../Engine/Component.h"
#include "../Application/GameObject.h"
#include "../Component/Player.h"
#include "../Component/Collider.h"
#include "../Engine/Engine.h"
#include "../Application/ItemObject.h"

//プレイヤーを強化するコンポーネント

class DropPowerUp :public Component
{
public:

	//コンストラクタ
	DropPowerUp() 
	{ 	
		//乱数を初期化
		std::random_device rd;
		RandomEngine.seed(rd());
	};	

	virtual ~DropPowerUp() = default;	//デストラクタ

	virtual void OnDestroy(GameObject& gameObject)override
	{
		//ターゲットが設定されていないなら、無視
		if (!target)
		{
			return;
		}


		CrateEffect(gameObject);

	}

	PlayerPtr target;	//強化対象
	Engine engine;
	const int Red = 0;
	const int Green = 1;
	const int Blue = 2;

private:

	void CrateEffect(GameObject& gameObject)
	{
		//０～7でランダムな数を生成
		const int rand = std::uniform_int_distribution<>(0, 7)(RandomEngine);

		//3より大きかったら無視
		if (rand >= 3)
		{
			return;
		}

		//パーティクルによる爆発
		auto ParticleObject = gameObject.engine->Create<ItemObject>(
			"particle explosion", gameObject.GetPos());
		ParticleObject->player = target;	//対象の設定
		ParticleObject->SetSpot(rand);		//強化する部位の設定

		auto ps = ParticleObject->AddComponent<ParticleSystem>();
		ps->Emitters.resize(1);
		auto& emitter = ps->Emitters[0];
		emitter.ep.ImagePath = "Res/face.tga";
		emitter.ep.Duration = 0.5f;				//放出時間
		emitter.ep.EmissionsPerSecond = 10;	//秒間放出数
		emitter.ep.RandomizeRotation = 1;		//角度をつける
		emitter.ep.RandomizeDirection = 0;
		emitter.ep.RandomizeSize = 1;			//大きさをランダムに
		emitter.pp.LifeTime = 0.4f;				//生存時間
		emitter.pp.rotation.Set(90.0f,0);

		//乱数によって色を変える
		if (rand == Red)
		{
			emitter.pp.color.Set({ 255, 1, 1, 1 }, { 1, 1, 1.5f, 0 });	//色付け
		}
		else if(rand == Green)
		{
			emitter.pp.color.Set({ 2, 255, 1, 1 }, { 1, 1, 1.5f, 0 });	//色付け
		}
		else if (rand == Blue)
		{
			emitter.pp.color.Set({ 2, 1, 255.5f, 1 }, { 1, 1, 1.5f, 0 });	//色付け
		}
		emitter.pp.scale.Set({ 0.01f,0.01f }, { 0.05f,0.05f });	//サイズを徐々にへ変更させる
		emitter.pp.radial.Set(0, 10);			//角度方向に速度をつける
		
		emitter.ep.Loop = true;	//ループ再生させる


		//コライダーを割り当てる
		auto patcollider = ParticleObject->AddComponent<SphereCollider>();
		patcollider->sphere.Center = vec3(0);
		patcollider->sphere.Radius = 1.0f;
	}


	std::mt19937 RandomEngine;
};


#endif  //COMPONENT_POWERUP_H_INCLUDED