//@file Collision.cpp

#include "Collision.h"
#include <algorithm>
#include <cmath>
#include <vector>

using namespace VecMath;


//衝突判定用の構造体や関数を格納する名前空間
namespace Collision
{
	//無名名前空間
	namespace
	{
		//OBBの頂点座標を取得する
		//@param box 頂点座標を求めるOBB

		//@return b0xの頂点座標配列

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

		//OBBの辺を取得する
		//@param vertices 頂点座標を求めるOBB

		//@return boxの辺の配列
		//	  5----4
		//	 /|   /|
		//	1----0 |
		//	| 7--| -6
		//	|/   |/
		//	3----2

		std::vector<LineSegment> GetEdge(const vec3* vertices)
		{
			return {

				//X軸方向
				{vertices[0],vertices[1]},
				{vertices[2],vertices[3]},
				{vertices[4],vertices[5]},
				{vertices[6],vertices[7]},

				//Y軸方向
				{vertices[0],vertices[2]},
				{vertices[1],vertices[3]},
				{vertices[4],vertices[6]},
				{vertices[5],vertices[7]},

				//Z軸方向
				{vertices[0],vertices[4]},
				{vertices[1],vertices[5]},
				{vertices[2],vertices[6]},
				{vertices[3],vertices[7]},
			};
		}

		//OBBの面を取得する
		//@param box 面を求めるOBB

		//@return boxの面の配列

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

	//OBBの頂点を軸に射影し、最大値と最小値を求める
	//@param vertices OBBの頂点配列
	//@param axis     射影先の軸ベクトル
	//@param min 	  最小値を格納する変数
	//@param max 	  最大値を格納する変数
	void ProjectBoxToAxis(const vec3* vertices, const vec3& axis,
		float& min, float& max)
	{
		//内積を使用して、射影する
		min = max = dot(axis, vertices[0]);

		for (int i = 0; i < 8; ++i)
		{
			float f = dot(axis, vertices[i]);

			min = std::min(min, f);
			max = std::max(max, f);
		}
	}

	//2つのOBBを軸ベクトルに射影し、交差している貫通距離を求める
	//@param verticesA  OBBその１の頂点配列
	//@param verticesB  OBBその２の頂点配列
	//@param axis       射影先の軸ベクトル
	//@param shouldFlip 衝突法線の反転の有無 

	//@return 交差している距離(計算の都合でマイナスの値になってるぞ)

	float GetIntersectionLength(const vec3* verticesA, const vec3* verticesB,
		const vec3& axis, bool& shouldFlip)
	{
		//2つのOBBを軸に射影
		float minA, maxA, minB, maxB;
		ProjectBoxToAxis(verticesA, axis, minA, maxA);
		ProjectBoxToAxis(verticesB, axis, minB, maxB);

		//射影した範囲が交差していないのなら、０を返す
		if (minB > maxA && minA > maxB)
		{
			return 0;
		}

		//衝突法線はOBBその１が左、その２が右にあると想定しているので、
		//順序が逆になっている場合、衝突の向きを反転させる必要がある
		shouldFlip = minB < minA;

		//交差している距離を計算
		float lenA = maxA - minA;	//OBBその１の長さを測定
		float lenB = maxB - minB;	//OBBその２の長さを測定
		float min = std::min(minA, minB);//重なっているオブジェクトの最小値を求める
		float max = std::max(maxA, maxB);//重なっているオブジェクトの最大値を求める

		float length = max - min;	//重なっているオブジェクトの長さを求める

		return length - lenA - lenB;
	}

	//カプセルとOBBを軸ベクトルに射影し、交差している範囲を求める
	//@param capsule    カプセル
	//@param vericesB   OBBの頂点配列
	//@param axis       射影先の軸ベクトル
	//@param shouldFlip 衝突法線の反転の有無
	//@param minIndex   射影範囲の最小値に対応する頂点配列インデックス
	//@param maxIndex   射影範囲の最大値に対応する頂点配列インデックス

	//@return 交差している距離(計算の都合でマイナス値になっている)

	float GetIntersectionLength(const Capsule& capsule, const vec3* verticesB,
		const vec3& axis, bool& shouldFlip, int& minIndex, int& maxIndex)
	{
		//カプセルを軸に射影
		float minA, maxA;

		//始点を軸ベクトルに射影
		minA = maxA = dot(axis, capsule.Seg.Start);

		//終点を軸ベクトルに射影
		float f = dot(axis, capsule.Seg.End);

		//最小値と最大値を設定
		minA = std::min(minA, f) - capsule.Radius;
		maxA = std::max(maxA, f) + capsule.Radius;

		//OBBを軸に射影
		minIndex = maxIndex = 0;
		float minB, maxB;
		minB = maxB = dot(axis, verticesB[0]);

		for (int i = 1; i < 8; ++i)
		{
			float f = dot(axis, verticesB[i]);

			//最小値のインデックスを記録
			if (f < minB)
			{
				minB = f;
				minIndex = i;	
			}

			//最大値のインデックスを記録
			if (f > maxB)
			{
				maxB = f;
				maxIndex = i;	
			}
		}

		//距離が０以上なら分離超平面を入れる隙間がある(つまり交差していない)
		if (minB > maxA && minA > maxB)
		{
			return false;
		}

		//衝突法線はOBBその１が左、OBBその２が右にあると想定しているので、
		//順序が逆になっている場合、衝突法施の向きを反転する必要がある
		shouldFlip = minB < minA;

		//交差している距離を計算
		float lenA = maxA - minA;
		float lenB = maxB - minB;

		//最大最小値を計算
		float min = std::min(minA, minB);
		float max = std::max(maxA, maxB);

		//長さを計算
		float length = max - min;

		return length - lenA - lenB;
	}

