//@file JumpingEnemy.h
#ifndef JUMPINGNENEMY_H_INCLUDED
#define JUMPINGENEMY_H_INCLUDED
#include "../Engine/Sprite.h"
#include "../Engine/Rect.h"
#include "../Engine/Component.h"
#include "../Engine/VecMath.h"
#include "../Application/EnemyBase.h"
#include "../Engine/Engine.h"

using namespace VecMath;

enum class JumpState
{
	Up,		//上昇中
	Fall,	//落下中
	Ground,	//着地中
};

//ジャンプしながら追いかけてくる敵
class JumpingEnemy :public EnemyBase
{
public:
	JumpingEnemy() = default;			//コンストラクタ
	virtual ~JumpingEnemy() = default;	//デストラクタ

	//イベントの操作
	virtual void Update(float deltaTime)override;
	virtual void Move(float deltaTime)override;
	virtual void OnCollision(GameObject& other);


	//ターゲットを設定
	void SetTarget(GameObjectPtr _target)
	{
		target = _target;
	}

private:

	//正面方向を計算する
	void CalcForward();

	//ジャンプする関数
	void Jump(float deltaTime);

	//形を変える
	void ChangeScale(JumpState _state);

	GameObjectPtr target;	//敵

	float MoveTimer = 0;		//何秒移動したかを記録する
	float JumpCoolTime = 0.5f;	//ジャンプするまでのクールタイム
	float FallPower = 0;		//下方向にかかる力
	bool OnceFlg = false;		//一度だけ通ってほしいフラグ

	
	JumpState state = JumpState::Ground;
};





#endif //JUMPINGENEMY_H_INCLUDED