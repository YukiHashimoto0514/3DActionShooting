//@file HitEffect.h
#ifndef HITEFFECT_H_INCLUDED
#define HITEFFECT_H_INCLUDED

#include "../Engine/Component.h"
#include "../Application/GameObject.h"
#include "../Engine/VecMath.h"
#include "../Component/MeshRenderer.h"
#include "../Component/ParticleSystem.h"
//#include "../Engine/VecMath.h"

using namespace VecMath;

//被弾したパーティクルを出すコンポーネント
class HitEffect :public Component
{
public:

	HitEffect() = default;				//コンストラクタ
	virtual ~HitEffect() = default;	//デストラクタ

	virtual void OnTakeDamage(GameObject& gameObject, GameObject& other, const Damage& dammage)override
	{

		const float Time = std::clamp(dammage.amount, 1.0f, 3.0f);

		//ヒットエフェクトの生成
		auto ParticleObject = gameObject.engine->Create<GameObject>(
			"particle explosion", gameObject.GetPos());

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
		emitter.pp.scale.Set({ 0.01f,0.01f }, { 0.005f,0.005f });	//サイズを徐々にへ変更させる
		emitter.pp.velocity.Set({ 0,1,0 }, { 0,-10,0 });//上方向に放出
	}

};




#endif //HITEFFECT_H_INCLUDED