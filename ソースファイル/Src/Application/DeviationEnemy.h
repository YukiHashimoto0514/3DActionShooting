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
	eChase,		//�ǐ�
	eStop,		//��~
	eShotWait,	//���܂őҋ@
	eShot,		//�ˌ�
	eChaseWait,	//�ǐՑ҂�
};


//�΍�����������G
class DeviationEnemy :public EnemyBase
{
public:
	DeviationEnemy() = default;				//�R���X�g���N�^
	virtual ~DeviationEnemy() = default;	//�f�X�g���N�^

	//�C�x���g�̑���
	virtual void Update(float deltaTime)override;
	virtual void Move(float deltaTime)override;

	//�ˌ�
	void Shot(float deltaTime);

	//�^�[�Q�b�g��ݒ�
	void SetTarget(GameObjectPtr _target)
	{
		target = _target;
	}

private:

	//���ʕ������v�Z���Đݒ肷��
	void CalcForward();

	//�΍������̋������v�Z����
	void AddDeviation();

	//�~�܂�
	bool Wait(float deltaTime);

	//�X�e�[�g��ς���
	void ChangeState(State nextstate);

	const float CHASETIME = 3.0f;	//�ǐՂ��鎞��
	const float CHARGETIME = 1.0f;	//�~�܂��Ďˌ�����܂ł̎���
	const float SHOTWAIT  = 0.4f;	//�ˌ��ҋ@���ԊԊu
	const float CHASEWAIT = 0.5f;	//�ˌ���̈ړ����J�n����܂ł̎���

	GameObjectPtr target;	//�G

	bool OnceFlg = false;	//��x�����ʂ��Ăق����t���O

	float MoveTimer = 0;	//���b�ړ����������L�^����

	State state = State::eStop;
};





#endif //ENEMYBASE_H_INCLUDED