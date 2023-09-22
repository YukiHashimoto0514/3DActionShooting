//@file anotherExplosion.h

#ifndef COMPONENT_ANOTHEREXPLOSION_H_INCLUDED
#define COMPONENT_ANOTHEREXPLOSION_H_INCLUDED
#include "../Engine/Component.h"
#include "../Engine/SpritePriority.h"
#include "../Application/GameObject.h"
#include "../Engine/Engine.h"
#include "Animator2D.h"

//爆発コンポーネント

class anotherExplosion :public Component
{
public:
	anotherExplosion() = default;			//コンストラクタ
	virtual ~anotherExplosion() = default;	//デストラクタ

	virtual void OnDestroy(GameObject& gameObject)override
	{
		//爆発アニメーションデータ
		const Texcoord tc[] = {
			MakeTexcoord(0,520,120,120),
			MakeTexcoord(120,520,120,120),
			MakeTexcoord(240,520,120,120),
			MakeTexcoord(360,520,120,120),
			MakeTexcoord(480,520,120,120),
			MakeTexcoord(600,520,120,120),
			MakeTexcoord(720,520,120,120),
		};

		//爆発の表示座標データ
		const float x[] = { 0,-40,60,-50,70 };
		const float y[] = { 0,50,70,-60,-40 };

		//爆発を表示
		int i = static_cast<int>(std::size(x));

		if (count < i)
		{
			i = count;
		}

		for (--i; i >= 0; --i)
		{
			auto explosion = gameObject.engine->Create<GameObject>(
				"explosion", gameObject.GetPos());

			explosion->AddSprite(tc[0]);			//爆発画像
			explosion->SetPriority(prioExplosion);	//優先順位
			explosion->SetScale(vec3(scale,scale,scale));				//大きさ

			auto anim = explosion->AddComponent<Animator2D>();	//アニメーションを持たせる
			anim->loopTime = false;		//ループ再生しない
			anim->killWhenEnd = true;	//再生完了時にゲームオブジェクトを削除
			anim->clip.resize(std::size(tc));
			anim->interval = 0.07;		//アニメーションの再生速度
			for (size_t frameNo = 0; frameNo < std::size(tc); ++frameNo)
			{
				anim->AddSprite(frameNo, tc[frameNo], x[i], y[i]);
			}
		}
	}

	int count = 4;		//爆発個数
	float scale = 1;	//爆発画像の大きさ
	float mx = 0; float my = 0;	//ずらして表示するための移動値
};



#endif // !COMPONENT_ANOTHEREXPLOSION_H_INCLUDED
