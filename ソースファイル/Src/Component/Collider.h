//@file Collider.h

#ifndef COMPONENT_COLLIDER_H_INCLUDED
#define COMPONENT_COLLIDER_H_INCLUDED

#include "../Engine/Component.h"
#include "../Engine/Collision.h"
#include "../Engine/VecMath.h"
#include <memory>

//��s�錾
class Collider;
using ColliderPtr = std::shared_ptr<Collider>;

//�Փˌ`��
class Collider : public Component
{
public:
	Collider() = default;			//�R���X�g���N�^
	virtual ~Collider() = default;	//�f�X�g���N�^

	//�R���C�_�[�`��
	enum class Type
	{
		Sphere,	//����
		AABB,	//�����s���E�{�b�N�X
		Box,	//�L�����E�{�b�N�X
		Capsule,//�J�v�Z��
	};

	//�ǂ̌`�󉻂��擾����
	virtual Type GetType() const = 0;

	//���W���X�V����
	virtual void AddPosition(const VecMath::vec3& translate) = 0;

	//���W�ϊ������R���C�_�[���擾����
	virtual ColliderPtr GetTransformedCollider(
		const VecMath::vec3& translation,
		const VecMath::mat3& matRotation,
		const VecMath::vec3& scale) = 0;

};


//���̗p�R���C�_�[
class SphereCollider :public Collider
{
public:

	SphereCollider() = default;				//�R���X�g���N�^
	virtual ~SphereCollider() = default;	//�f�X�g���N�^

	//�R���C�_�[�̌`��
	virtual Type GetType()const override
	{
		return Type::Sphere;
	}

	//�R���W�����̌`��
	const Collision::Sphere& GetShape() const 
	{
		return sphere;
	}

	//���W��ύX����
	virtual void AddPosition(const VecMath::vec3& translate)override;

	//���W�Ԋ҂����R���C�_�[���擾����
	virtual ColliderPtr GetTransformedCollider(
		const VecMath::vec3& translation,
		const VecMath::mat3& matRotation,
		const VecMath::vec3& scale)override;

	//����
	Collision::Sphere sphere = { VecMath::vec3(0),1 };
};


//AABB�p�R���C�_�[
class AABBCollider :public Collider
{
public:
	AABBCollider() = default;			//�R���X�g���N�^
	virtual ~AABBCollider() = default;	//�f�X�g���N�^

	//�R���C�_�[�̌`��
	virtual Type GetType() const override
	{
		return Type::AABB;
	}

	//�R���W�����̌`��
	const Collision::AABB& GetShape()const
	{
		return aabb;
	}

	//���W��ύX����
	virtual void AddPosition(const VecMath::vec3& translate)override;

	//���W�ϊ������R���C�_�[���擾����
	virtual ColliderPtr GetTransformedCollider(
		const VecMath::vec3& translation,
		const VecMath::mat3& matRotation,
		const VecMath::vec3& scale)override;

	//AABB
	Collision::AABB aabb;
};


//OBB�p�R���C�_�[
class BoxCollider :public Collider
{
public:
	BoxCollider() = default;			//�R���X�g���N�^
	virtual ~BoxCollider() = default;	//�f�X�g���N�^

	//�}�`���擾����
	virtual Type GetType() const override
	{
		return Type::Box;
	}

	//�`���擾����
	const Collision::Box& GetShape() const
	{
		return box;
	}

	//���W��ύX����
	virtual void AddPosition(const VecMath::vec3& translate)override;

	//���W�ϊ������R���C�_�[���擾����
	virtual ColliderPtr GetTransformedCollider(
		const VecMath::vec3& translation,
		const VecMath::mat3& matRotation,
		const VecMath::vec3& scale)override;

	Collision::Box box;
};

//�J�v�Z���p�R���C�_�[
class CapsuleCollider :public Collider
{
public:
	CapsuleCollider() = default;			//�R���X�g���N�^
	virtual ~CapsuleCollider() = default;	//�f�X�g���N�^

	//�}�`���擾����
	virtual Type GetType() const override { return Type::Capsule; }

	//�`���擾����
	const Collision::Capsule& GetShape() const { return capsule; }
	
	//���W��ύX����
	virtual void AddPosition(const VecMath::vec3& translate) override;

	//���W�ϊ������R���C�_�[���擾����
	virtual ColliderPtr GetTransformedCollider(
		const VecMath::vec3& translation,
		const VecMath::mat3& matRotation,
		const VecMath::vec3& scale)override;
	
	Collision::Capsule capsule = {VecMath::vec3(0)};
};

//�l�X�ȃR���C�_�[�ɑΉ��ł���Փ˔���
bool Intersect(const Collider& a, const Collider& b, Collision::ContactPoint& cp);

#endif // !COMPONENT_COLLIDER_H_INCLUDED

