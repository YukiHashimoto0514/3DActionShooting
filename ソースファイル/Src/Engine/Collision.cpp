//@file Collision.cpp

#include "Collision.h"
#include <algorithm>
#include <cmath>
#include <vector>

using namespace VecMath;


//�Փ˔���p�̍\���̂�֐����i�[���閼�O���
namespace Collision
{
	//�������O���
	namespace
	{
		//OBB�̒��_���W���擾����
		//@param box ���_���W�����߂�OBB

		//@return b0x�̒��_���W�z��

		//	  5----4
		//	 /|   /|
		//	1----0 |
		//	| 7--| -6
	    //	|/   |/
		//	3----2

		std::vector<vec3> GetVertices(const Box& box)
		{
			const vec3 ax = box.Axis[0] * box.Scale.x;
			const vec3 ay = box.Axis[1] * box.Scale.y;
			const vec3 az = box.Axis[2] * box.Scale.z;

			return
			{
				box.Center + ax + ay + az,	//0
				box.Center - ax + ay + az,	//1
				box.Center + ax - ay + az,	//2
				box.Center - ax - ay + az,	//3
				box.Center + ax + ay - az,	//4
				box.Center - ax + ay - az,	//5
				box.Center + ax - ay - az,	//6
				box.Center - ax - ay - az,	//7
			};
		}

		//OBB�̕ӂ��擾����
		//@param vertices ���_���W�����߂�OBB

		//@return box�̕ӂ̔z��
		//	  5----4
		//	 /|   /|
		//	1----0 |
		//	| 7--| -6
		//	|/   |/
		//	3----2

		std::vector<LineSegment> GetEdge(const vec3* vertices)
		{
			return {

				//X������
				{vertices[0],vertices[1]},
				{vertices[2],vertices[3]},
				{vertices[4],vertices[5]},
				{vertices[6],vertices[7]},

				//Y������
				{vertices[0],vertices[2]},
				{vertices[1],vertices[3]},
				{vertices[4],vertices[6]},
				{vertices[5],vertices[7]},

				//Z������
				{vertices[0],vertices[4]},
				{vertices[1],vertices[5]},
				{vertices[2],vertices[6]},
				{vertices[3],vertices[7]},
			};
		}

		//OBB�̖ʂ��擾����
		//@param box �ʂ����߂�OBB

		//@return box�̖ʂ̔z��

		std::vector<Plane> GetPlanes(const  Box& box)
		{
			const vec3 ax = box.Axis[0] * box.Scale.x;
			const vec3 ay = box.Axis[1] * box.Scale.y;
			const vec3 az = box.Axis[2] * box.Scale.z;

			return
			{
				{ box.Axis[0], dot(box.Axis[0],box.Center + ax)},	//+x
				{-box.Axis[0],-dot(box.Axis[0],box.Center - ax)},	//-x
				{ box.Axis[1], dot(box.Axis[1],box.Center + ay)},	//+y
				{-box.Axis[1],-dot(box.Axis[1],box.Center - ay)},	//-y
				{ box.Axis[2], dot(box.Axis[2],box.Center + az)},	//+z
				{-box.Axis[2],-dot(box.Axis[2],box.Center - az)},	//-z
			};
		}


	}

	//OBB�̒��_�����Ɏˉe���A�ő�l�ƍŏ��l�����߂�
	//@param vertices OBB�̒��_�z��
	//@param axis     �ˉe��̎��x�N�g��
	//@param min 	  �ŏ��l���i�[����ϐ�
	//@param max 	  �ő�l���i�[����ϐ�
	void ProjectBoxToAxis(const vec3* vertices, const vec3& axis,
		float& min, float& max)
	{
		//���ς��g�p���āA�ˉe����
		min = max = dot(axis, vertices[0]);

		for (int i = 0; i < 8; ++i)
		{
			float f = dot(axis, vertices[i]);

			min = std::min(min, f);
			max = std::max(max, f);
		}
	}

	//2��OBB�����x�N�g���Ɏˉe���A�������Ă���ђʋ��������߂�
	//@param verticesA  OBB���̂P�̒��_�z��
	//@param verticesB  OBB���̂Q�̒��_�z��
	//@param axis       �ˉe��̎��x�N�g��
	//@param shouldFlip �Փ˖@���̔��]�̗L�� 

	//@return �������Ă��鋗��(�v�Z�̓s���Ń}�C�i�X�̒l�ɂȂ��Ă邼)

	float GetIntersectionLength(const vec3* verticesA, const vec3* verticesB,
		const vec3& axis, bool& shouldFlip)
	{
		//2��OBB�����Ɏˉe
		float minA, maxA, minB, maxB;
		ProjectBoxToAxis(verticesA, axis, minA, maxA);
		ProjectBoxToAxis(verticesB, axis, minB, maxB);

		//�ˉe�����͈͂��������Ă��Ȃ��̂Ȃ�A�O��Ԃ�
		if (minB > maxA && minA > maxB)
		{
			return 0;
		}

		//�Փ˖@����OBB���̂P�����A���̂Q���E�ɂ���Ƒz�肵�Ă���̂ŁA
		//�������t�ɂȂ��Ă���ꍇ�A�Փ˂̌����𔽓]������K�v������
		shouldFlip = minB < minA;

		//�������Ă��鋗�����v�Z
		float lenA = maxA - minA;	//OBB���̂P�̒����𑪒�
		float lenB = maxB - minB;	//OBB���̂Q�̒����𑪒�
		float min = std::min(minA, minB);//�d�Ȃ��Ă���I�u�W�F�N�g�̍ŏ��l�����߂�
		float max = std::max(maxA, maxB);//�d�Ȃ��Ă���I�u�W�F�N�g�̍ő�l�����߂�

		float length = max - min;	//�d�Ȃ��Ă���I�u�W�F�N�g�̒��������߂�

		return length - lenA - lenB;
	}

	//�J�v�Z����OBB�����x�N�g���Ɏˉe���A�������Ă���͈͂����߂�
	//@param capsule    �J�v�Z��
	//@param vericesB   OBB�̒��_�z��
	//@param axis       �ˉe��̎��x�N�g��
	//@param shouldFlip �Փ˖@���̔��]�̗L��
	//@param minIndex   �ˉe�͈͂̍ŏ��l�ɑΉ����钸�_�z��C���f�b�N�X
	//@param maxIndex   �ˉe�͈͂̍ő�l�ɑΉ����钸�_�z��C���f�b�N�X

	//@return �������Ă��鋗��(�v�Z�̓s���Ń}�C�i�X�l�ɂȂ��Ă���)

