//@file DeviationEnemy.h
#ifndef DEVIATIONENEMY_H_INCLUDED
#define DEVIATIONENEMY_H_INCLUDED
#include "../Engine/Sprite.h"
#include "../Engine/Rect.h"
#include "../Engine/Component.h"
#include "../Engine/VecMath.h"
#include "../Application/EnemyBase.h"
#include "../Engine/Engine.h"

using namespace VecMath;

enum class State
{
	eChase,		//追跡
	eStop,		//停止
	eShotWait,	//撃つまで待機
	eShot,		//射撃
	eChaseWait,	//追跡待ち
};


//偏差撃ちをする敵
class DeviationEnemy :public EnemyBase
{
public:
	DeviationEnemy() = default;				//コンストラクタ
	virtual ~DeviationEnemy() = default;	//デストラクタ

	//イベントの操作
	virtual void Update(float deltaTime)override;
	virtual void Move(float deltaTime)override;

	//射撃
	void Shot(float deltaTime);

	//ターゲットを設定
	void SetTarget(GameObjectPtr _target)
	{
		target = _target;
	}

private:

	//正面方向を計算して設定する
	void CalcForward();

	//偏差撃ちの距離を計算する
	void AddDeviation();

	//止まる
	bool Wait(float deltaTime);

	//ステートを変える
	void ChangeState(State nextstate);

	const float CHASETIME = 3.0f;	//追跡する時間
	const float CHARGETIME = 1.0f;	//止まって射撃するまでの時間
	const float SHOTWAIT  = 0.4f;	//射撃待機時間間隔
	const float CHASEWAIT = 0.5f;	//射撃後の移動を開始するまでの時間

	GameObjectPtr target;	//敵

	bool OnceFlg = false;	//一度だけ通ってほしいフラグ

	float MoveTimer = 0;	//何秒移動したかを記録する

	State state = State::eStop;
};





#endif //ENEMYBASE_H_INCLUDED