	//点がOBB内にあるかどうかを調べる
	//@param point 調べる点の座標
	//@param box   調べるOBB

	//@retval true  pointはboxの内側にある
	//@retval false pointはboxの外側にある

	bool IsPointInsideBox(const vec3& point, const Box& box)
	{
		const vec3 v = point - box.Center;

		for (int i = 0; i < 3; ++i)
		{
			const float dis = dot(v, box.Axis[i]);

			if (dis < -box.Scale[i] || dis>box.Scale[i])
			{
				return false;	//点はOBBの外側にある
			}
		}

		return true;	//点はOBBの内側にある
	}

	//線分と面の交点を求める
	//@param seg   線分
	//@param plane 面
	//@param point 交点を格納する変数

	//@retval true  交点は線分上にある
	//@retval false 交点は線分の外にある

	bool ClipLineSegment(const LineSegment& seg, const Plane& plane,
		vec3& point)
	{
		//辺ベクトルを面法線に射影
		const vec3 ab = seg.End - seg.Start;
		float n = dot(plane.Normal, ab);

		//平行だった場合
		if (abs(n) < 0.001f)
		{
			//交点なし
			return false;
		}

		//交点までの距離を求める
		float t = plane.Distance - dot(plane.Normal, seg.Start);

		//外側にあるかどうか
		if (t<0 || t>n)
		{
			//交点なし
			return false;
		}

		//交点座標を計算
		t /= n;		//ベクトルabに対する比率に変換

		//始点から交点までの距離を足す
		point = seg.Start + ab * t;

		return true;
	}

	//OBBの全ての辺について、もう一つのOBBの面との交点を求める

	//@param vertices OBBその１の頂点配列
	//@param box      OBBその２

	//return edgeとboxの交点
	std::vector<vec3> ClipEdges(const std::vector<vec3>& vertices, const Box& box)
	{
		//辺と面を取得
		const auto Edges = GetEdge(vertices.data());	//辺
		const auto Planes = GetPlanes(box);	//面

		//交点用のメモリを予約
		//2つのOBBが相当深く交差していない限り、交点の数は辺の数以下になるはず
		std::vector<vec3> result;
		result.reserve(Edges.size());

		//辺と面の交点を求める
		for (const auto& plane : Planes)
		{
			for (const auto& edge : Edges)
			{
				//交点の座標
				vec3 point;

				if (ClipLineSegment(edge, plane, point))
				{
					//交点がBBその２の内側にあるなら、有効な点として記録
					if (IsPointInsideBox(point, box))
					{
						result.push_back(point);
					}
				}
			}
		}

		return result;
	}


	//AABB上の最近接点を求める
	//@param aabb  最近接点を求めるAABB
	//@param point 最近接点を求める座標

	//@return pointからaaabbへの最近接点

	vec3 ClosestPoint(const AABB& aabb, const vec3& point)
	{

		//AABBの範囲に収める
		return vec3(
			std::clamp(point.x, aabb.min.x, aabb.max.x),
			std::clamp(point.y, aabb.min.y, aabb.max.y),
			std::clamp(point.z, aabb.min.z, aabb.max.z));
	}

	//OBB上の最近接点を求める
	//@param box   最近接点を求めるOBB
	//@param point 最近接点を求める座標
	
	//@return pointからboxへの最近接点

	vec3 ClosestPoint(const Box& box, const vec3& point)
	{
		//OBBの中心から球体中心へのベクトルを求める
		vec3 v = point - box.Center;

		//OBBの中心点を代入
		vec3 center = box.Center;

		for (int i = 0; i < 3; ++i)
		{
			float dis = dot(box.Axis[i], v);//各軸に射影

			//射影した長さをOBBの長さに収める
			dis = std::clamp(dis, -box.Scale[i], box.Scale[i]);

			//近接点の位置を移動さる
			center += box.Axis[i] * dis;
		}

		//近接点を返す
		return center;
	}


	//線分上の最近接点を求める
	//@param seg
	//@param point

	//@return pointからsegへの最近接点

	vec3 ClosestPoint(const LineSegment& seg, const vec3& point)
	{
		//点を線分に射影
		const vec3 dis = seg.End - seg.Start;
		const float t = dot(point - seg.Start, dis);

		//線分の範囲に制限
		if (t <= 0)
		{
			return seg.Start;
		}

		const float dis2 = dot(dis, dis);//線分の長さの2乗
		//線分の範囲に制限
		if (t >= dis2)
		{
			return seg.End;
		}

		//最近接点を計算して返す
		return seg.Start + dis * (t / dis2);
	}

