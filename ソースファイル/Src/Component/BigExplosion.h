//@file BigExplosion.h

#ifndef COMPONENT_BIGEXPLOSION_H_INCLUDED
#define COMPONENT_BIGEXPLOSION_H_INCLUDED
#include "../Component.h"
#include "../SpritePriority.h"
#include "../GameObject.h"
#include "../Engine.h"
#include "Animator2D.h"

//爆発コンポーネント

class BigExplosion :public Component
{
public:
	BigExplosion() = default;			//コンストラクタ
	virtual ~BigExplosion() = default;	//デストラクタ

	virtual void OnDestroy(GameObject& gameObject)override
	{
		//画面外で死んだら、エフェクトを再生しない
		if (gameObject.Point == 0)
		{
			return;
		}

		//爆発アニメーションデータ
		const Texcoord tc[] = {
			MakeTexcoord(303,400,32,32),
			MakeTexcoord(335,400,32,32),
			MakeTexcoord(369,400,48,48),
			MakeTexcoord(417,400,48,48),
			MakeTexcoord(465,400,48,48),
		};

		//爆発の表示座標データ
		const float x[] = { 0,-20,20,-25,27 };
		const float y[] = { 0,20,30,-24,-15 };

		//爆発を表示
		int i = static_cast<int>(std::size(x));

		if (count < i)
		{
			i = count;
		}

		for (--i; i >= 0; --i)
		{
			auto explosion = gameObject.engine->Create<GameObject>(
				"explosion", gameObject.x, gameObject.y);

			explosion->AddSprite(tc[0]);			//爆発画像
			explosion->priority = prioExplosion;	//優先順位
			explosion->Scale = scale;				//爆発画像の大きさ
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

	int count = 4;	//爆発個数
	float scale = 1;//大きさ
};



#endif // !COMPONENT_BIGEXPLOSION_H_INCLUDED
