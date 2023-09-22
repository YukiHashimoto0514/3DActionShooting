//@file Collider.cpp

#include "Collider.h"

using namespace VecMath;

//このファイル内だけで使用できるようにする
namespace/*unnamed*/
{
	//コライダー型に対応する衝突判定関数を呼び出す
	template<typename T,typename U>
	bool Func(const Collider& a, const Collider& b,
		Collision::ContactPoint& cp)
	{
		return Intersect(static_cast<const T&>(a).GetShape(),
			static_cast<const U&>(b).GetShape(), cp);
	}

	//衝突しない組み合わせに使う関数
	bool NotCollide(const Collider& a, const Collider& b,
		Collision::ContactPoint& cp)
	{
		return false;
	}
}

//コライダーの衝突判定
//@param a      衝突判定を行うコライダーその１
//@param b      衝突判定を行うコライダーその２
//@param cp     衝突結果を格納するコンタクトポイント

//@retval true  衝突している
//@retval false 衝突していない

bool Intersect(const Collider& a, const Collider& b,
	Collision::ContactPoint& cp)
{
	//関数ポインタ型を定義	(ポインタが参照している関数を呼び出す:今の場合だと、Func関数)
	using FuncType =
		bool(*)(const Collider&, const Collider&, Collision::ContactPoint&);

	//関数テーブル	(実行する関数を切り替える)
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

	//コライダーの型に対応する衝突判定関数を実行
	const int shapeA = static_cast<int>(a.GetType());
	const int shapeB = static_cast<int>(b.GetType());

	return FuncList[shapeA][shapeB](a, b, cp);
}


//座標を更新する
void SphereCollider::AddPosition(const vec3& translate)
{
	sphere.Center += translate;
}

//座標変換したコライダーを取得する
ColliderPtr SphereCollider::GetTransformedCollider(
	const vec3& translation,
	const mat3& matRotation,
	const vec3& scale)
{
	auto world = std::make_shared<SphereCollider>();

	//sphereの半径と中心を代入
	world->sphere.Center = translation + matRotation * (sphere.Center * scale);

											//scaleのうち、一番大きな値を使用する
	world->sphere.Radius = sphere.Radius * std::max({ scale.x,scale.y,scale.z });

	return world;
}

//座標を更新する
void AABBCollider::AddPosition(const vec3& translate)
{
	aabb.min += translate;
	aabb.max += translate;
}

//座標変換したコライダーを取得する
ColliderPtr AABBCollider::GetTransformedCollider(
	const vec3& translation,
	const mat3& matRotation,
	const vec3& scale)
{
	auto world = std::make_shared<AABBCollider>();	//AABBコライダーを取得
	auto center = (aabb.max + aabb.min) * 0.5f;		//中心を求める

	center = translation + matRotation * (center * scale);

	auto s = (aabb.max - aabb.min) * 0.5f * scale;

	//scale分ずらした値で更新
	world->aabb.max = center + s;
	world->aabb.min = center - s;

	return world;
}


//座標を変更する

void BoxCollider::AddPosition(const vec3& translate)
{
	box.Center += translate;
}

//座標変換したコライダーを取得する
ColliderPtr BoxCollider::GetTransformedCollider(
	const vec3& translation,
	const mat3& matRotation,
	const vec3& scale)
{
	auto world = std::make_shared<BoxCollider>();
	world->box.Center = translation + matRotation * (box.Center * scale);

	//固有軸を回転させる
	for (int i = 0; i < 3; i++)
	{
		world->box.Axis[i] = matRotation * box.Axis[i];
	}

	world->box.Scale = box.Scale * scale;

	return world;
}

//座標を変更する
void CapsuleCollider::AddPosition(const vec3& translate)
{
	capsule.Seg.Start += translate;
	capsule.Seg.End += translate;
}

//座標変換したコライダーを取得する
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

	//半径のスケールにはXYZのうち最大のものを選ぶ
	world->capsule.Radius =
		capsule.Radius * std::max({ scale.x,scale.y,scale.z });

	return world;
}
