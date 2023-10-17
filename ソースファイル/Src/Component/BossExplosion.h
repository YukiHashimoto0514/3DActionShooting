//@file BossExplosion.h

#ifndef COMPONENT_BOSSEXPLOSION_H_INCLUDED
#define COMPONENT_BOSSEXPLOSION_H_INCLUDED
#include "../Engine/Component.h"
#include "../Engine/SpritePriority.h"
#include "../Application/GameObject.h"
#include "../Engine/Engine.h"
#include "../Component/ParticleSystem.h"

//爆発コンポーネント

class BossExplosion :public Component
{
public:
	BossExplosion() = default;			//コンストラクタ
	virtual ~BossExplosion() = default;	//デストラクタ

	virtual void OnDestroy(GameObject& gameObject)override
	{
		//パーティクルによる爆発
		auto ParticleObject = gameObject.engine->Create<GameObject>(
			"particle explosion", gameObject.GetPos());

		auto ps = ParticleObject->AddComponent<ParticleSystem>();
		ps->Emitters.resize(1);
		auto& emitter = ps->Emitters[0];
		emitter.ep.ImagePath = "Res/star.tga";
		emitter.ep.Duration = 0.5f;				//放出時間
		emitter.ep.EmissionsPerSecond = 30;		//秒間放出数
		emitter.ep.RandomizeRotation = 1;		//角度ランダム性をつける
		emitter.ep.RandomizeDirection = 1;		//方向にランダム性を
		emitter.ep.RandomizeSize = 1;			//大きさをランダムに
		emitter.pp.rotation.Set(0, radians(720));	//何度まで回転させるか
		emitter.pp.LifeTime = 1.0f;				//生存時間
		emitter.pp.radial.Set(10, 0);			//速さ
		emitter.pp.color.Set({ 1, 1, 0.5f, 1 }, { 1, 2, 10.5f, 0 });	//色付け
		emitter.pp.scale.Set({ 2,2 }, { 1,1 });	//サイズを徐々にへ変更させる
	}


	virtual void OnTakeDamage(GameObject& gameObject, GameObject& other, const Damage& dammage)override
	{

		//ヒットエフェクトの生成
		auto ParticleObject = gameObject.engine->Create<GameObject>(
			"particle explosion", gameObject.GetPos());

		auto ps = ParticleObject->AddComponent<ParticleSystem>();
		ps->Emitters.resize(1);
		auto& emitter = ps->Emitters[0];
		emitter.ep.ImagePath = "Res/hit.tga";
		emitter.ep.tiles = { 3,2 };				//画像枚数
		emitter.ep.Duration = 0.3f;				//放出時間
		emitter.ep.RandomizeSize = 1;			//大きさをランダムに
		emitter.ep.RandomizeRotation = 1;		//角度をつける
		emitter.ep.EmissionsPerSecond = 10;		//秒間放出数
		emitter.pp.LifeTime = 0.3f;				//生存時間
		emitter.pp.color.Set({ 5, 1, 0.5f, 1 }, { 1, 2, 1.5f, 0 });	//色付け
		emitter.pp.scale.Set({ 0.11f,0.11f }, { 0.005f,0.005f });	//サイズを徐々にへ変更させる
		emitter.pp.velocity.Set({ 0,1,0 }, { 0,-10,0 });//上方向に放出
	}

};



#endif // !COMPONENT_BossExplosion_H_INCLUDED
