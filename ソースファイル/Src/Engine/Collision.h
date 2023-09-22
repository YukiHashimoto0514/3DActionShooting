//@file Collision.h

#ifndef COLLISION_H_INCLUDED
#define COLLISION_H_INCLUDED
#include "VecMath.h"

namespace Collision
{
	//線分
	struct LineSegment
	{
		VecMath::vec3 Start;	//線分の始点
		VecMath::vec3 End;		//線分の終点
	};

	//平面
	struct Plane
	{
		VecMath::vec3 Normal;	//面の法線
		float Distance;			//原点からの距離
	};

	//3D長方形
	struct RectAngle
	{
		VecMath::vec3 Center;	//長方形の中心座標
		VecMath::vec3 Normal;	//面の法線
		VecMath::vec3 Axis[2];	//長方形の軸
		float scale[2];			//軸方向の大きさ
	};

	//軸平行境界ボックス
	struct AABB	//Axis Aligned Bounding Box 言っちゃえば直方体
	{
		VecMath::vec3 min = { 0,0,0 };
		VecMath::vec3 max = { 0,0,0 };
	};

	//有向境界ボックス
	struct Box	//OBB(oriented bounding box)
	{
		VecMath::vec3 Center = { 0,0,0 };	//中心の座標
		VecMath::vec3 Axis[3] =
			{ {1,0,0},{0,1,0},{0,0,1} };	//固有軸

		VecMath::vec3 Scale = { 1,1,1 };	//各軸方向の大きさ
	};

	//球体
	struct Sphere
	{
		VecMath::vec3 Center;	//中心座標
		float Radius;		//半径
	};

	//カプセル
	struct Capsule
	{
		LineSegment Seg;	//中心の線分
		float Radius;		//半径
	};

	//衝突情報を保持する構造体
	struct ContactPoint
	{
		VecMath::vec3 Position;	//衝突座標
		VecMath::vec3 Normal;	//衝突方向(物体Bから物体Aに向かう方向)
		float Penetration;		//貫通距離(マイナスの場合衝突している)
	};

	//AABBの最近接点を求める
	VecMath::vec3 ClosestPoint(const AABB& aabb, const VecMath::vec3& p);
	VecMath::vec3 ClosestPoint(const Box& box, const VecMath::vec3& p);
	VecMath::vec3 ClosestPoint(const LineSegment& seg, const VecMath::vec3& p);
	LineSegment ClosestPoint(const LineSegment& segA, const LineSegment& segB);
	LineSegment ClosestPoint(const RectAngle& rect, const LineSegment& segB);

	//衝突しているかどうか(true=衝突している　false=衝突していない)
	bool Intersect(const Sphere& s0, const Sphere& s1, ContactPoint& cp);		//球体同士
	bool Intersect(const AABB& a, const AABB& b, ContactPoint& cp);				//AABB同士
	bool Intersect(const Sphere& sphere, const AABB& aabb, ContactPoint& cp);	//球体とAABB
	bool Intersect(const AABB& aabb, const Sphere& sphere, ContactPoint& cp);	//AABBと球体
	bool Intersect(const Sphere& sphere, const Box& box, ContactPoint& cp);		//球体とAABB
	bool Intersect(const Box& box, const Sphere& sphere, ContactPoint& cp);		//OBBと球体
	bool Intersect(const Box& boxA, const Box& boxB, ContactPoint& cp);			//OBBとOBB
	bool Intersect(const Box& box, const AABB& aabb, ContactPoint& cp);			//OBBと球体
	bool Intersect(const AABB& aabb, const Box& box, ContactPoint& cp);			//OBBとOBB
	bool Intersect(const Sphere& sphere, const Capsule& capsule, ContactPoint& cp);		//球体とカプセル
	bool Intersect(const Capsule& capsule, const Sphere& sphere, ContactPoint& cp);		//カプセルと球体
	bool Intersect(const Capsule& capA, const Capsule& capB, ContactPoint& cp);			//カプセルとカプセル
	bool Intersect(const Capsule& capsule, const Box& box, ContactPoint& cp);			//カプセルとOBB
	bool Intersect(const Box& box, const Capsule& capsule, ContactPoint& cp);			//OBBとカプセル
	bool Intersect(const Capsule& capsule, const AABB& aabb, ContactPoint& cp);			//カプセルとAABB
	bool Intersect(const AABB& aabb, const Capsule& capsule, ContactPoint& cp);			//AABBとカプセル
}


#endif // !COLLISION_H_INCLUDED

