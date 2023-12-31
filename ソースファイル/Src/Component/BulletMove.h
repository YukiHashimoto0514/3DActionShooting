//@file BulletMove.h
#ifndef BULLETMOVE_H_INCLUDED
#define BULLETMOVE_H_INCLUDED

#include "../Engine/Component.h"
#include "../Application/GameObject.h"
#include "../Engine/VecMath.h"
#include "../Component/Player.h"
//#include "../Engine/VecMath.h"

using namespace VecMath;

//弾コンポーネント
class BulletMove :public Component
{
public:
	BulletMove() = default;				//コンストラクタ
	virtual ~BulletMove() = default;		//デストラクタ

	virtual void Start(GameObject& gameObject)override
	{
		RandomEngine.seed(rd());

		//乱数を使用するなら
		if (Randamness)
		{
			forward.x += std::clamp(
				std::normal_distribution<float>(0.0f, 0.05f)(rd), -0.1f, 0.1f);	//大体０付近の乱数

			forward.y += std::clamp(
				std::normal_distribution<float>(0.0f, 0.05f)(rd), -0.1f, 0.1f);	//大体０付近の乱数

			forward.z += std::clamp(
				std::normal_distribution<float>(0.0f, 0.05f)(rd), -0.1f, 0.1f);	//大体０付近の乱数
		}


	}

	//何に当たっても弾は壊れる
	virtual void OnCollision(GameObject& gameObject, GameObject& other)override
	{
		//玉同氏は無視
		if(other.name=="bullet")
		{
			return;
		}

		//反射するかどうかをチェック
		if (BoundFlg)
		{
			forward = gameObject.GetReflection();
		}
		else
		{
			gameObject.SetDeadFlg(true);
		}
	}

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{
		LiveTime -= deltaTime;

		//主人の正面方向に飛んでいく
		gameObject.AddPosition(forward * MoveSpeed * deltaTime);
		gameObject.SetForward(forward);
		//5秒立ったら死ぬ
		if (LiveTime <= 0)
		{
			gameObject.SetDeadFlg(true);
		}
	}

	void SetOwnerinfo(GameObject* _owner, GameObject& gameObject)
	{
		forward = _owner->GetForward();		//正面方向

		//自分より前から出現させる
		gameObject.SetPos(_owner->GetPos() + forward * 2);
		MoveSpeed = 30;
	}

	void SetPlayerinfo(Player* _player, GameObject& _obj)
	{
		forward = _player->GetForward();
		_obj.SetPos(_player->GetPos() + forward * 2);
		MoveSpeed = 50;
	}

	void SetRandom(bool _flg)
	{
		Randamness = _flg;
	}

	void SetBound(bool _bound)
	{
		BoundFlg = _bound;
	}
private:

	VecMath::vec3 forward = VecMath::vec3(0,0,1);	//正面
	float MoveSpeed = 50;	//移動速度
	float LiveTime = 2;		//生存時間
	bool Randamness = false;//弾の飛び方に乱数を加える(true=乱れる　false=まっすぐ)
	bool BoundFlg = false;	//跳ね返るかどうか(true=跳ねる　false=跳ねない)
	//乱数
	std::random_device rd;
	std::mt19937 RandomEngine;

};




#endif //BULLETMOVE_H_INCLUDED