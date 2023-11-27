//@file Boss.h
#ifndef BOSS_H_INCLUDED
#define BOSS_H_INCLUDED

#include "../Application/GameObject.h"
#include "../Engine/Debug.h"
#include "../Engine/Engine.h"


enum class Routine
{
	Search,			//探索（プレイヤーを）
	Think,			//次に取るべき行動を考える
	MoveRndPos,		//ランダムな位置に移動
	MovePlayerPos,	//プレイヤーに移動
	NearAttack,		//近攻撃
	FarAttack,		//遠攻撃
	EnemyCreate,	//敵を生成
	JumpAttack,		//飛びながら攻撃
	Wait,			//休憩
	None,			//何もない
};

//ボスクラス
class Boss :public GameObject
{
public:

	//コンストラクタ
	Boss()
	{
		State = Routine::Search;
	};
	virtual ~Boss() = default;	//デストラクタ

	virtual void Update(float deltaTime)override;
	virtual void TakeDamage(GameObject& other, const Damage& damage)override;

private:

	//状態ごとのアップデート
	void RoutineUpdate(Routine _state,float deltaTime);

	//状態を変化させる
	void ChangeState(Routine _state);

	//攻撃された時にエフェクトを出す
	void CreateHitEffect(float _count);	//ダメージによって出るエフェクトの数が変わる

	Routine State = Routine::Search;	//ボスの状態
	GameObjectPtr player;	//プレイヤー
	Routine old = Routine::Search;
	int Same = 0;
	int BoxCount = 5;		//攻撃で作る箱の数
	float BoxSize = 10.0f;		//攻撃の箱の大きさ
	float MaxHP = 180;		//最大HP
	float TakeTime = 0;		//イージングの時に使用する時間
	float MoveTimer = 0;	//行動時間
	float WaitTime = 0;		//待機時間
	float DeadTimer = 0;	//死んだ時から計測
	float BattleTime = 0;	//どのくらいの時間戦闘しているか
	float velosity = 0;

	vec3 beforePos = vec3(0);
	vec3 targetPos = vec3(0);
	vec3 Direction = vec3(0);
};




#endif //BOSS_H_INCLUDED