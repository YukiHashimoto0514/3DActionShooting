//@file Collider.cpp

#include "Collider.h"

using namespace VecMath;

//���̃t�@�C���������Ŏg�p�ł���悤�ɂ���
namespace/*unnamed*/
{
	//�R���C�_�[�^�ɑΉ�����Փ˔���֐����Ăяo��
	template<typename T,typename U>
	bool Func(const Collider& a, const Collider& b,
		Collision::ContactPoint& cp)
	{
		return Intersect(static_cast<const T&>(a).GetShape(),
			static_cast<const U&>(b).GetShape(), cp);
	}

	//�Փ˂��Ȃ��g�ݍ��킹�Ɏg���֐�
	bool NotCollide(const Collider& a, const Collider& b,
		Collision::ContactPoint& cp)
	{
		return false;
	}
}

//�R���C�_�[�̏Փ˔���
//@param a      �Փ˔�����s���R���C�_�[���̂P
//@param b      �Փ˔�����s���R���C�_�[���̂Q
//@param cp     �Փˌ��ʂ��i�[����R���^�N�g�|�C���g

//@retval true  �Փ˂��Ă���
//@retval false �Փ˂��Ă��Ȃ�

bool Intersect(const Collider& a, const Collider& b,
	Collision::ContactPoint& cp)
{
	//�֐��|�C���^�^���`	(�|�C���^���Q�Ƃ��Ă���֐����Ăяo��:���̏ꍇ���ƁAFunc�֐�)
	using FuncType =
		bool(*)(const Collider&, const Collider&, Collision::ContactPoint&);

	//�֐��e�[�u��	(���s����֐���؂�ւ���)
	static const FuncType FuncList[4][4] = {
		{
			Func<SphereCollider,SphereCollider>,
			Func<SphereCollider,AABBCollider>,
			Func<SphereCollider,BoxCollider>,
			Func<SphereCollider,CapsuleCollider>,
		},
		{
			Func<AABBCollider,SphereCollider>,
			Func<AABBCollider,AABBCollider>,
			Func<AABBCollider,BoxCollider>,
			Func<AABBCollider,CapsuleCollider>,
		},
		{
			Func<BoxCollider,SphereCollider>,
			Func<BoxCollider,AABBCollider>,
			Func<BoxCollider,BoxCollider>,
			Func<BoxCollider,CapsuleCollider>,
		},
		{
			Func<CapsuleCollider,SphereCollider>,
			Func<CapsuleCollider,AABBCollider>,
			Func<CapsuleCollider,BoxCollider>,
			Func<CapsuleCollider,CapsuleCollider>,
		},

	};

	//�R���C�_�[�̌^�ɑΉ�����Փ˔���֐������s
	const int shapeA = static_cast<int>(a.GetType());
	const int shapeB = static_cast<int>(b.GetType());

	return FuncList[shapeA][shapeB](a, b, cp);
}


//���W���X�V����
void SphereCollider::AddPosition(const vec3& translate)
{
	sphere.Center += translate;
}

//���W�ϊ������R���C�_�[���擾����
ColliderPtr SphereCollider::GetTransformedCollider(
	const vec3& translation,
	const mat3& matRotation,
	const vec3& scale)
{
	auto world = std::make_shared<SphereCollider>();

	//sphere�̔��a�ƒ��S����
	world->sphere.Center = translation + matRotation * (sphere.Center * scale);

											//scale�̂����A��ԑ傫�Ȓl���g�p����
	world->sphere.Radius = sphere.Radius * std::max({ scale.x,scale.y,scale.z });

	return world;
}

//���W���X�V����
void AABBCollider::AddPosition(const vec3& translate)
{
	aabb.min += translate;
	aabb.max += translate;
}

//���W�ϊ������R���C�_�[���擾����
ColliderPtr AABBCollider::GetTransformedCollider(
	const vec3& translation,
	const mat3& matRotation,
	const vec3& scale)
{
	auto world = std::make_shared<AABBCollider>();	//AABB�R���C�_�[���擾
	auto center = (aabb.max + aabb.min) * 0.5f;		//���S�����߂�

	center = translation + matRotation * (center * scale);

	auto s = (aabb.max - aabb.min) * 0.5f * scale;

	//scale�����炵���l�ōX�V
	world->aabb.max = center + s;
	world->aabb.min = center - s;

	return world;
}


//���W��ύX����

void BoxCollider::AddPosition(const vec3& translate)
{
	box.Center += translate;
}

//���W�ϊ������R���C�_�[���擾����
ColliderPtr BoxCollider::GetTransformedCollider(
	const vec3& translation,
	const mat3& matRotation,
	const vec3& scale)
{
	auto world = std::make_shared<BoxCollider>();
	world->box.Center = translation + matRotation * (box.Center * scale);

	//�ŗL������]������
	for (int i = 0; i < 3; i++)
	{
		world->box.Axis[i] = matRotation * box.Axis[i];
	}

	world->box.Scale = box.Scale * scale;

	return world;
}

//���W��ύX����
void CapsuleCollider::AddPosition(const vec3& translate)
{
	capsule.Seg.Start += translate;
	capsule.Seg.End += translate;
}

//���W�ϊ������R���C�_�[���擾����
ColliderPtr CapsuleCollider::GetTransformedCollider(
	const vec3& translation,
	const mat3& matRotation,
	const vec3& scale)
{
	auto world = std::make_shared<CapsuleCollider>();

	world->capsule.Seg.Start =
		translation + matRotation * (capsule.Seg.Start * scale);

	world->capsule.Seg.End =
		translation + matRotation * (capsule.Seg.End * scale);

	//���a�̃X�P�[���ɂ�XYZ�̂����ő�̂��̂�I��
	world->capsule.Radius =
		capsule.Radius * std::max({ scale.x,scale.y,scale.z });

	return world;
}
