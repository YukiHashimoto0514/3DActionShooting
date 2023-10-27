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

	size_t mouce;			//照準画像
	GameObjectPtr uimouce;	//マウスオブジェ

private:

	void PlayerInit(Engine& engine);	//プレイヤー更新
	void UIUpdate();	//UI更新

	GameObjectPtr BossObj;
	PlayerPtr PlayerObj;

	GameObjectPtr cameraobj;	//カメラ
	GameObjectPtr Select;		//選択画像のオブジェクト
	GameObjectPtr Half[3];		//背景の半透明のオブジェクト
	GameObjectPtr Pistole;		//ピストル画像のオブジェクトを
	GameObjectPtr Assault;		//アサルト画像のオブジェクトを
	GameObjectPtr ShotGun;		//ショットガン画像のオブジェクトを

	//乱数
	std::random_device rd;

	TimeLimitPtr TimeManager;	//制限時間を測るマネージャー
	GameObjectPtr HPBarImg;		//HPバーの画像

	double MouceX = 0;	//マウスのポジション
	double MouceY = 0;	//マウスのポジション
	double oldX = 0;	//マウスの昔の位置
	double oldY = 0;	//マウスの昔の位置

	float EnemyCreate = 0;	//エネミーを生成するクールタイム
	float LimitTime = 1;	//残り時間

	const float UIMARGINE = 150.0f;	//UI画像の幅

	bool BossShow = false;//ボスが出現する(true=出現　false=未出現) 

};

#endif //MAINGAMESCENE_H_INCLUDED