	//2つの線分の最近接点を求める
	//@param segA　最近接点を求める線分その１
	//@param segB  最近接点を求める線分その２
	//@param denom segA上の最近接点の比率

	//@return segA上の最近接点の比率

	LineSegment ClosestPoint(const LineSegment& segA, const LineSegment& segB)
	{
		const vec3 d0 = segA.End - segA.Start;	//線分Aの方向ベクトル
		const vec3 d1 = segB.End - segB.Start;	//線分Bの方向ベクトル
		const vec3 r = segA.Start - segB.Start;	//線分Aから線分Bの方向ベクトル

		const float a = dot(d0, d0);	//線分Aの長さの2乗
		const float b = dot(d0, d1);	//線分Aと線分Bの内積
		const float c = dot(d0, r);
		const float e = dot(d1, d1);	//線分Bの長さの2乗
		const float f = dot(d1, r);

		//線分の長さが０という特殊ケースを処理する
		//長さが０の線分を点とみなして計算する

		//線分A,B両方の長さが０
		if (a < 0.0001f && e < 0.0001f)
		{
			return { segA.Start,segB.Start };
		}

		//線分Ａの長さが０
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

		//線分Ｂの長さが０
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

		//線分の長さが０より大きいとわかったら、線分が平行かどうかをチェックする
		//線分が平行な場合、ｓを求める式の分母が０になり、計算できないため
		const float denom = a * e - b * b;

		if (denom)
		{
			//線分Ａの最近接点を求め、線分Ａの範囲に制限
			float s = std::clamp((f * b - c * e) / denom, 0.0f, 1.0f);//媒介変数
		
			//線分Ｂの最近接点を求める
			float t = b * s + f;

			//tが線分Ｂの外側を指している場合
			//tを線分Ｂの端点に設定し、sを計算しなおす
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

		//線分が平行な場合、線分の重複する範囲の中点を選択

		//線分Bを線分Aに射影
		float u = dot(d0, segB.Start - segA.Start);
		float v = dot(d0, segB.End - segA.Start);

		//射影した座標を線分Aの範囲に制限
		u = std::clamp(u, 0.0f, a);
		v = std::clamp(v, 0.0f, a);

		//uとvの中点が線分Aの最近接点になる
		float s = (u + v) / a * 0.5f;
		LineSegment result;
		result.Start = segA.Start + d0 * s;

		//線分Aの最近接点を線分Bに射影
		const float t = dot(d1, result.Start - segB.Start);

		//射影した座標を線分Bの範囲に制限
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

	//3D長方形と線分の最近接点を求める
	//@param rect 最近接点を求める3D長方形
	//@param seg  最近接点を求める線分

	//@return seg乗の最近接点とrect上の最近接点

	LineSegment ClosestPoint(const RectAngle& rect, const LineSegment& seg)
	{
		//線分の始点を面に射影
		const vec3 ca = seg.Start - rect.Center;
		const float a0 = dot(ca, rect.Axis[0]);
		const float a1 = dot(ca, rect.Axis[1]);

		//長方形と線分の平行性を調べる
		const vec3 ab = seg.End - seg.Start;
		const float n = dot(rect.Normal, ab);

		if (abs(n) <= 0.0001f)
		{
			//平行な場合、線分が長方形の上に乗っているかどうかを調べる
			float tmin = -FLT_MAX;
			float tmax = FLT_MAX;

			const float list[] = { a0,a1 };

			for (int i = 0; i < 2; ++i)
			{
				//線分とaxis[i]との平行性を調べる
				const float d = dot(ab, rect.Axis[i]);

				if (abs(d) <= 0.0001f)
				{
					//平行なので交点がない
					continue;
				}

				//長方形中心からの距離を線分の比率として表す
				float t0 = (-rect.scale[i] - list[i]) / d;
				float t1 = (rect.scale[i] - list[i]) / d;

				if (t0 > t1)
				{
					std::swap(t0, t1);
				}

				//長方形と交差している範囲を計算
				tmin = std::max(t0, tmin);
				tmax = std::min(t1, tmax);
			}

			//線分の始点と終点の範囲に制限する
			tmin = std::max(0.0f, tmin);
			tmax = std::min(1.0f, tmax);

			//線分が長方形に重なっている場合
			//重複範囲の中心を最近接点とする
			if (tmin <= tmax)
			{
				const vec3 p = seg.Start + ab * ((tmin + tmax) * 0.5f);
				const float d = dot(rect.Normal, p - rect.Center);

				return { p,p - rect.Normal * d };
			}
		}

		//線分が面と交差しているかチェック
		//交差している場合、最近接点は交点になる
		{
			//長方形の中心から法線面に対して射影
			const float d = dot(rect.Normal, rect.Center);

			//長方形から線分の始点までの距離を計算
			const float t = (d - dot(rect.Normal, seg.Start)) / n;

			//交差している場合
			if (0 <= t && t <= 1)
			{
				//交点が長方形の範囲内にあるか調べる
				const vec3 p = seg.Start + ab * t;	//線分から交点に向かうベクトル
				const vec3 v = p - rect.Center;		//長方形の中心から交点に向かうベクトル
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

		//平行でなく、交差もしていない場合
		//線分の終点を面に射影
		const vec3 cb = seg.End - rect.Center;
		const float b0 = dot(cb, rect.Axis[0]);
		const float b1 = dot(cb, rect.Axis[1]);

		//線分の始点および終点の最近接点を求め
		//長方形の範囲に含まれる点のうち、より近い方を仮の最近接点とする
		float distance = FLT_MAX;
		LineSegment Result;//= { vec3(0),vec3(0) };

		if (abs(a0) <= rect.scale[0] && abs(a1) <= rect.scale[1])
		{
			distance = dot(rect.Normal, ca);
			distance *= distance;//比較のため二乗する
			Result = { seg.Start,rect.Center + rect.Axis[0] * a0 + rect.Axis[1] * a1 };
		}

		if (abs(b0) <= rect.scale[0] && abs(b1) <= rect.scale[1])
		{
			float tmp = dot(rect.Normal, cb);
			tmp *= tmp;//比較のため二乗

			if (tmp < distance)
			{
				distance = tmp;
				Result = { seg.End,rect.Center + rect.Axis[0] * b0 + rect.Axis[1] * b1 };
			}
		}

		//長方形の４辺について、最近接点を計算
		const vec3 v1 = rect.Axis[0] * rect.scale[0];
		const vec3 v2 = rect.Axis[1] * rect.scale[1];

		const vec3 v[] = {
			rect.Center + v1 + v2,
			rect.Center + v1 - v2,
			rect.Center - v1 - v2,
			rect.Center - v1 + v2 };
		
		for (int i = 0; i < 4; ++i)
		{
			//4つｎ辺に対して最近接点を求める
			const auto s = ClosestPoint(seg, LineSegment{ v[i],v[(i + 1) % 4] });
			const vec3 ab = s.End - s.Start;
			const float distanceEdge = dot(ab, ab);//長さの２乗

			if (distanceEdge < distance)
			{
				distance = distanceEdge;
				Result = s;
			}
		}

		return Result;
	}

	//球体と球体の衝突判定
	//@param s0 衝突判定を行う球体その1
	//@param s1 衝突判定を行う球体その2
	//@param cp 衝突判定を格納するコンタクトポイント

	//@retval true  衝突している
	//@retval false 衝突していない

	bool Intersect(const Sphere& s0, const Sphere& s1, ContactPoint& cp)
	{
		//中心間の距離が半径合計より大きければ、衝突していない
		const vec3 d = s0.Center - s1.Center;	//中心間の距離
		const float d2 = dot(d, d);	//大きさ
		const float totalRadius = s0.Radius + s1.Radius;	//合計半径

		//ベクトルの大きさを比較
		if (d2 > totalRadius * totalRadius)
		{
			//衝突していない
			return false;
		}

		//衝突している
		
		//コンタクトポイントの設定
		//球体間の距離がほぼ0の場合、衝突方向を上向きに設定
		if (d2 < 0.00001f)
		{
			cp.Normal = vec3(0, 1, 0);
			cp.Penetration = -totalRadius;
		}
		else
		{
			const float distance = std::sqrt(d2);
			cp.Normal = d * (1.0f / distance);			//方向を取得
			cp.Penetration = distance - totalRadius;	//貫通距離を計算
		}

		//衝突位置を球体の中間に設定
		cp.Position = s0.Center - cp.Normal * (s0.Radius - cp.Penetration * 0.5f);
		return true;
	}

	//AABBとAABBの衝突判定
	//@param a 衝突判定を行うAABBその1
	//@param b 衝突判定を行うAABBその2
	//@param cp 衝突判定を格納するコンタクトポイント
	
	//@retval true  衝突している
	//@retval false 衝突していない

	bool Intersect(const AABB& a, const AABB& b, ContactPoint& cp)
	{
		
		int n = 0;	//衝突方向
		cp.Penetration = -FLT_MAX;	//貫通距離を最大にしておく

		//衝突したかどうかを判定
		for (int i = 0; i < 3; ++i)
		{
			const float t0 = a.min[i] - b.max[i];

			if (t0 > 0)
			{
				//衝突していない
				return false;
			}

			//貫通距離が短い方向を衝突方向とする
			if (t0 > cp.Penetration)
			{
				cp.Penetration = t0;
				n = i * 2;
			}

			const float t1 = b.min[i] - a.max[i];

			if (t1 > 0)
			{
				//衝突していない
				return false;
			}
			//貫通距離が短い方向を衝突方向とする
			if (t1 > cp.Penetration)
			{
				cp.Penetration = t1;
				n = i * 2 + 1;
			}

		}

		//衝突方向を設定
		static const vec3 normal[] =
		{
			{ 1, 0, 0},{ -1,  0,  0},
			{ 0, 1, 0},{  0, -1,  0},
			{ 0, 0, 1},{  0,  0, -1},
		};
		cp.Normal = normal[n];

		//衝突点を計算
		for (int i = 0; i < 3; ++i)
		{
			cp.Position[i] =
				std::max(a.min[i], b.min[i]) + std::min(a.max[i], b.max[i]);
		}

		cp.Position *= 0.5f;
		return true;
	}

	//球体とAABBの衝突判定
	//@param sphere 衝突判定を行う球体
	//@param aabb   衝突判定を行うAABB
	//@param cp     衝突結果を格納するコンタクトポイント
	
	//@retval true  衝突している
	//@retval false 衝突していない

	bool Intersect(const Sphere& sphere, const AABB& aabb, ContactPoint& cp)
	{
		//最近接点から球体の中心までの距離が、球体の半径より大きければ、衝突していない
		const vec3 point = ClosestPoint(aabb, sphere.Center);//最近接点
		const vec3 dis = sphere.Center - point;//最近接点から球体の中心へのベクトル

		const float dis2 = dot(dis, dis);//距離の大きさ

		//距離の大きさが半径よりも大きい
		if (dis2 > sphere.Radius * sphere.Radius)
		{
			//衝突していない
			return false;
		}

		//コンタクトポイントの設定

		//衝突した距離が限りなく０に近い場合
		if (dis2 < 0.00001f)
		{
			//球体の中心はAABB内部にあるので、貫通距離が最も短い面から衝突したとみなす

			int n = 0;	//衝突方向
			cp.Penetration = -FLT_MAX;	//貫通距離を最大にしておく

			//衝突したかどうかを判定
			for (int i = 0; i < 3; ++i)
			{
				const float t0 = point[i] - aabb.max[i];

				//貫通距離が短い方向を衝突方向とする
				if (t0 > cp.Penetration)
				{
					n = i * 2;
					cp.Penetration = t0;
				}

				const float t1 = aabb.min[i] - point[i];

				//貫通距離が短い方向を衝突方向とする
				if (t1 > cp.Penetration)
				{
					n = i * 2 + 1;
					cp.Penetration = t1;
				}

			}

			//衝突方向を設定
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
			//距離が０より大きい場合、球体の中心はAABBの外側にあるので、
			//最近接点から球体の中心へ向かう方向から衝突したとみなす
			const float distance = std::sqrt(dis2);

			cp.Normal = dis * (1.0f / distance);		//向きを取得
			cp.Penetration = distance - sphere.Radius;	//貫通距離を計算
			
		}
		//衝突している
		return true;
	}

	//AABBと球体の衝突判定
	//@param aabb   衝突判定を行うAABB
	//@param sphere 衝突判定を行う球体
	//@param cp     衝突結果を格納するコンタクトポイント
	
	//@retval true  衝突している
	//@retval false 衝突していない

	bool Intersect(const AABB& aabb, const Sphere& sphere, ContactPoint& cp)
	{
		//引数の順番が逆なだけなので、関数を流用する
		if (Intersect(sphere, aabb, cp))
		{
			//衝突の向きを逆にする
			cp.Normal = -cp.Normal;
			return true;
		}

		return false;
	}

	//球体とOBBの衝突判定
	//@param sphere   衝突判定を行う球体
	//@param box      衝突判定を行うOBB
	//@param cp       衝突結果を格納するコンタクトポイント
	
	//@retval true  衝突している
	//@retval false 衝突していない

	bool Intersect(const Sphere& sphere, const Box& box, ContactPoint& cp)
	{
		//最近接点から球体の中心までの距離が、球体の半径より大きければ、衝突していない
		const vec3 point = ClosestPoint(box, sphere.Center);	//最近接点
		const vec3 dis = sphere.Center - point;		//最近接点から球体の中心へのベクトル

		const float dis2 = dot(dis, dis);//距離の大きさ

		//距離の大きさが半径よりも大きい
		if (dis2 > sphere.Radius * sphere.Radius)
		{
			//衝突していない
			return false;
		}

		//コンタクトポイントの設定

		//衝突した距離が限りなく０に近い場合
		if (dis2 < 0.00001f)
		{
			//球体の中心はAABB内部にあるので、貫通距離が最も短い面から衝突したとみなす

			//OBB中心から球体中心へのベクトル
			const vec3 a = point - box.Center;
			cp.Penetration = -FLT_MAX;		//貫通距離を最大にしておく

			//衝突したかどうかを判定
			for (int i = 0; i < 3; ++i)
			{
				const float f = dot(box.Axis[i], a);	//固有軸へ射影
				const float t0 = f - box.Scale[i];

				//貫通距離が短い方向を衝突方向とする
				if (t0 > cp.Penetration)
				{
					cp.Normal = box.Axis[i];
					cp.Penetration = t0;
				}

				const float t1 = -box.Scale[i] - f;

				//貫通距離が短い方向を衝突方向とする
				if (t1 > cp.Penetration)
				{
					cp.Normal = -box.Axis[i];
					cp.Penetration = t1;
				}
			}

			//貫通距離に球の半径を加える
			cp.Penetration -= sphere.Radius;
		}
		else
		{
			//距離が０より大きい場合、球体の中心はAABBの外側にあるので、
			//最近接点から球体の中心へ向かう方向から衝突したとみなす
			const float distance = std::sqrt(dis2);

			cp.Normal = dis * (1.0f / distance);		//向きを取得
			cp.Penetration = distance - sphere.Radius;	//貫通距離を計算

		}
		cp.Position = point - cp.Normal * cp.Penetration * 0.5f;
		//衝突している
		return true;
	}

	//OBBと球体の衝突判定
	//@param box      衝突判定を行うOBB
	//@param sphere   衝突判定を行う球体
	//@param cp       衝突結果を格納するコンタクトポイント
	
	//@retval true  衝突している
	//@retval false 衝突していない

	bool Intersect(const Box& box, const Sphere& sphere, ContactPoint& cp)
	{
		//関数を流用する
		if (Intersect(sphere, box, cp))
		{
			//貫通方向を反対にする
			cp.Normal = -cp.Normal;

			//衝突している
			return true;
		}

		//衝突していない
		return false;
	}

	//球体とカプセルの衝突判定
	//@param sphere   衝突判定を行う球体
	//@param capsule  衝突判定を行うカプセル
	//@param cp       衝突結果を格納するコンタクトポイント

	//@retval true  衝突している
	//@retval false 衝突していない

	bool Intersect(const Sphere& sphere, const Capsule& capsule, ContactPoint& cp)
	{
		//カプセルの中心の線分と球体の中心点の近接点を求める
		const vec3 point = ClosestPoint(capsule.Seg, sphere.Center);

		//最近接点を中心とする球体と元の球体との衝突判定を求める
		return Intersect(sphere, Sphere{ point,capsule.Radius }, cp);
	}

	//カプセルと球体の衝突判定
	//@param capsule  衝突判定を行うカプセル
	//@param sphere   衝突判定を行う球体
	//@param cp       衝突結果を格納するコンタクトポイント
	
	//@retval true  衝突している
	//@retval false 衝突していない

	bool Intersect(const Capsule& capsule, const Sphere& sphere, ContactPoint& cp)
	{
		//カプセルの中心の線分と球体の中心点の近接点を求める
		const vec3 point = ClosestPoint(capsule.Seg, sphere.Center);

		//球体と最近接点を中心とする球体との衝突判定を求める
		return Intersect(Sphere{ point,capsule.Radius },sphere, cp);
	}

	//カプセルとカプセルの衝突判定
	//@param capsule  衝突判定を行うカプセルその１
	//@param sphere   衝突判定を行うカプセルその２
	//@param cp       衝突結果を格納するコンタクトポイント

	//@retval true  衝突している
	//@retval false 衝突していない
	bool Intersect(const Capsule& capA, const Capsule& capB, ContactPoint& cp)
	{
		//カプセル同士の中心線の最近接点を求める
		const LineSegment seg = ClosestPoint(capA.Seg, capB.Seg);

		//求めた線分の向きを取得
		const vec3 dis = seg.Start - seg.End;	

		//長さの2乗
		const float dis2 = dot(dis, dis);

		//合計半径
		const float totalRadius = capA.Radius + capB.Radius;

		//線分の長さが半径の合計よりも大きかったら
		if (dis2 > totalRadius * totalRadius)
		{
			return false;
		}

		//最近接点同士の距離が0より大きい場合
		//球体同士と同じ方法でコンタクトポイントを計算する
		if (dis2 >= 0.0001f)
		{
			const float distance = std::sqrt(dis2);

			cp.Normal = dis * (1.0f / distance);		//衝突方向
			cp.Penetration = distance - totalRadius;	//衝突距離
			cp.Position = seg.Start - cp.Normal * (capA.Radius - cp.Penetration * 0.5f);

			return true;
		}

		//最近接点同士の距離が０の場合
		//segから法線を計算できないので、2軸に垂直な方向を衝突法線とする

		//それぞれのベクトルを取得
		const vec3 d0 = capA.Seg.End - capA.Seg.Start;
		const vec3 d1 = capB.Seg.End - capB.Seg.Start;

		//外積で垂直方向を取得
		cp.Normal = cross(d0, d1);

		//垂直法線の長さが0の場合、2軸は平行
		if (dot(cp.Normal, cp.Normal) < 0.00001f)
		{
			//法線ベクトルを作れないので、d0ベクトルに垂直な方向を衝突法線とする
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

		//大きさを１にする
		cp.Normal = normalize(cp.Normal);

		//大きい方の半径を貫通距離とする
		cp.Penetration = -std::max(capA.Radius, capB.Radius);

		//コンタクトポイントの座標を計算
		cp.Position = seg.Start - cp.Normal * (capA.Radius - cp.Penetration * 0.5f);
		return true;
	}


	//OBBとOBBの衝突判定
	//@param boxA      衝突判定を行うOBBその１
	//@param boxB	   衝突判定を行うOBBその２
	//@param cp        衝突結果を格納するコンタクトポイント
	
	//@retval true  衝突している
	//@retval false 衝突していない

	bool Intersect(const Box& boxA, const Box& boxB, ContactPoint& cp)
	{
		//OBBの頂点座標配列を取得
		const auto verticesA = GetVertices(boxA);
		const auto verticesB = GetVertices(boxB);

		//貫通距離を最大にしておく
		cp.Penetration = -FLT_MAX;

		//分離軸判定用の軸ベクトルを計算
		vec3 axisList[15] = {
			boxA.Axis[0],boxA.Axis[1],boxA.Axis[2],
			-boxB.Axis[0],-boxB.Axis[1],-boxB.Axis[2],
		};

		for (int i = 0; i < 3; ++i)
		{
			//boxAとboxBの外積を計算する
			axisList[6 + i * 3 + 0] = cross(axisList[i], axisList[3]);
			axisList[6 + i * 3 + 1] = cross(axisList[i], axisList[4]);
			axisList[6 + i * 3 + 2] = cross(axisList[i], axisList[5]);
		}

		//すべての軸ベクトルに対して分離軸判定を実行
		for (int i = 0; i < 15; ++i)
		{
			//外積によって作られた軸ベクトルの場合、元の軸が平行だと、長さが０になる
			//この場合、ＡまたはＢの軸による分離軸判定と等しいので、スキップ
			const float Lengh2 = dot(axisList[i], axisList[i]);
			if (Lengh2 < 0.001f)
			{
				continue;
			}

			//交差している距離を計算
			bool shouldFlip;
			float dis = GetIntersectionLength(
				verticesA.data(), verticesB.data(), axisList[i], shouldFlip);

			//距離が0以上なら分離超平面を入れる隙間がある(つまり交差していない)
			if (dis >= 0)
			{
				//衝突していない
				return false;
			}

			//外戚で作成した軸ベクトルの長さは、１にならない(sinθなので)
			//結果として、disは軸ベクトルの長さを単位とした値となるので
			//長さを比較するために、正規化し、単位をそろえる
			dis /= std::sqrt(Lengh2);

			//衝突距離がより短い方向から衝突したとみなす
			if (dis > cp.Penetration)
			{
				//貫通距離を設定
				cp.Penetration = dis;

				//衝突法線は軸ベクトルの逆方向
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

		//衝突法線を正規化
		cp.Normal = normalize(cp.Normal);


		//コンタクトポイントを計算

		//OBBとOBBの交点を求める
		std::vector<vec3> c0 = ClipEdges(verticesB, boxA);	//辺と面の交点を求める
		std::vector<vec3> c1 = ClipEdges(verticesA, boxB);	//辺と面の交点を求める

		//c0に交点を集める
		c0.insert(c0.end(), c1.begin(), c1.end());

		//貫通距離の中間に衝突平面を設定(衝突平面上の点ｐを求める)
		float min, max;
		ProjectBoxToAxis(verticesA.data(), cp.Normal, min, max);	//minとmaxを設定

		const vec3 p = boxA.Center - cp.Normal * (max - min + cp.Penetration) * 0.5f;

		//交点を衝突平面に射影しつつ、重複する交点を削除
		int count = 0;
		for (int i = 0; i < c0.size(); ++i, ++count)
		{
			//交点を衝突平面に射影
			c0[count] = c0[i] + cp.Normal * dot(cp.Normal, p - c0[i]);

			//重複する交点がある場合は削除
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

		//交点の重心を計算し、コンタクトポイントとする
		cp.Position = vec3(0);

		for (const auto& e : c0)
		{
			cp.Position += e;
		}
		cp.Position /= static_cast<float>(c0.size());

		//衝突している
		return true;
	}


	//OBBとAABBの衝突判定

	//@param box  衝突判定を行うOBB
	//@param aabb 衝突判定を行うAABB
	//@param cp   衝突結果を格納するコンタクトポイント

	//@retval true  衝突している
	//@retval false 衝突していない

	bool Intersect(const Box& box, const AABB& aabb, ContactPoint& cp)
	{
		//AABBを回転していないBoxとして扱う
		Box boxB;
		boxB.Center = (aabb.min + aabb.max) * 0.5f;
		boxB.Scale = (aabb.max - aabb.min) * 0.5f;

		return Intersect(box, boxB, cp);
	}

	//AABBとOBBの衝突判定

	//@param aabb 衝突判定を行うAABB
	//@param box  衝突判定を行うOBB
	//@param cp   衝突結果を格納するコンタクトポイント
	
	//@retval true  衝突している
	//@retval false 衝突していない

	bool Intersect(const AABB& aabb, const Box& box, ContactPoint& cp)
	{
		//AABBを回転していないBoxとして扱う
		Box boxA;
		boxA.Center = (aabb.min + aabb.max) * 0.5f;
		boxA.Scale = (aabb.max - aabb.min) * 0.5f;

		return Intersect(boxA, box, cp);

	}

	//カプセルとOBBの衝突判定

	//@param capsule 衝突判定を行うカプセル
	//@param box  衝突判定を行うOBB
	//@param cp   衝突結果を格納するコンタクトポイント
	
	//@retval true  衝突している
	//@retval false 衝突していない

	bool Intersect(const Capsule& capsule, const Box& box, ContactPoint& cp)
	{
		//OBBの頂点配列を取得
		const std::vector<vec3> verticesA = { capsule.Seg.Start,capsule.Seg.End };
		const auto verticesB = GetVertices(box);

		//分離軸判定用の軸ベクトルを計算
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
		

		//貫通距離を最大にしておく
		cp.Penetration = -FLT_MAX;

		//全ての軸ベクトルに対して分離軸判定を実行
		int axisIndex = 0;//衝突した文理軸のインデックス
		int minIndex = 0, maxIndex = 0;

		for (int i = 0; i < std::size(axisList); ++i)
		{
			//外積によって作られた軸ベクトルの場合、元の軸が平行だと長さが０になる
			//この場合AまたはBの軸による分離軸判定と等しいのでスキップする
			const float length2 = dot(axisList[i], axisList[i]);

			if (length2 < 0.0001f)
			{
				continue;
			}

			//分離軸を正規化
			const vec3 axis = axisList[i] * (1.0f / sqrt(length2));

			//交差している距離を計算
			bool shouldFlip;
			int tmpMinIndex, tmpMaxIndex;
			const float d = GetIntersectionLength(capsule, verticesB.data(),
				axis, shouldFlip, tmpMinIndex, tmpMaxIndex);

			//距離が０以上なら分離超平面を入れる隙間がある(交差していない)
			if (d >= 0)
			{
				return false;
			}

			//貫通距離がより短い方向から衝突したとみなす
			if (d > cp.Penetration)
			{
				cp.Penetration = d;

				//衝突法線は軸ベクトルの逆方向
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

		//半球部分の交差を確認
		const vec3 list[] = { capsule.Seg.Start,capsule.Seg.End };
		float direction = 1;//カプセル軸の向き調整用

		for (const auto& e : list)
		{
			//OBBの最近接点が半球側にない場合は無視
			const vec3 c = ClosestPoint(box, e);
			const vec3 v = c - e;
			const float p = dot(capsuleDirection, v) * direction;

			direction *= -1;

			if (p >= 0)
			{
				continue;
			}

			//最近接点までの距離がカプセルの半径より大きい場合は無視
			float t = dot(v, v);	//距離の２乗
			
			if (t > capsule.Radius * capsule.Radius)
			{
				continue;
			}

			//最近接点までの距離が分離軸による距離より短い場合のみ半球との衝突を選択
			t = sqrt(t);	//2乗の状態を解除
			const float d = t - capsule.Radius;

			if (d > cp.Penetration)
			{
				cp.Penetration = d;
				cp.Normal = -v * (1.0f / t);
				cp.Position = c + cp.Normal * (cp.Penetration * 0.5f);
				axisIndex = -1;
			}
		}

		//コンタクトポイントを計算
		//分離軸によって場合分け
		switch (axisIndex)
		{
			case 0:
			case 1:
			case 2:
			{
				//面で衝突
				//線分の始点および、終点について面上の最近接点を計算
				//最近接点がOBBの範囲内にある場合のみ有効
				const int axisIndex1 = (axisIndex + 1) % 3;
				const int axisIndex2 = (axisIndex + 2) % 3;

				const RectAngle rect = {
					box.Center + cp.Normal * box.Scale[axisIndex],
					cp.Normal,
					box.Axis[axisIndex1],box.Axis[axisIndex2],
					box.Scale[axisIndex1],box.Scale[axisIndex2] };

				const auto seg = ClosestPoint(rect, capsule.Seg);

				//交差したセグメントが０では無かったら
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
				//辺で衝突
				//   5----4
				//  /|   /|
				// 1----0 |
				// | 7--|-6
				// |/   |/
				// 3----2

				//目的の辺を作り出すための頂点を素早く選択する
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
				//頂点で衝突
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

	//OBBとカプセルの衝突判定

	//@param box      衝突判定を行うOBB
	//@param capsule  衝突判定を行うカプセル
	//@param cp       衝突結果を格納するコンタクトポイント
	
	//@retval true  衝突している
	//@retval false 衝突していない

	bool Intersect(const Box& box, const Capsule& capsule, ContactPoint& cp)
	{
		//引数の順番が違うだけなので、そのまま流用
		if (Intersect(capsule, box, cp))
		{
			//衝突の向きを逆にする
			cp.Normal = -cp.Normal;

			return true;
		}

		return false;
	}

	//カプセルとAABBの衝突判定

	//@param capsule  衝突判定を行うカプセル
	//@param AABB     衝突判定を行うAABB
	//@param cp       衝突結果を格納するコンタクトポイント
	
	//@retval true  衝突している
	//@retval false 衝突していない

	bool Intersect(const Capsule& capsule, const AABB& aabb, ContactPoint& cp)
	{
		//AABBをBOXに変換して当たりをとる
		Box box;
		box.Center = (aabb.min + aabb.max) * 0.5f;
		box.Scale = (aabb.max - aabb.min) * 0.5f;


		return Intersect(capsule, box, cp);
	}

	//カプセルとAABBの衝突判定

	//@param capsule  衝突判定を行うカプセル
	//@param AABB     衝突判定を行うAABB
	//@param cp       衝突結果を格納するコンタクトポイント
	
	//@retval true  衝突している
	//@retval false 衝突していない

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