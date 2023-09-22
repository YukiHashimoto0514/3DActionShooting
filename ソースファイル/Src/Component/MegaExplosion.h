//@file MegaExplosion.h

#ifndef COMPONENT_MEGAEXPLOSION_H_INCLUDED
#define COMPONENT_MEGAEXPLOSION_H_INCLUDED
#include "../Engine/Component.h"
#include "../Engine/SpritePriority.h"
#include "../Application/GameObject.h"
#include "../Engine/Engine.h"
#include "Animator2D.h"

//爆発コンポーネント

class MegaExplosion :public Component
{
public:
	MegaExplosion() = default;			//コンストラクタ
	virtual ~MegaExplosion() = default;	//デストラクタ

	virtual void OnDestroy(GameObject& gameObject)override
	{
		//パーティクルによる爆発
		auto ParticleObject = gameObject.engine->Create<GameObject>(
			"particle explosion", gameObject.GetPos());

		auto ps = ParticleObject->AddComponent<ParticleSystem>();
		ps->Emitters.resize(1);
		auto& emitter = ps->Emitters[0];
		emitter.ep.ImagePath = "Res/KP.tga";
		emitter.ep.Duration = 0.5f;				//放出時間
		emitter.ep.EmissionsPerSecond = 30;		//秒間放出数
		emitter.ep.RandomizeRotation = 1;		//角度ランダム性をつける
		emitter.ep.RandomizeDirection = 1;		//方向にランダム性を
		emitter.ep.RandomizeSize = 1;			//大きさをランダムに
		emitter.pp.rotation.Set(0, radians(720));	//何度まで回転させるか
		emitter.pp.LifeTime = 1.0f;				//生存時間
		emitter.pp.radial.Set(2, 0);			//速さ
		emitter.pp.color.Set({ 1, 1, 0.5f, 1 }, { 1, 2, 10.5f, 0 });	//色付け
		emitter.pp.scale.Set({ 0.05f,0.05f }, { 0.005f,0.01f });	//サイズを徐々にへ変更させる
	}

};



#endif // !COMPONENT_EXPLOSION_H_INCLUDED
