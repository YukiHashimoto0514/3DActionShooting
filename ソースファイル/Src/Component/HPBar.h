//@file HPBar.h

#ifndef COMPONENT_HPBAR_H_INCLUDED
#define COMPONENT_HPBAR_H_INCLUDED

#include "../Engine/Component.h"
#include "../Application/GameObject.h"
#include "../Engine/Sprite.h"

//HPバーを動かすコンポーネント

class HPBar :public Component
{
public:
	HPBar() = default;			//コンストラクタ
	virtual ~HPBar() = default;	//デストラクタ

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{
		//設定されていないなら無視
		if (player == NULL)
		{
			LOG("プレイヤーを設定してください");
			return;
		}

		//ゲームオブジェクトに設定
		gameObject.SetHP(player->GetHP());

		//変数に代入
		NowHP = gameObject.GetHP();

		//割合を計算
		float diff = NowHP / MaxHP;

		//割合の位置を設定
		gameObject.SetPos({ 460 * diff,646,0 });
	}

	//最大HPの設定(ついでに現在のHPも合わせておく)
	void SetMaxHP(float _max)
	{
		MaxHP = _max;
		NowHP = _max;
	}

	void SetNowHP(float _hp)
	{
		NowHP = _hp;
	}

	void SetPlayer(GameObjectPtr _player)
	{
		player = _player;
	}

private:

	float MaxHP = 0;		//最大HP
	float NowHP = 0;		//現在のHP
	
	GameObjectPtr player;	//参照するHP

};


#endif // !COMPONENT_HPBAR_H_INCLUDED
