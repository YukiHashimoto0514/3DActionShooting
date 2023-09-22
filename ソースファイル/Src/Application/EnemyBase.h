//@file EnemyBase.h
#ifndef ENEMYBASE_H_INCLUDED
#define ENEMYBASE_H_INCLUDED
#include "../Engine/Sprite.h"
#include "../Engine/Rect.h"
#include "../Engine/Component.h"
#include "../Engine/VecMath.h"
#include "../Application/GameObject.h"


using namespace VecMath;

//敵の基底クラス
class EnemyBase:public GameObject
{
public:
	EnemyBase() = default;			//コンストラクタ
	virtual ~EnemyBase() = default;	//デストラクタ

	//イベントの操作
	virtual void Move(float deltaTime) = 0;

private:

};




#endif //ENEMYBASE_H_INCLUDED