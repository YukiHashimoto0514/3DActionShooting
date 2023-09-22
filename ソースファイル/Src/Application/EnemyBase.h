//@file EnemyBase.h
#ifndef ENEMYBASE_H_INCLUDED
#define ENEMYBASE_H_INCLUDED
#include "../Engine/Sprite.h"
#include "../Engine/Rect.h"
#include "../Engine/Component.h"
#include "../Engine/VecMath.h"
#include "../Application/GameObject.h"


using namespace VecMath;

//�G�̊��N���X
class EnemyBase:public GameObject
{
public:
	EnemyBase() = default;			//�R���X�g���N�^
	virtual ~EnemyBase() = default;	//�f�X�g���N�^

	//�C�x���g�̑���
	virtual void Move(float deltaTime) = 0;

private:

};




#endif //ENEMYBASE_H_INCLUDED