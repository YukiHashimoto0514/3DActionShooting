//@file Collider.h

#ifndef COMPONENT_COLLIDER_H_INCLUDED
#define COMPONENT_COLLIDER_H_INCLUDED

#include "../Engine/Component.h"
#include "../Engine/Collision.h"
#include "../Engine/VecMath.h"
#include <memory>

//先行宣言
class Collider;
using ColliderPtr = std::shared_ptr<Collider>;

//衝突形状
class Collider : public Component
{
public:
	Collider() = default;			//コンストラクタ
	virtual ~Collider() = default;	//デストラクタ

	//コライダー形状
	enum class Type
	{
		Sphere,	//球体
		AABB,	//軸平行境界ボックス
		Box,	//有向境界ボックス
		Capsule,//カプセル
	};

	//どの形状化を取得する
	virtual Type GetType() const = 0;

	//座標を更新する
	virtual void AddPosition(const VecMath::vec3& translate) = 0;

	//座標変換したコライダーを取得する
	virtual ColliderPtr GetTransformedCollider(
		const VecMath::vec3& translation,
		const VecMath::mat3& matRotation,
		const VecMath::vec3& scale) = 0;

};


//球体用コライダー
class SphereCollider :public Collider
{
public:

	SphereCollider() = default;				//コンストラクタ
	virtual ~SphereCollider() = default;	//デストラクタ

	//コライダーの形状
	virtual Type GetType()const override
	{
		return Type::Sphere;
	}

	//コリジョンの形状
	const Collision::Sphere& GetShape() const 
	{
		return sphere;
	}

	//座標を変更する
	virtual void AddPosition(const VecMath::vec3& translate)override;

	//座標返還したコライダーを取得する
	virtual ColliderPtr GetTransformedCollider(
		const VecMath::vec3& translation,
		const VecMath::mat3& matRotation,
		const VecMath::vec3& scale)override;

	//球体
	Collision::Sphere sphere = { VecMath::vec3(0),1 };
};


//AABB用コライダー
class AABBCollider :public Collider
{
public:
	AABBCollider() = default;			//コンストラクタ
	virtual ~AABBCollider() = default;	//デストラクタ

	//コライダーの形状
	virtual Type GetType() const override
	{
		return Type::AABB;
	}

	//コリジョンの形状
	const Collision::AABB& GetShape()const
	{
		return aabb;
	}

	//座標を変更する
	virtual void AddPosition(const VecMath::vec3& translate)override;

	//座標変換したコライダーを取得する
	virtual ColliderPtr GetTransformedCollider(
		const VecMath::vec3& translation,
		const VecMath::mat3& matRotation,
		const VecMath::vec3& scale)override;

	//AABB
	Collision::AABB aabb;
};


//OBB用コライダー
class BoxCollider :public Collider
{
public:
	BoxCollider() = default;			//コンストラクタ
	virtual ~BoxCollider() = default;	//デストラクタ

	//図形を取得する
	virtual Type GetType() const override
	{
		return Type::Box;
	}

	//形を取得する
	const Collision::Box& GetShape() const
	{
		return box;
	}

	//座標を変更する
	virtual void AddPosition(const VecMath::vec3& translate)override;

	//座標変換したコライダーを取得する
	virtual ColliderPtr GetTransformedCollider(
		const VecMath::vec3& translation,
		const VecMath::mat3& matRotation,
		const VecMath::vec3& scale)override;

	Collision::Box box;
};

//カプセル用コライダー
class CapsuleCollider :public Collider
{
public:
	CapsuleCollider() = default;			//コンストラクタ
	virtual ~CapsuleCollider() = default;	//デストラクタ

	//図形を取得する
	virtual Type GetType() const override { return Type::Capsule; }

	//形を取得する
	const Collision::Capsule& GetShape() const { return capsule; }
	
	//座標を変更する
	virtual void AddPosition(const VecMath::vec3& translate) override;

	//座標変換したコライダーを取得する
	virtual ColliderPtr GetTransformedCollider(
		const VecMath::vec3& translation,
		const VecMath::mat3& matRotation,
		const VecMath::vec3& scale)override;
	
	Collision::Capsule capsule = {VecMath::vec3(0)};
};

//様々なコライダーに対応できる衝突判定
bool Intersect(const Collider& a, const Collider& b, Collision::ContactPoint& cp);

#endif // !COMPONENT_COLLIDER_H_INCLUDED

