//@file Player.h
#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "../Application/GameObject.h"
#include "../Engine/Debug.h"
#include "../Engine/Engine.h"

//�v���C���[�N���X
class Player :public GameObject
{
public:
	Player() = default;				//�R���X�g���N�^
	virtual ~Player() = default;	//�f�X�g���N�^

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

	//�d��
	const float Gravity = 9.8f;

	float ShotInterval = 0.5f;	//�ˌ��N�[���^�C��
	float ShotCoolTime = 0;		//�ˌ��Ԋu
	float ShotDamage = 1.0f;	//�^����_���[�W
	float JumpTimer = 0;		//�W�����v�Ŏg���^�C�}�[
	float FallTimer = 0;		//�������Ɏg���^�C�}�[
	float MoveSpeed = 15.0f;	//�ړ����x
	float MaxHP = 7.0f;			//�ő�HP
	int ShotStyle = 0;			//�ˌ����@

	bool ShotGunFlg = false;	//�V���b�g�K�����g�p�ł��邩�ǂ���
	bool ShooterFlg = false;	//�V���[�^�[���g�p�ł��邩�ǂ���
	bool NearFlg = false;		//�߂����t���O
	void Jump(float deltaTime);	//�W�����v����
	void Move(float deltaTime);	//�ړ�����
	void Shot(float deltaTime);	//�ˌ�����
	void Fall(float deltaTime);	//��������
	void SelectShot(int _shot);	//����I������
};




#endif //GAMEOBJECT_H_INCLUDED