	float GetIntersectionLength(const Capsule& capsule, const vec3* verticesB,
		const vec3& axis, bool& shouldFlip, int& minIndex, int& maxIndex)
	{
		//�J�v�Z�������Ɏˉe
		float minA, maxA;

		//�n�_�����x�N�g���Ɏˉe
		minA = maxA = dot(axis, capsule.Seg.Start);

		//�I�_�����x�N�g���Ɏˉe
		float f = dot(axis, capsule.Seg.End);

		//�ŏ��l�ƍő�l��ݒ�
		minA = std::min(minA, f) - capsule.Radius;
		maxA = std::max(maxA, f) + capsule.Radius;

		//OBB�����Ɏˉe
		minIndex = maxIndex = 0;
		float minB, maxB;
		minB = maxB = dot(axis, verticesB[0]);

		for (int i = 1; i < 8; ++i)
		{
			float f = dot(axis, verticesB[i]);

			//�ŏ��l�̃C���f�b�N�X���L�^
			if (f < minB)
			{
				minB = f;
				minIndex = i;	
			}

			//�ő�l�̃C���f�b�N�X���L�^
			if (f > maxB)
			{
				maxB = f;
				maxIndex = i;	
			}
		}

		//�������O�ȏ�Ȃ番�������ʂ����錄�Ԃ�����(�܂�������Ă��Ȃ�)
		if (minB > maxA && minA > maxB)
		{
			return false;
		}

		//�Փ˖@����OBB���̂P�����AOBB���̂Q���E�ɂ���Ƒz�肵�Ă���̂ŁA
		//�������t�ɂȂ��Ă���ꍇ�A�Փ˖@�{�̌����𔽓]����K�v������
		shouldFlip = minB < minA;

		//�������Ă��鋗�����v�Z
		float lenA = maxA - minA;
		float lenB = maxB - minB;

		//�ő�ŏ��l���v�Z
		float min = std::min(minA, minB);
		float max = std::max(maxA, maxB);

		//�������v�Z
		float length = max - min;

		return length - lenA - lenB;
	}

	//�_��OBB���ɂ��邩�ǂ����𒲂ׂ�
	//@param point ���ׂ�_�̍��W
	//@param box   ���ׂ�OBB

	//@retval true  point��box�̓����ɂ���
	//@retval false point��box�̊O���ɂ���

	bool IsPointInsideBox(const vec3& point, const Box& box)
	{
		const vec3 v = point - box.Center;

		for (int i = 0; i < 3; ++i)
		{
			const float dis = dot(v, box.Axis[i]);

			if (dis < -box.Scale[i] || dis>box.Scale[i])
			{
				return false;	//�_��OBB�̊O���ɂ���
			}
		}

		return true;	//�_��OBB�̓����ɂ���
	}

	//�����Ɩʂ̌�_�����߂�
	//@param seg   ����
	//@param plane ��
	//@param point ��_���i�[����ϐ�

	//@retval true  ��_�͐�����ɂ���
	//@retval false ��_�͐����̊O�ɂ���

	bool ClipLineSegment(const LineSegment& seg, const Plane& plane,
		vec3& point)
	{
		//�Ӄx�N�g����ʖ@���Ɏˉe
		const vec3 ab = seg.End - seg.Start;
		float n = dot(plane.Normal, ab);

		//���s�������ꍇ
		if (abs(n) < 0.001f)
		{
			//��_�Ȃ�
			return false;
		}

		//��_�܂ł̋��������߂�
		float t = plane.Distance - dot(plane.Normal, seg.Start);

		//�O���ɂ��邩�ǂ���
		if (t<0 || t>n)
		{
			//��_�Ȃ�
			return false;
		}

		//��_���W���v�Z
		t /= n;		//�x�N�g��ab�ɑ΂���䗦�ɕϊ�

		//�n�_�����_�܂ł̋����𑫂�
		point = seg.Start + ab * t;

		return true;
	}

	//OBB�̑S�Ă̕ӂɂ��āA�������OBB�̖ʂƂ̌�_�����߂�

	//@param vertices OBB���̂P�̒��_�z��
	//@param box      OBB���̂Q

	//return edge��box�̌�_
	std::vector<vec3> ClipEdges(const std::vector<vec3>& vertices, const Box& box)
	{
		//�ӂƖʂ��擾
		const auto Edges = GetEdge(vertices.data());	//��
		const auto Planes = GetPlanes(box);	//��

		//��_�p�̃�������\��
		//2��OBB�������[���������Ă��Ȃ�����A��_�̐��͕ӂ̐��ȉ��ɂȂ�͂�
		std::vector<vec3> result;
		result.reserve(Edges.size());

		//�ӂƖʂ̌�_�����߂�
		for (const auto& plane : Planes)
		{
			for (const auto& edge : Edges)
			{
				//��_�̍��W
				vec3 point;

				if (ClipLineSegment(edge, plane, point))
				{
					//��_��BB���̂Q�̓����ɂ���Ȃ�A�L���ȓ_�Ƃ��ċL�^
					if (IsPointInsideBox(point, box))
					{
						result.push_back(point);
					}
				}
			}
		}

		return result;
	}


	//AABB��̍ŋߐړ_�����߂�
	//@param aabb  �ŋߐړ_�����߂�AABB
	//@param point �ŋߐړ_�����߂���W

	//@return point����aaabb�ւ̍ŋߐړ_

	vec3 ClosestPoint(const AABB& aabb, const vec3& point)
	{

		//AABB�͈̔͂Ɏ��߂�
		return vec3(
			std::clamp(point.x, aabb.min.x, aabb.max.x),
			std::clamp(point.y, aabb.min.y, aabb.max.y),
			std::clamp(point.z, aabb.min.z, aabb.max.z));
	}

	//OBB��̍ŋߐړ_�����߂�
	//@param box   �ŋߐړ_�����߂�OBB
	//@param point �ŋߐړ_�����߂���W
	
	//@return point����box�ւ̍ŋߐړ_

	vec3 ClosestPoint(const Box& box, const vec3& point)
	{
		//OBB�̒��S���狅�̒��S�ւ̃x�N�g�������߂�
		vec3 v = point - box.Center;

		//OBB�̒��S�_����
		vec3 center = box.Center;

		for (int i = 0; i < 3; ++i)
		{
			float dis = dot(box.Axis[i], v);//�e���Ɏˉe

			//�ˉe����������OBB�̒����Ɏ��߂�
			dis = std::clamp(dis, -box.Scale[i], box.Scale[i]);

			//�ߐړ_�̈ʒu���ړ�����
			center += box.Axis[i] * dis;
		}

		//�ߐړ_��Ԃ�
		return center;
	}


	//������̍ŋߐړ_�����߂�
	//@param seg
	//@param point

	//@return point����seg�ւ̍ŋߐړ_

	vec3 ClosestPoint(const LineSegment& seg, const vec3& point)
	{
		//�_������Ɏˉe
		const vec3 dis = seg.End - seg.Start;
		const float t = dot(point - seg.Start, dis);

		//�����͈̔͂ɐ���
		if (t <= 0)
		{
			return seg.Start;
		}

		const float dis2 = dot(dis, dis);//�����̒�����2��
		//�����͈̔͂ɐ���
		if (t >= dis2)
		{
			return seg.End;
		}

		//�ŋߐړ_���v�Z���ĕԂ�
		return seg.Start + dis * (t / dis2);
	}

