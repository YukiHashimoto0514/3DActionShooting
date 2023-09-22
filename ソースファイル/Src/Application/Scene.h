//@file Scene.h

#ifndef SCENE_H_INCLUDED
#define SCENE_H_INCLUDED
#include <memory>

//先行宣言
class Engine;
class GameObject;

using GameObjectPtr = std::shared_ptr<GameObject>;

//シーンの基底クラス
class Scene
{
public:
	Scene() = default;			//コンストラクタ
	virtual ~Scene() = default;	//デストラクタ
	virtual bool Initialize(Engine& engine) { return 0; }		//初期化
	virtual void Update(Engine& engine, float deltaTime) = 0;	//更新処理
	virtual void Finalize(Engine& engine){}						//終了処理

};

using ScenePtr = std::shared_ptr<Scene>;	//シーンポインタ型


#endif //SCENE_H_INCLUDED