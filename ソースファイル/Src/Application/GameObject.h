//@file GameObject.h
#ifndef GAMEOBJECT_H_INCLUDED
#define GAMEOBJECT_H_INCLUDED
#include "../Engine/Sprite.h"
#include "../Engine/Rect.h"
#include "../Engine/Component.h"
#include "../Engine/VecMath.h"
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <type_traits>

//��s�錾
class Engine;
class Player;
class Collider;
class GameObject;
class MeshRenderer;
class ProgramPipeline;

using PlayerPtr = std::shared_ptr<Player>;				//�v���C���[�̃Q�[���I�u�W�F�N�g�|�C���^
using ColliderPtr = std::shared_ptr<Collider>;			//�R���C�_�[�p�̔z��;
using GameObjectPtr = std::shared_ptr<GameObject>;		//�Q�[���I�u�W�F�N�g�̃|�C���^
using GameObjectList = std::vector<GameObjectPtr>;		//�Q�[���I�u�W�F�N�g�̃|�C���^�̔z��
using MeshRendererPtr = std::shared_ptr<MeshRenderer>;	//�`��R���|�[�l���g


using namespace VecMath;

//�Q�[�����ɓo�ꂷ��l�X�ȃQ�[���I�u�W�F�g��\�����N���X
class GameObject
{
public:
	GameObject() = default;				//�R���X�g���N�^
	virtual ~GameObject() = default;	//�f�X�g���N�^

	//�C�x���g�̑���
	virtual void Start();							//�ŏ���Update�̒��O�ŌĂяo�����
	virtual void Update(float deltaTime);			//���t���[��1��Ăяo�����
	virtual void OnCollision(GameObject& object);	//�Փ˂��N�����Ƃ��ɌĂяo�����
	virtual void OnDestroy();						//�Q�[���I�u�W�F�N�g���G���W������폜�����Ƃ��ɌĂяo�����
	virtual void TakeDamage(GameObject& other, const Damage& damage);	//�_���[�W���󂯂��Ƃ��ɌĂяo�����

	bool IsStarted() const { return isStarted; }	//�n�܂��Ă��邩�ǂ���

	//�X�v���C�g�̑���
	void AddSprite(const Texcoord& tc,
		float x = 0, float y = 0, float scale = 1, float rotation = 0);

	const SpriteList& GetSprite() const { return spriteList; }

	//�R���W�����̑���
	void AddCollision(const Rect& r);
	const RectList& GetCollision()const { return collisionList; }
	const Rect& GetAabb()const { return aabb; }
	
	//�R���|�[�l���g�̑���
	template<typename T>
	std::shared_ptr<T> AddComponent()	//����͂ǂ�Ȍ^�ɂł��Ή��ł���֐��̏������i���g���ꏏ�̏ꍇ�j
	{
		std::shared_ptr<T> p = std::make_shared<T>();
		componentList.push_back(p);

		//���b�V�������_���̏ꍇ�͐�p���X�g�ɒǉ�
		if constexpr (std::is_base_of<MeshRenderer, T>::value)//���N���X��Meshrendere�������Ă��邩
		{
			meshRendererList.push_back(p);
		}

		//�R���C�_�[��p���X�g�ɒǉ�
		if constexpr (std::is_base_of<Collider, T>::value)//���N���X��Collider�������Ă��邩
		{
			colliderList.push_back(p);
		}
		return p;
	}

	//�`��
	void DrawStaticMesh(const ProgramPipeline& program) const;

	void AddPosition(vec3 vec)
	{
		Position += vec;
	}
	void AddRotaion(vec3 rot)
	{
		Rotation += rot;
	}
	void AddHP(float _hp)
	{
		HP += _hp;
	}

	//���G���Ԃ����炷
	void DecImmortalTime(float _time)
	{
		ImmortalTime -= _time;
	}
	
	int GetPriority() const
	{
		return Priority;
	}

	float GetImmortalTime() const
	{
		return ImmortalTime;
	}
	float GetHP() const
	{
		return HP;
	}
	float GetAlpha() const
	{
		return Alpha;
	}