	//2�̐����̍ŋߐړ_�����߂�
	//@param segA�@�ŋߐړ_�����߂�������̂P
	//@param segB  �ŋߐړ_�����߂�������̂Q
	//@param denom segA��̍ŋߐړ_�̔䗦

	//@return segA��̍ŋߐړ_�̔䗦

	LineSegment ClosestPoint(const LineSegment& segA, const LineSegment& segB)
	{
		const vec3 d0 = segA.End - segA.Start;	//����A�̕����x�N�g��
		const vec3 d1 = segB.End - segB.Start;	//����B�̕����x�N�g��
		const vec3 r = segA.Start - segB.Start;	//����A�������B�̕����x�N�g��

		const float a = dot(d0, d0);	//����A�̒�����2��
		const float b = dot(d0, d1);	//����A�Ɛ���B�̓���
		const float c = dot(d0, r);
		const float e = dot(d1, d1);	//����B�̒�����2��
		const float f = dot(d1, r);

		//�����̒������O�Ƃ�������P�[�X����������
		//�������O�̐�����_�Ƃ݂Ȃ��Čv�Z����

		//����A,B�����̒������O
		if (a < 0.0001f && e < 0.0001f)
		{
			return { segA.Start,segB.Start };
		}

		//�����`�̒������O
		if (a < 0.0001f)
		{
			if (f <= 0)
			{
				return { segA.Start,segB.Start };
			}
			else if (f >= e)
			{
				return { segA.Start,segB.End };
			}
			return { segA.Start,segB.Start + d1 * (f / e) };
		}

		//�����a�̒������O
		if (e < 0.0001f)
		{
			if (-c <= 0)
			{
				return { segA.Start,segB.Start };
			}
			else if (-c >= a)
			{
				return { segA.End,segB.Start };
			}

			return { segA.Start + d0 * (-c / a),segB.Start };
		}

		//�����̒������O���傫���Ƃ킩������A���������s���ǂ������`�F�b�N����
		//���������s�ȏꍇ�A�������߂鎮�̕��ꂪ�O�ɂȂ�A�v�Z�ł��Ȃ�����
		const float denom = a * e - b * b;

		if (denom)
		{
			//�����`�̍ŋߐړ_�����߁A�����`�͈̔͂ɐ���
			float s = std::clamp((f * b - c * e) / denom, 0.0f, 1.0f);//�}��ϐ�
		
			//�����a�̍ŋߐړ_�����߂�
			float t = b * s + f;

			//t�������a�̊O�����w���Ă���ꍇ
			//t������a�̒[�_�ɐݒ肵�As���v�Z���Ȃ���
			LineSegment result;
			if (t < 0)
			{
				result.End = segB.Start;
				s = std::clamp(-c / a, 0.0f, 1.0f);
			}
			else if (t > e)
			{
				result.End = segB.End;
				s = std::clamp((b - c) / a, 0.0f, 1.0f);
			}
			else
			{
				result.End = segB.Start + d1 * (t / e);
			}

			result.Start = segA.Start + d0 * s;
			return result;
		}

		//���������s�ȏꍇ�A�����̏d������͈͂̒��_��I��

		//����B�����A�Ɏˉe
		float u = dot(d0, segB.Start - segA.Start);
		float v = dot(d0, segB.End - segA.Start);

		//�ˉe�������W�����A�͈̔͂ɐ���
		u = std::clamp(u, 0.0f, a);
		v = std::clamp(v, 0.0f, a);

		//u��v�̒��_������A�̍ŋߐړ_�ɂȂ�
		float s = (u + v) / a * 0.5f;
		LineSegment result;
		result.Start = segA.Start + d0 * s;

		//����A�̍ŋߐړ_�����B�Ɏˉe
		const float t = dot(d1, result.Start - segB.Start);

		//�ˉe�������W�����B�͈̔͂ɐ���
		if (t <= 0)
		{
			result.End = segB.Start;
		}
		else if (t >= e)
		{
			result.End = segB.End;
		}
		else
		{
			result.End = segB.Start + d1 * (t / e);
		}

		return result;
	}

	//3D�����`�Ɛ����̍ŋߐړ_�����߂�
	//@param rect �ŋߐړ_�����߂�3D�����`
	//@param seg  �ŋߐړ_�����߂����

	//@return seg��̍ŋߐړ_��rect��̍ŋߐړ_

	LineSegment ClosestPoint(const RectAngle& rect, const LineSegment& seg)
	{
		//�����̎n�_��ʂɎˉe
		const vec3 ca = seg.Start - rect.Center;
		const float a0 = dot(ca, rect.Axis[0]);
		const float a1 = dot(ca, rect.Axis[1]);

		//�����`�Ɛ����̕��s���𒲂ׂ�
		const vec3 ab = seg.End - seg.Start;
		const float n = dot(rect.Normal, ab);

		if (abs(n) <= 0.0001f)
		{
			//���s�ȏꍇ�A�����������`�̏�ɏ���Ă��邩�ǂ����𒲂ׂ�
			float tmin = -FLT_MAX;
			float tmax = FLT_MAX;

			const float list[] = { a0,a1 };

			for (int i = 0; i < 2; ++i)
			{
				//������axis[i]�Ƃ̕��s���𒲂ׂ�
				const float d = dot(ab, rect.Axis[i]);

				if (abs(d) <= 0.0001f)
				{
					//���s�Ȃ̂Ō�_���Ȃ�
					continue;
				}

				//�����`���S����̋���������̔䗦�Ƃ��ĕ\��
				float t0 = (-rect.scale[i] - list[i]) / d;
				float t1 = (rect.scale[i] - list[i]) / d;

				if (t0 > t1)
				{
					std::swap(t0, t1);
				}

				//�����`�ƌ������Ă���͈͂��v�Z
				tmin = std::max(t0, tmin);
				tmax = std::min(t1, tmax);
			}

			//�����̎n�_�ƏI�_�͈̔͂ɐ�������
			tmin = std::max(0.0f, tmin);
			tmax = std::min(1.0f, tmax);

			//�����������`�ɏd�Ȃ��Ă���ꍇ
			//�d���͈͂̒��S���ŋߐړ_�Ƃ���
			if (tmin <= tmax)
			{
				const vec3 p = seg.Start + ab * ((tmin + tmax) * 0.5f);
				const float d = dot(rect.Normal, p - rect.Center);

				return { p,p - rect.Normal * d };
			}
		}

		//�������ʂƌ������Ă��邩�`�F�b�N
		//�������Ă���ꍇ�A�ŋߐړ_�͌�_�ɂȂ�
		{
			//�����`�̒��S����@���ʂɑ΂��Ďˉe
			const float d = dot(rect.Normal, rect.Center);

			//�����`��������̎n�_�܂ł̋������v�Z
			const float t = (d - dot(rect.Normal, seg.Start)) / n;

			//�������Ă���ꍇ
			if (0 <= t && t <= 1)
			{
				//��_�������`�͈͓̔��ɂ��邩���ׂ�
				const vec3 p = seg.Start + ab * t;	//���������_�Ɍ������x�N�g��
				const vec3 v = p - rect.Center;		//�����`�̒��S�����_�Ɍ������x�N�g��
				const float dis1 = dot(v, rect.Axis[0]);

				if (abs(dis1) <= rect.scale[0])
				{
					const float dis2 = dot(v, rect.Axis[1]);

					if (abs(dis2) <= rect.scale[1])
					{
						return { p,p };
					}
				}
			}
		}

		//���s�łȂ��A���������Ă��Ȃ��ꍇ
		//�����̏I�_��ʂɎˉe
		const vec3 cb = seg.End - rect.Center;
		const float b0 = dot(cb, rect.Axis[0]);
		const float b1 = dot(cb, rect.Axis[1]);

		//�����̎n�_����яI�_�̍ŋߐړ_������
		//�����`�͈̔͂Ɋ܂܂��_�̂����A���߂��������̍ŋߐړ_�Ƃ���
		float distance = FLT_MAX;
		LineSegment Result;//= { vec3(0),vec3(0) };

		if (abs(a0) <= rect.scale[0] && abs(a1) <= rect.scale[1])
		{
			distance = dot(rect.Normal, ca);
			distance *= distance;//��r�̂��ߓ�悷��
			Result = { seg.Start,rect.Center + rect.Axis[0] * a0 + rect.Axis[1] * a1 };
		}

		if (abs(b0) <= rect.scale[0] && abs(b1) <= rect.scale[1])
		{
			float tmp = dot(rect.Normal, cb);
			tmp *= tmp;//��r�̂��ߓ��

			if (tmp < distance)
			{
				distance = tmp;
				Result = { seg.End,rect.Center + rect.Axis[0] * b0 + rect.Axis[1] * b1 };
			}
		}

		//�����`�̂S�ӂɂ��āA�ŋߐړ_���v�Z
		const vec3 v1 = rect.Axis[0] * rect.scale[0];
		const vec3 v2 = rect.Axis[1] * rect.scale[1];

		const vec3 v[] = {
			rect.Center + v1 + v2,
			rect.Center + v1 - v2,
			rect.Center - v1 - v2,
			rect.Center - v1 + v2 };
		
		for (int i = 0; i < 4; ++i)
		{
			//4���ӂɑ΂��čŋߐړ_�����߂�
			const auto s = ClosestPoint(seg, LineSegment{ v[i],v[(i + 1) % 4] });
			const vec3 ab = s.End - s.Start;
			const float distanceEdge = dot(ab, ab);//�����̂Q��

			if (distanceEdge < distance)
			{
				distance = distanceEdge;
				Result = s;
			}
		}

		return Result;
	}

