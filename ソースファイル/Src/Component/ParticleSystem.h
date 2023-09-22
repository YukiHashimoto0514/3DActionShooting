//@file ParticleSystem.h

#ifndef COMPONENT_PARTICLESYSTEM_H_INCLUDED
#define COMPONENT_PARTICLESYSTEM_H_INCLUDED

#include "../Engine/Component.h"
#include "../Engine/Engine.h"

//パーティクル管理コンポーネント

//コンポーネント作成時にEmitter配列のサイズを設定し、ep,ppを設定しておく。
//そして、Update関数でエミッターを作成

class ParticleSystem :public Component
{
public:
	//コンストラクタ
	ParticleSystem() = default;

	//デストラクタ
	~ParticleSystem() = default;

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{
		for (auto& e : Emitters)
		{
			//エミッターを作成
			if (!e.emitter)
			{
				e.emitter = gameObject.engine->AddParticleEmitter(e.ep, e.pp);
			}

			//パラメータを取得し、座標を更新
			auto& parameter = e.emitter->GetEmitterParameter();
			parameter.position = e.ep.position + gameObject.GetPos();
		}

		auto itr = std::partition(Emitters.begin(), Emitters.end(),
			[](const Parameter& p) {return !p.emitter->IsDead(); });

		Emitters.erase(itr, Emitters.end());

		//すべてのエミッターが死んだらゲームオブジェクトを消す
		if (Emitters.empty() && KillWhenEmpty)
		{
			gameObject.SetDeadFlg(true);
		}
	}

	//破壊処理
	virtual void OnDestroy(GameObject& gameObject) override
	{
		for (auto& e : Emitters)
		{
			e.emitter->Die();
		}
	}

	//epとppにパラメーターを設定しておくと、エミッターが生成される
	struct Parameter
	{
		ParticleEmitterParameter ep;
		ParticleParameter pp;
		ParticleEmitterPtr emitter;
	};



	std::vector<Parameter> Emitters;

	//すべてのエミッターが死んだらゲームオブジェクトを消す
	bool KillWhenEmpty = true;
};


#endif //COMPONENT_PARTICLESYSTEM_H_INCLUDED
