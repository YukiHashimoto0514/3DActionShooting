//@file Boss.h
#ifndef BOSS_H_INCLUDED
#define BOSS_H_INCLUDED

#include "../Application/GameObject.h"
#include "../Engine/Debug.h"
#include "../Engine/Engine.h"


enum class Routine
{
	Search,			//�T���i�v���C���[���j
	Think,			//���Ɏ��ׂ��s�����l����
	MoveRndPos,		//�����_���Ȉʒu�Ɉړ�
	MovePlayerPos,	//�v���C���[�Ɉړ�
	NearAttack,		//�ߍU��
	FarAttack,		//���U��
	EnemyCreate,	//�G�𐶐�
	JumpAttack,		//��тȂ���U��
	Wait,			//�x�e
	None,			//�����Ȃ�
};

//�{�X�N���X
class Boss :public GameObject
{
public:

	//�R���X�g���N�^
	Boss()
	{
		State = Routine::Search;
	};
	virtual ~Boss() = default;	//�f�X�g���N�^

	virtual void Update(float deltaTime)override;
	virtual void TakeDamage(GameObject& other, const Damage& damage)override;

private:

	//��Ԃ��Ƃ̃A�b�v�f�[�g
	void RoutineUpdate(Routine _state,float deltaTime);

	//��Ԃ�ω�������
	void ChangeState(Routine _state);

	//�U�����ꂽ���ɃG�t�F�N�g���o��
	void CreateHitEffect(float _count);	//�_���[�W�ɂ���ďo��G�t�F�N�g�̐����ς��

	Routine State = Routine::Search;	//�{�X�̏��
	GameObjectPtr player;	//�v���C���[
	Routine old = Routine::Search;
	int Same = 0;
	int BoxCount = 5;		//�U���ō�锠�̐�
	float BoxSize = 10.0f;		//�U���̔��̑傫��
	float MaxHP = 180;		//�ő�HP
	float TakeTime = 0;		//�C�[�W���O�̎��Ɏg�p���鎞��
	float MoveTimer = 0;	//�s������
	float WaitTime = 0;		//�ҋ@����
	float DeadTimer = 0;	//���񂾎�����v��
	float BattleTime = 0;	//�ǂ̂��炢�̎��Ԑ퓬���Ă��邩
	float velosity = 0;

	vec3 beforePos = vec3(0);
	vec3 targetPos = vec3(0);
	vec3 Direction = vec3(0);
};




#endif //BOSS_H_INCLUDED