	//���̂Ƌ��̂̏Փ˔���
	//@param s0 �Փ˔�����s�����̂���1
	//@param s1 �Փ˔�����s�����̂���2
	//@param cp �Փ˔�����i�[����R���^�N�g�|�C���g

	//@retval true  �Փ˂��Ă���
	//@retval false �Փ˂��Ă��Ȃ�

	bool Intersect(const Sphere& s0, const Sphere& s1, ContactPoint& cp)
	{
		//���S�Ԃ̋��������a���v���傫����΁A�Փ˂��Ă��Ȃ�
		const vec3 d = s0.Center - s1.Center;	//���S�Ԃ̋���
		const float d2 = dot(d, d);	//�傫��
		const float totalRadius = s0.Radius + s1.Radius;	//���v���a

		//�x�N�g���̑傫�����r
		if (d2 > totalRadius * totalRadius)
		{
			//�Փ˂��Ă��Ȃ�
			return false;
		}

		//�Փ˂��Ă���
		
		//�R���^�N�g�|�C���g�̐ݒ�
		//���̊Ԃ̋������ق�0�̏ꍇ�A�Փ˕�����������ɐݒ�
		if (d2 < 0.00001f)
		{
			cp.Normal = vec3(0, 1, 0);
			cp.Penetration = -totalRadius;
		}
		else
		{
			const float distance = std::sqrt(d2);
			cp.Normal = d * (1.0f / distance);			//�������擾
			cp.Penetration = distance - totalRadius;	//�ђʋ������v�Z
		}

		//�Փˈʒu�����̂̒��Ԃɐݒ�
		cp.Position = s0.Center - cp.Normal * (s0.Radius - cp.Penetration * 0.5f);
		return true;
	}

	//AABB��AABB�̏Փ˔���
	//@param a �Փ˔�����s��AABB����1
	//@param b �Փ˔�����s��AABB����2
	//@param cp �Փ˔�����i�[����R���^�N�g�|�C���g
	
	//@retval true  �Փ˂��Ă���
	//@retval false �Փ˂��Ă��Ȃ�

	bool Intersect(const AABB& a, const AABB& b, ContactPoint& cp)
	{
		
		int n = 0;	//�Փ˕���
		cp.Penetration = -FLT_MAX;	//�ђʋ������ő�ɂ��Ă���

		//�Փ˂������ǂ����𔻒�
		for (int i = 0; i < 3; ++i)
		{
			const float t0 = a.min[i] - b.max[i];

			if (t0 > 0)
			{
				//�Փ˂��Ă��Ȃ�
				return false;
			}

			//�ђʋ������Z���������Փ˕����Ƃ���
			if (t0 > cp.Penetration)
			{
				cp.Penetration = t0;
				n = i * 2;
			}

			const float t1 = b.min[i] - a.max[i];

			if (t1 > 0)
			{
				//�Փ˂��Ă��Ȃ�
				return false;
			}
			//�ђʋ������Z���������Փ˕����Ƃ���
			if (t1 > cp.Penetration)
			{
				cp.Penetration = t1;
				n = i * 2 + 1;
			}

		}

		//�Փ˕�����ݒ�
		static const vec3 normal[] =
		{
			{ 1, 0, 0},{ -1,  0,  0},
			{ 0, 1, 0},{  0, -1,  0},
			{ 0, 0, 1},{  0,  0, -1},
		};
		cp.Normal = normal[n];

		//�Փ˓_���v�Z
		for (int i = 0; i < 3; ++i)
		{
			cp.Position[i] =
				std::max(a.min[i], b.min[i]) + std::min(a.max[i], b.max[i]);
		}

		cp.Position *= 0.5f;
		return true;
	}

	//���̂�AABB�̏Փ˔���
	//@param sphere �Փ˔�����s������
	//@param aabb   �Փ˔�����s��AABB
	//@param cp     �Փˌ��ʂ��i�[����R���^�N�g�|�C���g
	
	//@retval true  �Փ˂��Ă���
	//@retval false �Փ˂��Ă��Ȃ�

