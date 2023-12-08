//@file BossCubeBullet.h
#ifndef BOSSCUBEBULLET_H_INCLUDED
#define BOSSCUBEBULLET_H_INCLUDED

#include "../Engine/Component.h"
#include "../Application/GameObject.h"
#include "../Engine/VecMath.h"
#include "../Component/Player.h"

using namespace VecMath;

//ボスが発する弾コンポーネント
class BossCubeBullet :public Component
{
public:
	BossCubeBullet() = default;				//コンストラクタ
	virtual ~BossCubeBullet() = default;	//デストラクタ

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{
		//命を削る
		LiveTime -= deltaTime;

		//移動可能時間まで待機
		if (MoveTimer >= 0)
		{
			MoveTimer -= deltaTime;
			return;
		}

		//回転指せる
		Rot += deltaTime;

		//良い感じに回転させる
		const float Rotate = Rot * deltaTime;
		gameObject.AddRotaion(vec3(Rotate));

		//正面方向を計算
		forward = player->GetPos() - gameObject.GetPos();
		forward = normalize(forward);

		//プレイヤー方向に飛んでいく
		gameObject.AddPosition(forward * MoveSpeed * deltaTime);

		//時間が来たら死ぬ
		if (LiveTime <= 0)
		{
			gameObject.SetDeadFlg(true);
		}
	}

	virtual void OnCollision(GameObject& gameObject, GameObject& other)override
	{
		gameObject.SetDeadFlg(true);
	}

	void SetLiveTime(float _time)
	{
		LiveTime = _time;
	}

	void SetMoveTime(float _timer)
	{
		MoveTimer = _timer;
	}

	void SetPlayer(GameObjectPtr _player)
	{
		player = _player;
	}

	void SetMoveSpeed(float _speed)
	{
		MoveSpeed = _speed;
	}
private:

	VecMath::vec3 forward = VecMath::vec3(0, 0, 1);	//正面
	float MoveSpeed = 10;	//移動速度
	float LiveTime = 60;	//生存時間
	float MoveTimer = 0;	//行動を開始するまでの時間
	float Rot = 0;			//回転させる

	GameObjectPtr player;
};




#endif //BOSSCUBEBULLET_H_INCLUDED