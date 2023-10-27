//@file Explosion.h

#ifndef COMPONENT_EXPLOSION_H_INCLUDED
#define COMPONENT_EXPLOSION_H_INCLUDED
#include "../Engine/Component.h"
#include "../Engine/SpritePriority.h"
#include "../Application/GameObject.h"
#include "../Engine/Engine.h"
#include "../Application/EasyAudio.h"
#include "../Engine/AudioSettings.h"

#include "ParticleSystem.h"

//爆発コンポーネント

class Explosion :public Component
{
public: 
	Explosion() = default;			//コンストラクタ
	virtual ~Explosion() = default;	//デストラクタ

	virtual void OnDestroy(GameObject& gameObject)override
	{
		//パーティクルによる爆発
		auto ParticleObject = gameObject.engine->Create<GameObject>(
			"particle explosion", gameObject.GetPos());

		auto ps = ParticleObject->AddComponent<ParticleSystem>();
		ps->Emitters.resize(1);
		auto& emitter = ps->Emitters[0];
		emitter.ep.ImagePath = "Res/UI/Exp.tga";
		emitter.ep.tiles = { 1,1 };
		emitter.ep.Duration = 0.5f;				//放出時間
		emitter.ep.EmissionsPerSecond = 50;		//秒間放出数
		emitter.ep.RandomizeRotation = 1;		//角度をつける
		emitter.ep.RandomizeDirection = 1;
		emitter.ep.RandomizeSize = 1;			//大きさをランダムに
		emitter.pp.LifeTime = 0.4f;				//生存時間
		emitter.pp.color.Set({ 5, 1, 0.5f, 1 }, { 1, 2, 1.5f, 0 });	//色付け
		emitter.pp.scale.Set({ 0.05f,0.05f }, { 0.001f,0.001f });	//サイズを徐々にへ変更させる
		//emitter.ep.Shape = ParticleEmitterShape::box;]

		//効果音を鳴らす
		Audio::PlayOneShot(SE::enemyExplosionD, 0.2f);

	}

};



#endif // !COMPONENT_EXPLOSION_H_INCLUDED