	bool Intersect(const Sphere& sphere, const AABB& aabb, ContactPoint& cp)
	{
		//�ŋߐړ_���狅�̂̒��S�܂ł̋������A���̂̔��a���傫����΁A�Փ˂��Ă��Ȃ�
		const vec3 point = ClosestPoint(aabb, sphere.Center);//�ŋߐړ_
		const vec3 dis = sphere.Center - point;//�ŋߐړ_���狅�̂̒��S�ւ̃x�N�g��

		const float dis2 = dot(dis, dis);//�����̑傫��

		//�����̑傫�������a�����傫��
		if (dis2 > sphere.Radius * sphere.Radius)
		{
			//�Փ˂��Ă��Ȃ�
			return false;
		}

		//�R���^�N�g�|�C���g�̐ݒ�

		//�Փ˂�������������Ȃ��O�ɋ߂��ꍇ
		if (dis2 < 0.00001f)
		{
			//���̂̒��S��AABB�����ɂ���̂ŁA�ђʋ������ł��Z���ʂ���Փ˂����Ƃ݂Ȃ�

			int n = 0;	//�Փ˕���
			cp.Penetration = -FLT_MAX;	//�ђʋ������ő�ɂ��Ă���

			//�Փ˂������ǂ����𔻒�
			for (int i = 0; i < 3; ++i)
			{
				const float t0 = point[i] - aabb.max[i];

				//�ђʋ������Z���������Փ˕����Ƃ���
				if (t0 > cp.Penetration)
				{
					n = i * 2;
					cp.Penetration = t0;
				}

				const float t1 = aabb.min[i] - point[i];

				//�ђʋ������Z���������Փ˕����Ƃ���
				if (t1 > cp.Penetration)
				{
					n = i * 2 + 1;
					cp.Penetration = t1;
				}

			}

			//�Փ˕�����ݒ�
			static const vec3 normal[] =
			{
				{ 1, 0, 0},{ -1,  0,  0},
				{ 0, 1, 0},{  0, -1,  0},
				{ 0, 0, 1},{  0,  0, -1},
			};
			cp.Normal = normal[n];
		}
		else
		{
			//�������O���傫���ꍇ�A���̂̒��S��AABB�̊O���ɂ���̂ŁA
			//�ŋߐړ_���狅�̂̒��S�֌�������������Փ˂����Ƃ݂Ȃ�
			const float distance = std::sqrt(dis2);

			cp.Normal = dis * (1.0f / distance);		//�������擾
			cp.Penetration = distance - sphere.Radius;	//�ђʋ������v�Z
			
		}
		//�Փ˂��Ă���
		return true;
	}

	//AABB�Ƌ��̂̏Փ˔���
	//@param aabb   �Փ˔�����s��AABB
	//@param sphere �Փ˔�����s������
	//@param cp     �Փˌ��ʂ��i�[����R���^�N�g�|�C���g
	
	//@retval true  �Փ˂��Ă���
	//@retval false �Փ˂��Ă��Ȃ�

	bool Intersect(const AABB& aabb, const Sphere& sphere, ContactPoint& cp)
	{
		//�����̏��Ԃ��t�Ȃ����Ȃ̂ŁA�֐��𗬗p����
		if (Intersect(sphere, aabb, cp))
		{
			//�Փ˂̌������t�ɂ���
			cp.Normal = -cp.Normal;
			return true;
		}

		return false;
	}

	//���̂�OBB�̏Փ˔���
	//@param sphere   �Փ˔�����s������
	//@param box      �Փ˔�����s��OBB
	//@param cp       �Փˌ��ʂ��i�[����R���^�N�g�|�C���g
	
	//@retval true  �Փ˂��Ă���
	//@retval false �Փ˂��Ă��Ȃ�

	bool Intersect(const Sphere& sphere, const Box& box, ContactPoint& cp)
	{
		//�ŋߐړ_���狅�̂̒��S�܂ł̋������A���̂̔��a���傫����΁A�Փ˂��Ă��Ȃ�
		const vec3 point = ClosestPoint(box, sphere.Center);	//�ŋߐړ_
		const vec3 dis = sphere.Center - point;		//�ŋߐړ_���狅�̂̒��S�ւ̃x�N�g��

		const float dis2 = dot(dis, dis);//�����̑傫��

		//�����̑傫�������a�����傫��
		if (dis2 > sphere.Radius * sphere.Radius)
		{
			//�Փ˂��Ă��Ȃ�
			return false;
		}

		//�R���^�N�g�|�C���g�̐ݒ�

		//�Փ˂�������������Ȃ��O�ɋ߂��ꍇ
		if (dis2 < 0.00001f)
		{
			//���̂̒��S��AABB�����ɂ���̂ŁA�ђʋ������ł��Z���ʂ���Փ˂����Ƃ݂Ȃ�

			//OBB���S���狅�̒��S�ւ̃x�N�g��
			const vec3 a = point - box.Center;
			cp.Penetration = -FLT_MAX;		//�ђʋ������ő�ɂ��Ă���

			//�Փ˂������ǂ����𔻒�
			for (int i = 0; i < 3; ++i)
			{
				const float f = dot(box.Axis[i], a);	//�ŗL���֎ˉe
				const float t0 = f - box.Scale[i];

				//�ђʋ������Z���������Փ˕����Ƃ���
				if (t0 > cp.Penetration)
				{
					cp.Normal = box.Axis[i];
					cp.Penetration = t0;
				}

				const float t1 = -box.Scale[i] - f;

				//�ђʋ������Z���������Փ˕����Ƃ���
				if (t1 > cp.Penetration)
				{
					cp.Normal = -box.Axis[i];
					cp.Penetration = t1;
				}
			}

			//�ђʋ����ɋ��̔��a��������
			cp.Penetration -= sphere.Radius;
		}
		else
		{
			//�������O���傫���ꍇ�A���̂̒��S��AABB�̊O���ɂ���̂ŁA
			//�ŋߐړ_���狅�̂̒��S�֌�������������Փ˂����Ƃ݂Ȃ�
			const float distance = std::sqrt(dis2);

			cp.Normal = dis * (1.0f / distance);		//�������擾
			cp.Penetration = distance - sphere.Radius;	//�ђʋ������v�Z

		}
		cp.Position = point - cp.Normal * cp.Penetration * 0.5f;
		//�Փ˂��Ă���
		return true;
	}

	//OBB�Ƌ��̂̏Փ˔���
	//@param box      �Փ˔�����s��OBB
	//@param sphere   �Փ˔�����s������
	//@param cp       �Փˌ��ʂ��i�[����R���^�N�g�|�C���g
	
	//@retval true  �Փ˂��Ă���
	//@retval false �Փ˂��Ă��Ȃ�

	bool Intersect(const Box& box, const Sphere& sphere, ContactPoint& cp)
	{
		//�֐��𗬗p����
		if (Intersect(sphere, box, cp))
		{
			//�ђʕ����𔽑΂ɂ���
			cp.Normal = -cp.Normal;

			//�Փ˂��Ă���
			return true;
		}

		//�Փ˂��Ă��Ȃ�
		return false;
	}

	//���̂ƃJ�v�Z���̏Փ˔���
	//@param sphere   �Փ˔�����s������
	//@param capsule  �Փ˔�����s���J�v�Z��
	//@param cp       �Փˌ��ʂ��i�[����R���^�N�g�|�C���g

