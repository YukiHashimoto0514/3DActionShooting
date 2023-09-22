//@file Component.h

#ifndef COMPONENT_H_INCLUDED
#define COMPONENT_H_INCLUDED

#include <string>
#include <vector>
#include <memory>

//先行宣言
class GameObject;
class Component;

using ComponentPtr = std::shared_ptr<Component>;//コンポーネントのポインタ
using ComponentList = std::vector<ComponentPtr>;//コンポーネントのポインタの配列

//ダメージ情報
struct Damage {
	float amount = 0;	//ダメージ量
	int type = 0;		//ダメージの種類
};

//コンポーネントの基底クラス
class Component
{
public:
	Component() = default;
	virtual ~Component() = default;

	//最初のUpdateの直前で呼び出される
	virtual void Start(GameObject& gameObject){}

	//毎フレーム1回呼び出される
	virtual void Update(GameObject& gameObject, float deltaTime){}

	//衝突が起きたときに呼び出される
	virtual void OnCollision(GameObject& gameObject,GameObject& other){}

	//ダメージを受けたときに呼び出される
	virtual void OnTakeDamage(GameObject& gameObject, GameObject& other, const Damage& dammage) {}

	//ゲームオブジェクトがエンジンから削除されるときに呼び出される
	virtual void OnDestroy(GameObject& gameObject) {}

	std::string name;//コンポーネント名
};


#endif //COMPONENT_H_INCLUDED