//@file Player.h
#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "../Application/GameObject.h"
#include "../Engine/Debug.h"
#include "../Engine/Engine.h"

//プレイヤークラス
class Player :public GameObject
{
public:
	Player() = default;				//コンストラクタ
	virtual ~Player() = default;	//デストラクタ

	virtual void Update(float deltaTime)override;
	virtual void TakeDamage(GameObject& other, const Damage& damage)override;
	virtual void OnCollision(GameObject& object)override;	

	int GetShotStyle() const
	{
		return ShotStyle;
	}

	float GetMaxHP()const
	{
		return MaxHP;
	}

	float GetShotInterval() const
	{
		return ShotInterval;
	}
	float GetMoveSpeed() const
	{
		return MoveSpeed;
	}
	float GetShotDamage() const
	{
		return ShotDamage;
	}
	bool GetShotGunFlg() const
	{
		return ShotGunFlg;
	}
	bool GetShooterFlg() const
	{
		return ShooterFlg;
	}

	void SetShotInterval(float _interval)
	{
		ShotInterval = _interval;
	}
	void SetMoveSpeed(float _speed)
	{
		MoveSpeed = _speed;
	}
	void SetShotDamage(float _damage)
	{
		ShotDamage = _damage;
	}

	void SetShotGunFlg(bool _flg)
	{
		ShotGunFlg = _flg;
	}
	void SetShooterFlg(bool _flg)
	{
		ShooterFlg = _flg;
	}

private:

	//重力
	const float Gravity = 9.8f;

	float ShotInterval = 0.5f;	//射撃クールタイム
	float ShotCoolTime = 0;		//射撃間隔
	float ShotDamage = 1.0f;	//与えるダメージ
	float JumpTimer = 0;		//ジャンプで使うタイマー
	float FallTimer = 0;		//落下中に使うタイマー
	float MoveSpeed = 15.0f;	//移動速度
	float MaxHP = 7.0f;			//最大HP
	int ShotStyle = 0;			//射撃方法

	bool ShotGunFlg = false;	//ショットガンを使用できるかどうか
	bool ShooterFlg = false;	//シューターを使用できるかどうか
	bool NearFlg = false;		//近すぎフラグ
	void Jump(float deltaTime);	//ジャンプ処理
	void Move(float deltaTime);	//移動処理
	void Shot(float deltaTime);	//射撃処理
	void Fall(float deltaTime);	//落下処理
	void SelectShot(int _shot);	//武器選択処理
};




#endif //GAMEOBJECT_H_INCLUDED