	//@retval true  �Փ˂��Ă���
	//@retval false �Փ˂��Ă��Ȃ�

	bool Intersect(const Sphere& sphere, const Capsule& capsule, ContactPoint& cp)
	{
		//�J�v�Z���̒��S�̐����Ƌ��̂̒��S�_�̋ߐړ_�����߂�
		const vec3 point = ClosestPoint(capsule.Seg, sphere.Center);

		//�ŋߐړ_�𒆐S�Ƃ��鋅�̂ƌ��̋��̂Ƃ̏Փ˔�������߂�
		return Intersect(sphere, Sphere{ point,capsule.Radius }, cp);
	}

	//�J�v�Z���Ƌ��̂̏Փ˔���
	//@param capsule  �Փ˔�����s���J�v�Z��
	//@param sphere   �Փ˔�����s������
	//@param cp       �Փˌ��ʂ��i�[����R���^�N�g�|�C���g
	
	//@retval true  �Փ˂��Ă���
	//@retval false �Փ˂��Ă��Ȃ�

	bool Intersect(const Capsule& capsule, const Sphere& sphere, ContactPoint& cp)
	{
		//�J�v�Z���̒��S�̐����Ƌ��̂̒��S�_�̋ߐړ_�����߂�
		const vec3 point = ClosestPoint(capsule.Seg, sphere.Center);

		//���̂ƍŋߐړ_�𒆐S�Ƃ��鋅�̂Ƃ̏Փ˔�������߂�
		return Intersect(Sphere{ point,capsule.Radius },sphere, cp);
	}

	//�J�v�Z���ƃJ�v�Z���̏Փ˔���
	//@param capsule  �Փ˔�����s���J�v�Z�����̂P
	//@param sphere   �Փ˔�����s���J�v�Z�����̂Q
	//@param cp       �Փˌ��ʂ��i�[����R���^�N�g�|�C���g

	//@retval true  �Փ˂��Ă���
	//@retval false �Փ˂��Ă��Ȃ�
	bool Intersect(const Capsule& capA, const Capsule& capB, ContactPoint& cp)
	{
		//�J�v�Z�����m�̒��S���̍ŋߐړ_�����߂�
		const LineSegment seg = ClosestPoint(capA.Seg, capB.Seg);

		//���߂������̌������擾
		const vec3 dis = seg.Start - seg.End;	

		//������2��
		const float dis2 = dot(dis, dis);

		//���v���a
		const float totalRadius = capA.Radius + capB.Radius;

		//�����̒��������a�̍��v�����傫��������
		if (dis2 > totalRadius * totalRadius)
		{
			return false;
		}

		//�ŋߐړ_���m�̋�����0���傫���ꍇ
		//���̓��m�Ɠ������@�ŃR���^�N�g�|�C���g���v�Z����
		if (dis2 >= 0.0001f)
		{
			const float distance = std::sqrt(dis2);

			cp.Normal = dis * (1.0f / distance);		//�Փ˕���
			cp.Penetration = distance - totalRadius;	//�Փˋ���
			cp.Position = seg.Start - cp.Normal * (capA.Radius - cp.Penetration * 0.5f);

			return true;
		}

		//�ŋߐړ_���m�̋������O�̏ꍇ
		//seg����@�����v�Z�ł��Ȃ��̂ŁA2���ɐ����ȕ������Փ˖@���Ƃ���

		//���ꂼ��̃x�N�g�����擾
		const vec3 d0 = capA.Seg.End - capA.Seg.Start;
		const vec3 d1 = capB.Seg.End - capB.Seg.Start;

		//�O�ςŐ����������擾
		cp.Normal = cross(d0, d1);

		//�����@���̒�����0�̏ꍇ�A2���͕��s
		if (dot(cp.Normal, cp.Normal) < 0.00001f)
		{
			//�@���x�N�g�������Ȃ��̂ŁAd0�x�N�g���ɐ����ȕ������Փ˖@���Ƃ���
			if (d0.x)
			{
				cp.Normal = { -(d0.y + d0.z) / d0.x,1,1 };
			}
			else if (d0.y)
			{
				cp.Normal = { 1,-(d0.x + d0.z) / d0.y,1 };
			}
			else
			{
				cp.Normal = { 1,1,-(d0.x + d0.y) / d0.z };
			}
		}

		//�傫�����P�ɂ���
		cp.Normal = normalize(cp.Normal);

		//�傫�����̔��a���ђʋ����Ƃ���
		cp.Penetration = -std::max(capA.Radius, capB.Radius);

		//�R���^�N�g�|�C���g�̍��W���v�Z
		cp.Position = seg.Start - cp.Normal * (capA.Radius - cp.Penetration * 0.5f);
		return true;
	}


	//OBB��OBB�̏Փ˔���
	//@param boxA      �Փ˔�����s��OBB���̂P
	//@param boxB	   �Փ˔�����s��OBB���̂Q
	//@param cp        �Փˌ��ʂ��i�[����R���^�N�g�|�C���g
	
	//@retval true  �Փ˂��Ă���
	//@retval false �Փ˂��Ă��Ȃ�

