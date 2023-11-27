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

//先行宣言
class Engine;
class Player;
class Collider;
class GameObject;
class MeshRenderer;
class ProgramPipeline;

using PlayerPtr = std::shared_ptr<Player>;				//プレイヤーのゲームオブジェクトポインタ
using ColliderPtr = std::shared_ptr<Collider>;			//コライダー用の配列;
using GameObjectPtr = std::shared_ptr<GameObject>;		//ゲームオブジェクトのポインタ
using GameObjectList = std::vector<GameObjectPtr>;		//ゲームオブジェクトのポインタの配列
using MeshRendererPtr = std::shared_ptr<MeshRenderer>;	//描画コンポーネント


using namespace VecMath;

//ゲーム内に登場する様々なゲームオブジェトを表す基底クラス
class GameObject
{
public:
	GameObject() = default;				//コンストラクタ
	virtual ~GameObject() = default;	//デストラクタ

	//イベントの操作
	virtual void Start();							//最初のUpdateの直前で呼び出される
	virtual void Update(float deltaTime);			//毎フレーム1回呼び出される
	virtual void OnCollision(GameObject& object);	//衝突が起きたときに呼び出される
	virtual void OnDestroy();						//ゲームオブジェクトがエンジンから削除されるときに呼び出される
	virtual void TakeDamage(GameObject& other, const Damage& damage);	//ダメージを受けたときに呼び出される

	bool IsStarted() const { return isStarted; }	//始まっているかどうか

	//スプライトの操作
	void AddSprite(const Texcoord& tc,
		float x = 0, float y = 0, float scale = 1, float rotation = 0);

	const SpriteList& GetSprite() const { return spriteList; }

	//コリジョンの操作
	void AddCollision(const Rect& r);
	const RectList& GetCollision()const { return collisionList; }
	const Rect& GetAabb()const { return aabb; }
	
	//コンポーネントの操作
	template<typename T>
	std::shared_ptr<T> AddComponent()	//これはどんな型にでも対応できる関数の書き方（中身が一緒の場合）
	{
		std::shared_ptr<T> p = std::make_shared<T>();
		componentList.push_back(p);

		//メッシュレンダラの場合は専用リストに追加
		if constexpr (std::is_base_of<MeshRenderer, T>::value)//基底クラスにMeshrendereをもっているか
		{
			meshRendererList.push_back(p);
		}

		//コライダー専用リストに追加
		if constexpr (std::is_base_of<Collider, T>::value)//基底クラスにColliderをもっているか
		{
			colliderList.push_back(p);
		}
		return p;
	}

	//描画
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

	//無敵時間を減らす
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


	Engine* engine = nullptr;		//エンジンのアドレス
	std::string name;				//オブジェクト名

	float v0 = 5;					//初速度
	float JumpTimer = 0;			//飛んでいる時間を計測

	SpriteList spriteList;			//スプライト配列
	ComponentList componentList;	//コンポーネント配列

	int only = 0;
	std::vector<ColliderPtr> colliderList;
private:
	vec3 Position = { 0,0,0 };	//座標
	vec3 Reflection = { 0,0,0 };//反射ベクトル
	vec3 Forward = { 0,0,0 };	//正面の向き
	vec3 Right = { 0,0,0 };		//右側の向き
	vec3 Rotation = { 0,0,0 };	//回転
	vec3 Scale = { 1,1,1 };		//拡大率
	vec3 RGB = { 1,1,1 };		//３原色

	float HP = 1.0f;				//体力
	float ImmortalTime = 0;			//無敵時間
	float Alpha = 1.0f;				//画像の透明度
	int Priority = 0;

	bool isDead = false;			//死亡フラグ
	bool isStarted = false;			//スタート実行済みフラグ
	bool isJump = false;			//ジャンプできるかどうか(true=できる false=できない)
	bool isMovable = true;			//移動できるかどうか(true=移動できるfalse=無理)
	bool PinchFlg = false;			//true=HPが半分より小さい false=HPが半分以上
	bool isAir = true;				//空中にいるかどうか(true=空なう　false=陸)

	RectList collisionList;			//コリジョン配列
	Rect aabb = { 0,0,0,0 };		//コリジョン境界ボックス(最小の長方形)
	//   aabb = Axis Aligned Bounding Box

	std::vector<MeshRendererPtr> meshRendererList;
};


#endif //GAMEOBJECT_H_INCLUDED