	const vec3& GetPos() const
	{
		return Position;
	}
	const vec3& GetReflection() const
	{
		return Reflection;
	}
	const vec3& GetRotation() const
	{
		return Rotation;
	}
	const vec3& GetForward() const
	{
		return Forward;
	}
	const vec3& GetRight() const
	{
		return Right;
	}
	const vec3& GetScale() const
	{
		return Scale;
	}
	const vec3& GetRGB() const
	{
		return RGB;
	}

	bool GetMovable() const
	{
		return isMovable;
	}
	bool GetJump() const
	{
		return isJump;
	}
	bool GetDead() const
	{
		return isDead;
	}
	bool GetAir() const
	{
		return isAir;
	}
	

	void SetImmortalTime(float _time)
	{
		ImmortalTime = _time;
	}
	void SetHP(float _hp)
	{
		HP = _hp;
	}
	void SetAlpha(float _alpha)
	{
		Alpha = _alpha;
	}

	void SetPriority(int _prio)
	{
		Priority = _prio;
	}

	void SetPos(vec3 _pos)
	{
		Position = _pos;
	}

	void SetRef(vec3 _ref)
	{
		Reflection = _ref;
	}
	void SetScale(vec3 _scale)
	{
		Scale = _scale;
	}
	void SetRotation(vec3 _rot)
	{
		Rotation = _rot;
	}
	void SetForward(vec3 _forward)
	{
		Forward = _forward;
	}
	void SetRight(vec3 _right)
	{
		Right = _right;
	}
	void SetRGB(vec3 _rgb)
	{
		RGB = _rgb;
	}

	void SetJumpFlg(bool _jump)
	{
		isJump = _jump;
	}
	void SetMoveFlg(bool _move)
	{
		isMovable = _move;
	}
	void SetDeadFlg(bool _dead)
	{
		isDead = _dead;
	}
	void SetAir(bool _air)
	{
		isAir = _air;
		if (isAir) {
			only = 0;
		}
	}


	Engine* engine = nullptr;		//�G���W���̃A�h���X
	std::string name;				//�I�u�W�F�N�g��

	float v0 = 5;					//�����x
	float JumpTimer = 0;			//���ł��鎞�Ԃ��v��

	SpriteList spriteList;			//�X�v���C�g�z��
	ComponentList componentList;	//�R���|�[�l���g�z��

	int only = 0;
	std::vector<ColliderPtr> colliderList;
private:
	vec3 Position = { 0,0,0 };	//���W
	vec3 Reflection = { 0,0,0 };//���˃x�N�g��
	vec3 Forward = { 0,0,0 };	//���ʂ̌���
	vec3 Right = { 0,0,0 };		//�E���̌���
	vec3 Rotation = { 0,0,0 };	//��]
	vec3 Scale = { 1,1,1 };		//�g�嗦
	vec3 RGB = { 1,1,1 };		//�R���F

	float HP = 1.0f;				//�̗�
	float ImmortalTime = 0;			//���G����
	float Alpha = 1.0f;				//�摜�̓����x
	int Priority = 0;

	bool isDead = false;			//���S�t���O
	bool isStarted = false;			//�X�^�[�g���s�ς݃t���O
	bool isJump = false;			//�W�����v�ł��邩�ǂ���(true=�ł��� false=�ł��Ȃ�)
	bool isMovable = true;			//�ړ��ł��邩�ǂ���(true=�ړ��ł���false=����)
	bool PinchFlg = false;			//true=HP��������菬���� false=HP�������ȏ�
	bool isAir = true;				//�󒆂ɂ��邩�ǂ���(true=��Ȃ��@false=��)

	RectList collisionList;			//�R���W�����z��
	Rect aabb = { 0,0,0,0 };		//�R���W�������E�{�b�N�X(�ŏ��̒����`)
	//   aabb = Axis Aligned Bounding Box

	std::vector<MeshRendererPtr> meshRendererList;
};


#endif //GAMEOBJECT_H_INCLUDED