	bool Intersect(const Box& boxA, const Box& boxB, ContactPoint& cp)
	{
		//OBB�̒��_���W�z����擾
		const auto verticesA = GetVertices(boxA);
		const auto verticesB = GetVertices(boxB);

		//�ђʋ������ő�ɂ��Ă���
		cp.Penetration = -FLT_MAX;

		//����������p�̎��x�N�g�����v�Z
		vec3 axisList[15] = {
			boxA.Axis[0],boxA.Axis[1],boxA.Axis[2],
			-boxB.Axis[0],-boxB.Axis[1],-boxB.Axis[2],
		};

		for (int i = 0; i < 3; ++i)
		{
			//boxA��boxB�̊O�ς��v�Z����
			axisList[6 + i * 3 + 0] = cross(axisList[i], axisList[3]);
			axisList[6 + i * 3 + 1] = cross(axisList[i], axisList[4]);
			axisList[6 + i * 3 + 2] = cross(axisList[i], axisList[5]);
		}

		//���ׂĂ̎��x�N�g���ɑ΂��ĕ�������������s
		for (int i = 0; i < 15; ++i)
		{
			//�O�ςɂ���č��ꂽ���x�N�g���̏ꍇ�A���̎������s���ƁA�������O�ɂȂ�
			//���̏ꍇ�A�`�܂��͂a�̎��ɂ�镪��������Ɠ������̂ŁA�X�L�b�v
			const float Lengh2 = dot(axisList[i], axisList[i]);
			if (Lengh2 < 0.001f)
			{
				continue;
			}

			//�������Ă��鋗�����v�Z
			bool shouldFlip;
			float dis = GetIntersectionLength(
				verticesA.data(), verticesB.data(), axisList[i], shouldFlip);

			//������0�ȏ�Ȃ番�������ʂ����錄�Ԃ�����(�܂�������Ă��Ȃ�)
			if (dis >= 0)
			{
				//�Փ˂��Ă��Ȃ�
				return false;
			}

			//�O�ʂō쐬�������x�N�g���̒����́A�P�ɂȂ�Ȃ�(sin�ƂȂ̂�)
			//���ʂƂ��āAdis�͎��x�N�g���̒�����P�ʂƂ����l�ƂȂ�̂�
			//�������r���邽�߂ɁA���K�����A�P�ʂ����낦��
			dis /= std::sqrt(Lengh2);

			//�Փˋ��������Z����������Փ˂����Ƃ݂Ȃ�
			if (dis > cp.Penetration)
			{
				//�ђʋ�����ݒ�
				cp.Penetration = dis;

				//�Փ˖@���͎��x�N�g���̋t����
				if (shouldFlip)
				{
					cp.Normal = axisList[i];
				}
				else
				{
					cp.Normal = -axisList[i];
				}
			}
		}

		//�Փ˖@���𐳋K��
		cp.Normal = normalize(cp.Normal);


		//�R���^�N�g�|�C���g���v�Z

		//OBB��OBB�̌�_�����߂�
		std::vector<vec3> c0 = ClipEdges(verticesB, boxA);	//�ӂƖʂ̌�_�����߂�
		std::vector<vec3> c1 = ClipEdges(verticesA, boxB);	//�ӂƖʂ̌�_�����߂�

		//c0�Ɍ�_���W�߂�
		c0.insert(c0.end(), c1.begin(), c1.end());

		//�ђʋ����̒��ԂɏՓ˕��ʂ�ݒ�(�Փ˕��ʏ�̓_�������߂�)
		float min, max;
		ProjectBoxToAxis(verticesA.data(), cp.Normal, min, max);	//min��max��ݒ�

		const vec3 p = boxA.Center - cp.Normal * (max - min + cp.Penetration) * 0.5f;

		//��_���Փ˕��ʂɎˉe���A�d�������_���폜
		int count = 0;
		for (int i = 0; i < c0.size(); ++i, ++count)
		{
			//��_���Փ˕��ʂɎˉe
			c0[count] = c0[i] + cp.Normal * dot(cp.Normal, p - c0[i]);

			//�d�������_������ꍇ�͍폜
			for (int j = 0; j < count; ++j)
			{
				const vec3 v = c0[j] - c0[count];

				if (dot(v, v) < 0.001f)
				{
					--count;

					break;
				}
			}
		}

		c0.resize(count);

		//��_�̏d�S���v�Z���A�R���^�N�g�|�C���g�Ƃ���
		cp.Position = vec3(0);

		for (const auto& e : c0)
		{
			cp.Position += e;
		}
		cp.Position /= static_cast<float>(c0.size());

		//�Փ˂��Ă���
		return true;
	}


	//OBB��AABB�̏Փ˔���

	//@param box  �Փ˔�����s��OBB
	//@param aabb �Փ˔�����s��AABB
	//@param cp   �Փˌ��ʂ��i�[����R���^�N�g�|�C���g

	//@retval true  �Փ˂��Ă���
	//@retval false �Փ˂��Ă��Ȃ�

	bool Intersect(const Box& box, const AABB& aabb, ContactPoint& cp)
	{
		//AABB����]���Ă��Ȃ�Box�Ƃ��Ĉ���
		Box boxB;
		boxB.Center = (aabb.min + aabb.max) * 0.5f;
		boxB.Scale = (aabb.max - aabb.min) * 0.5f;

		return Intersect(box, boxB, cp);
	}

	//AABB��OBB�̏Փ˔���

	//@param aabb �Փ˔�����s��AABB
	//@param box  �Փ˔�����s��OBB
	//@param cp   �Փˌ��ʂ��i�[����R���^�N�g�|�C���g
	
	//@retval true  �Փ˂��Ă���
	//@retval false �Փ˂��Ă��Ȃ�

	bool Intersect(const AABB& aabb, const Box& box, ContactPoint& cp)
	{
		//AABB����]���Ă��Ȃ�Box�Ƃ��Ĉ���
		Box boxA;
		boxA.Center = (aabb.min + aabb.max) * 0.5f;
		boxA.Scale = (aabb.max - aabb.min) * 0.5f;

		return Intersect(boxA, box, cp);

	}

	//�J�v�Z����OBB�̏Փ˔���

	//@param capsule �Փ˔�����s���J�v�Z��
	//@param box  �Փ˔�����s��OBB
	//@param cp   �Փˌ��ʂ��i�[����R���^�N�g�|�C���g
	
	//@retval true  �Փ˂��Ă���
	//@retval false �Փ˂��Ă��Ȃ�

