//@file RedHPBar.h

#ifndef COMPONENT_REDHPBAR_H_INCLUDED
#define COMPONENT_REDHPBAR_H_INCLUDED

#include "../Engine/Component.h"
#include "../Application/GameObject.h"
#include "../Engine/Sprite.h"
#include "../Component/Lerp.h"

//赤いHPバーを動かすコンポーネント

class RedHPBar :public Component
{
public:
	RedHPBar() = default;			//コンストラクタ
	virtual ~RedHPBar() = default;	//デストラクタ

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{
		//設定されていないなら無視
		if (GreenBar == NULL)
		{
			return;
		}

		if (gameObject.GetHP() != GreenBar->GetHP())
		{
			if (TakeTime < 1)
			{
				if (!isOnce)
				{
					BeforePos = gameObject.GetPos();
					TargetPos = GreenBar->GetPos();
					isOnce = true;
				}
				gameObject.SetPos(
					vec3(BeforePos.x + Circle(TakeTime) * (TargetPos.x - BeforePos.x),
						BeforePos.y + Circle(TakeTime) * (TargetPos.y - BeforePos.y), 0));
			}
			else
			{
				//HPを同じにする
				gameObject.SetHP(GreenBar->GetHP());
				TakeTime = 0;	//時間をリセット
				isOnce = false;
			}

			//時間を経過
			TakeTime += deltaTime;
		}
		else
		{
			gameObject.SetPos(GreenBar->GetPos());
		}

	}

	void SetGreenBar(GameObjectPtr _bar)
	{
		GreenBar = _bar;
	}

private:

	GameObjectPtr GreenBar;	//付随する緑色のバー

	float TakeTime = 0;		//イージングに使う変数
	bool isOnce = false;	//一度だけ入る
	vec3 BeforePos = vec3(1);	//移動前の座標
	vec3 TargetPos = vec3(1);	//移動する座標
};


#endif // !COMPONENT_REDHPBAR_H_INCLUDED
