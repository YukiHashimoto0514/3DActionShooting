//@file Collision.h

#ifndef COLLISION_H_INCLUDED
#define COLLISION_H_INCLUDED
#include "VecMath.h"

namespace Collision
{
	//����
	struct LineSegment
	{
		VecMath::vec3 Start;	//�����̎n�_
		VecMath::vec3 End;		//�����̏I�_
	};

	//����
	struct Plane
	{
		VecMath::vec3 Normal;	//�ʂ̖@��
		float Distance;			//���_����̋���
	};

	//3D�����`
	struct RectAngle
	{
		VecMath::vec3 Center;	//�����`�̒��S���W
		VecMath::vec3 Normal;	//�ʂ̖@��
		VecMath::vec3 Axis[2];	//�����`�̎�
		float scale[2];			//�������̑傫��
	};

	//�����s���E�{�b�N�X
	struct AABB	//Axis Aligned Bounding Box �������Ⴆ�Β�����
	{
		VecMath::vec3 min = { 0,0,0 };
		VecMath::vec3 max = { 0,0,0 };
	};

	//�L�����E�{�b�N�X
	struct Box	//OBB(oriented bounding box)
	{
		VecMath::vec3 Center = { 0,0,0 };	//���S�̍��W
		VecMath::vec3 Axis[3] =
			{ {1,0,0},{0,1,0},{0,0,1} };	//�ŗL��

		VecMath::vec3 Scale = { 1,1,1 };	//�e�������̑傫��
	};

	//����
	struct Sphere
	{
		VecMath::vec3 Center;	//���S���W
		float Radius;		//���a
	};

	//�J�v�Z��
	struct Capsule
	{
		LineSegment Seg;	//���S�̐���
		float Radius;		//���a
	};

	//�Փˏ���ێ�����\����
	struct ContactPoint
	{
		VecMath::vec3 Position;	//�Փˍ��W
		VecMath::vec3 Normal;	//�Փ˕���(����B���畨��A�Ɍ���������)
		float Penetration;		//�ђʋ���(�}�C�i�X�̏ꍇ�Փ˂��Ă���)
	};

	//AABB�̍ŋߐړ_�����߂�
	VecMath::vec3 ClosestPoint(const AABB& aabb, const VecMath::vec3& p);
	VecMath::vec3 ClosestPoint(const Box& box, const VecMath::vec3& p);
	VecMath::vec3 ClosestPoint(const LineSegment& seg, const VecMath::vec3& p);
	LineSegment ClosestPoint(const LineSegment& segA, const LineSegment& segB);
	LineSegment ClosestPoint(const RectAngle& rect, const LineSegment& segB);

	//�Փ˂��Ă��邩�ǂ���(true=�Փ˂��Ă���@false=�Փ˂��Ă��Ȃ�)
	bool Intersect(const Sphere& s0, const Sphere& s1, ContactPoint& cp);		//���̓��m
	bool Intersect(const AABB& a, const AABB& b, ContactPoint& cp);				//AABB���m
	bool Intersect(const Sphere& sphere, const AABB& aabb, ContactPoint& cp);	//���̂�AABB
	bool Intersect(const AABB& aabb, const Sphere& sphere, ContactPoint& cp);	//AABB�Ƌ���
	bool Intersect(const Sphere& sphere, const Box& box, ContactPoint& cp);		//���̂�AABB
	bool Intersect(const Box& box, const Sphere& sphere, ContactPoint& cp);		//OBB�Ƌ���
	bool Intersect(const Box& boxA, const Box& boxB, ContactPoint& cp);			//OBB��OBB
	bool Intersect(const Box& box, const AABB& aabb, ContactPoint& cp);			//OBB�Ƌ���
	bool Intersect(const AABB& aabb, const Box& box, ContactPoint& cp);			//OBB��OBB
	bool Intersect(const Sphere& sphere, const Capsule& capsule, ContactPoint& cp);		//���̂ƃJ�v�Z��
	bool Intersect(const Capsule& capsule, const Sphere& sphere, ContactPoint& cp);		//�J�v�Z���Ƌ���
	bool Intersect(const Capsule& capA, const Capsule& capB, ContactPoint& cp);			//�J�v�Z���ƃJ�v�Z��
	bool Intersect(const Capsule& capsule, const Box& box, ContactPoint& cp);			//�J�v�Z����OBB
	bool Intersect(const Box& box, const Capsule& capsule, ContactPoint& cp);			//OBB�ƃJ�v�Z��
	bool Intersect(const Capsule& capsule, const AABB& aabb, ContactPoint& cp);			//�J�v�Z����AABB
	bool Intersect(const AABB& aabb, const Capsule& capsule, ContactPoint& cp);			//AABB�ƃJ�v�Z��
}


#endif // !COLLISION_H_INCLUDED

