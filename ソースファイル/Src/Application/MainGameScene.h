//@file MainGameScene.h

#ifndef MAINGAMESCENE_H_INCLUDED
#define MAINGAMESCENE_H_INCLUDED

#include "Scene.h"
#include <vector>
#include "../Engine/Engine.h"
#include "../Component/TimeLimit.h"

//先行宣言
class Player;

using PlayerPtr = std::shared_ptr<Player>;

class MainGameScene :public Scene
{
public:
	GLFWwindow* window = nullptr;

	//コンストラクタ
	MainGameScene() = default;

	//デストラクタ
	virtual ~MainGameScene() = default;

	virtual bool Initialize(Engine& engine)override;	//初期化関数
	virtual void Update(Engine& engine, float deltaTime)override;		//更新関数

	void CameraRot(GLFWwindow* window, GameObjectPtr camera, float deltaTime, double MouceX, double MouceY);
	void PlayerUpdate(Engine& engine, PlayerPtr player, float deltaTime);

	size_t mouce;
	GameObjectPtr uimouce;

private:

	void PlayerInit(Engine& engine);

	GameObjectPtr BossObj;
	PlayerPtr PlayerObj;

	GameObjectPtr cameraobj;


	//乱数
	std::random_device rd;

	TimeLimitPtr TimeManager;	//制限時間を測るマネージャー
	GameObjectPtr HPBarImg;		//HPバーの画像

	double MouceX, MouceY;		//マウスのポジション
	double oldX, oldY;			//マウスの昔の位置

	float enemycreate = 0;	//エネミーを生成するクールタイム
	float LimitTime = 180;	//残り時間

	bool BossShow = false;//ボスが出現する(true=出現　false=未出現)

};

#endif //MAINGAMESCENE_H_INCLUDED