//@file TreasureBox.h

#ifndef COMPONENT_TREASUREBOX_H_INCLUDED
#define COMPONENT_TREASUREBOX_H_INCLUDED
#include "../Engine/Component.h"
#include "ParticleSystem.h"
#include "../Component/Player.h"
#include <stdio.h>

//宝箱コンポーネント
class TreasureBox :public Component
{
public:
	TreasureBox() = default;			//コンストラクタ
	virtual ~TreasureBox() = default;	//デストラクタ

	virtual void Start(GameObject& gameObject)override
	{
		//パーティクルに派手目なエフェクト
		ParticleObj = gameObject.engine->Create<GameObject>(
			"particle explosion", gameObject.GetPos());

		auto ps = ParticleObj->AddComponent<ParticleSystem>();
		ps->Emitters.resize(1);
		auto& emitter = ps->Emitters[0];
		
			emitter.ep.ImagePath = "Res/UI/star.tga";
			emitter.ep.Duration = 0.1f;				//放出時間
			emitter.ep.RandomizeSize = 1;			//大きさをランダムに
			emitter.ep.RandomizeRotation = 1;		//角度をつける
			emitter.ep.EmissionsPerSecond = 10;		//秒間放出数
			emitter.pp.LifeTime = 3.0f;				//生存時間
			emitter.pp.color.Set({ 5, 1, 0.5f, 1 }, { 1, 2, 1.5f, 0 });	//色付け
			emitter.pp.scale.Set({ 0.01f,0.01f }, { 0.005f,0.005f });	//サイズを徐々にへ変更させる
			emitter.pp.velocity.Set({ 0,25,0 }, { 0,-1,0 });//上方向に放出

			emitter.ep.Loop = true;



	}

	virtual void OnCollision(GameObject& gameObject, GameObject& other)override
	{
		if (other.name == Target->name)
		{
			//パーティクルを消す
			ParticleObj->SetDeadFlg(true);

			//ショットガンかシューターを獲得できる
			if (!Target->GetShooterFlg())
			{
				Target->SetShooterFlg(true);
				gameObject.componentList.clear();	//複数回当たらないようにする
			}
			else if (!Target->GetShotGunFlg())
			{
				Target->SetShotGunFlg(true);
				gameObject.componentList.clear();	//複数回当たらないようにする
			}
		}
	}

	void SetTarget(PlayerPtr _p)
	{
		Target = _p;
	}

private:
	GameObjectPtr ParticleObj;
	PlayerPtr Target;

};


#endif // !COMPONENT_TREASUREBOX_H_INCLUDED