	bool Intersect(const Capsule& capsule, const Box& box, ContactPoint& cp)
	{
		//OBB�̒��_�z����擾
		const std::vector<vec3> verticesA = { capsule.Seg.Start,capsule.Seg.End };
		const auto verticesB = GetVertices(box);

		//����������p�̎��x�N�g�����v�Z
		const vec3 capsuleDirection = capsule.Seg.End - capsule.Seg.Start;
		const vec3 axis0 = box.Axis[0] * box.Scale[0];
		const vec3 axis1 = box.Axis[1] * box.Scale[1];
		const vec3 axis2 = box.Axis[2] * box.Scale[2];

		const vec3 axisList[] = {
			box.Axis[0],box.Axis[1],box.Axis[2],			//0,1,2			cross(box.Axis[0],capsuleDirection),//
			cross(box.Axis[0],capsuleDirection),			//3
			cross(box.Axis[1],capsuleDirection),			//4
			cross(box.Axis[2],capsuleDirection),			//5
			cross(axis0 + axis1 + axis2,capsuleDirection),	//6
			cross(axis0 + axis1 - axis2,capsuleDirection),	//7
			cross(axis0 - axis1 + axis2,capsuleDirection),	//8
			cross(axis0 - axis1 - axis2,capsuleDirection),	//9
		};
		

		//�ђʋ������ő�ɂ��Ă���
		cp.Penetration = -FLT_MAX;

		//�S�Ă̎��x�N�g���ɑ΂��ĕ�������������s
		int axisIndex = 0;//�Փ˂����������̃C���f�b�N�X
		int minIndex = 0, maxIndex = 0;

		for (int i = 0; i < std::size(axisList); ++i)
		{
			//�O�ςɂ���č��ꂽ���x�N�g���̏ꍇ�A���̎������s���ƒ������O�ɂȂ�
			//���̏ꍇA�܂���B�̎��ɂ�镪��������Ɠ������̂ŃX�L�b�v����
			const float length2 = dot(axisList[i], axisList[i]);

			if (length2 < 0.0001f)
			{
				continue;
			}

			//�������𐳋K��
			const vec3 axis = axisList[i] * (1.0f / sqrt(length2));

			//�������Ă��鋗�����v�Z
			bool shouldFlip;
			int tmpMinIndex, tmpMaxIndex;
			const float d = GetIntersectionLength(capsule, verticesB.data(),
				axis, shouldFlip, tmpMinIndex, tmpMaxIndex);

			//�������O�ȏ�Ȃ番�������ʂ����錄�Ԃ�����(�������Ă��Ȃ�)
			if (d >= 0)
			{
				return false;
			}

			//�ђʋ��������Z����������Փ˂����Ƃ݂Ȃ�
			if (d > cp.Penetration)
			{
				cp.Penetration = d;

				//�Փ˖@���͎��x�N�g���̋t����
				if (shouldFlip)
				{
					cp.Normal = axis;
				}
				else
				{
					cp.Normal = -axis;
				}
				minIndex = tmpMinIndex;
				maxIndex = tmpMaxIndex;
				axisIndex = i;
			}
		}

		//���������̌������m�F
		const vec3 list[] = { capsule.Seg.Start,capsule.Seg.End };
		float direction = 1;//�J�v�Z�����̌��������p

		for (const auto& e : list)
		{
			//OBB�̍ŋߐړ_���������ɂȂ��ꍇ�͖���
			const vec3 c = ClosestPoint(box, e);
			const vec3 v = c - e;
			const float p = dot(capsuleDirection, v) * direction;

			direction *= -1;

			if (p >= 0)
			{
				continue;
			}

			//�ŋߐړ_�܂ł̋������J�v�Z���̔��a���傫���ꍇ�͖���
			float t = dot(v, v);	//�����̂Q��
			
			if (t > capsule.Radius * capsule.Radius)
			{
				continue;
			}

			//�ŋߐړ_�܂ł̋������������ɂ�鋗�����Z���ꍇ�̂ݔ����Ƃ̏Փ˂�I��
			t = sqrt(t);	//2��̏�Ԃ�����
			const float d = t - capsule.Radius;

			if (d > cp.Penetration)
			{
				cp.Penetration = d;
				cp.Normal = -v * (1.0f / t);
				cp.Position = c + cp.Normal * (cp.Penetration * 0.5f);
				axisIndex = -1;
			}
		}

		//�R���^�N�g�|�C���g���v�Z
		//�������ɂ���ďꍇ����
		switch (axisIndex)
		{
			case 0:
			case 1:
			case 2:
			{
				//�ʂŏՓ�
				//�����̎n�_����сA�I�_�ɂ��Ėʏ�̍ŋߐړ_���v�Z
				//�ŋߐړ_��OBB�͈͓̔��ɂ���ꍇ�̂ݗL��
				const int axisIndex1 = (axisIndex + 1) % 3;
				const int axisIndex2 = (axisIndex + 2) % 3;

				const RectAngle rect = {
					box.Center + cp.Normal * box.Scale[axisIndex],
					cp.Normal,
					box.Axis[axisIndex1],box.Axis[axisIndex2],
					box.Scale[axisIndex1],box.Scale[axisIndex2] };

				const auto seg = ClosestPoint(rect, capsule.Seg);

				//���������Z�O�����g���O�ł͖���������
				if (dot(seg.Start - seg.End, seg.Start - seg.End) >= 0.0001f)
				{
					cp.Normal = normalize(seg.Start - seg.End);
				}
				cp.Position = seg.End + cp.Normal * (cp.Penetration * 0.5f);

				break;
			}

			case 3:
			case 4:
			case 5:
			{
				//�ӂŏՓ�
				//   5----4
				//  /|   /|
				// 1----0 |
				// | 7--|-6
				// |/   |/
				// 3----2

				//�ړI�̕ӂ����o�����߂̒��_��f�����I������
				static const int PairList[][8] = {
					//0, 1, 2, 3, 4, 5, 6, 7
					{ 1, 0, 3, 2, 5, 4, 7, 6},
					{ 2, 3, 0, 1, 6, 7, 4, 5},
					{ 4, 5, 6, 7, 0, 1, 2, 3},
				};

				int index0 = 0;
				const float a = dot(cp.Normal, capsule.Seg.Start - verticesB[minIndex]);
				const float b = dot(cp.Normal, capsule.Seg.Start - verticesB[maxIndex]);
				
				if (a < b)
				{
					index0 = minIndex;
				}
				else
				{
					index0 = maxIndex;
				}

				const int index1 = PairList[axisIndex - 3][index0];
				const auto seg = ClosestPoint(capsule.Seg,
					LineSegment{ verticesB[index0],verticesB[index1] });

				cp.Position = seg.End + cp.Normal * (cp.Penetration * 0.5f);
				break;

			}

			case 6:
			case 7:
			case 8:
			case 9:
			{
				//���_�ŏՓ�
				const float a = dot(cp.Normal, capsule.Seg.Start - verticesB[minIndex]);
				const float b = dot(cp.Normal, capsule.Seg.Start - verticesB[maxIndex]);

				if (a < b)
				{
					cp.Position = verticesB[minIndex] + cp.Normal * (cp.Penetration * 0.5f);
				}
				else
				{
					cp.Position = verticesB[maxIndex] + cp.Normal * (cp.Penetration * 0.5f);
				}
				break;
			}
		}

		return true;
	}

	//OBB�ƃJ�v�Z���̏Փ˔���

	//@param box      �Փ˔�����s��OBB
	//@param capsule  �Փ˔�����s���J�v�Z��
	//@param cp       �Փˌ��ʂ��i�[����R���^�N�g�|�C���g
	
	//@retval true  �Փ˂��Ă���
	//@retval false �Փ˂��Ă��Ȃ�

	bool Intersect(const Box& box, const Capsule& capsule, ContactPoint& cp)
	{
		//�����̏��Ԃ��Ⴄ�����Ȃ̂ŁA���̂܂ܗ��p
		if (Intersect(capsule, box, cp))
		{
			//�Փ˂̌������t�ɂ���
			cp.Normal = -cp.Normal;

			return true;
		}

		return false;
	}

	//�J�v�Z����AABB�̏Փ˔���

	//@param capsule  �Փ˔�����s���J�v�Z��
	//@param AABB     �Փ˔�����s��AABB
	//@param cp       �Փˌ��ʂ��i�[����R���^�N�g�|�C���g
	
	//@retval true  �Փ˂��Ă���
	//@retval false �Փ˂��Ă��Ȃ�

	bool Intersect(const Capsule& capsule, const AABB& aabb, ContactPoint& cp)
	{
		//AABB��BOX�ɕϊ����ē�������Ƃ�
		Box box;
		box.Center = (aabb.min + aabb.max) * 0.5f;
		box.Scale = (aabb.max - aabb.min) * 0.5f;


		return Intersect(capsule, box, cp);
	}

	//�J�v�Z����AABB�̏Փ˔���

	//@param capsule  �Փ˔�����s���J�v�Z��
	//@param AABB     �Փ˔�����s��AABB
	//@param cp       �Փˌ��ʂ��i�[����R���^�N�g�|�C���g
	
	//@retval true  �Փ˂��Ă���
	//@retval false �Փ˂��Ă��Ȃ�

	bool Intersect(const AABB& aabb, const Capsule& capsule, ContactPoint& cp)
	{
		if (Intersect(capsule, aabb, cp))
		{
			cp.Normal = -cp.Normal;

			return true;
		}


		return false;
	}

}