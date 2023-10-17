//@file TutorialScene.h

#ifndef TUTORIALSCENE_H_INCLUDED
#define TUTORIALSCENE_H_INCLUDED
#include "Scene.h"
#include "../Component/Text.h"
#include "../Engine/Engine.h"

//チュートリアルシーン

class TutorialScene :public Scene
{
public:

	GLFWwindow* window = nullptr;

	TutorialScene() = default;				//コンストラクタ
	virtual ~TutorialScene() = default;		//デストラクタ

	virtual bool Initialize(Engine& engine)override;
	virtual void Update(Engine& engine, float deltaTime)override;

	//カメラの回転
	void CameraRot(GLFWwindow* window, GameObjectPtr camera, float deltaTime, double MouceX, double MouceY);

	//プレイヤーアップデート
	void PlayerUpdate(Engine& engine, PlayerPtr player, float deltaTime);

	//UI更新
	void UIUpdate();

	PlayerPtr PlayerObj;
	GameObjectPtr cameraobj;	//カメラ
	GameObjectPtr Select;		//選択画像のオブジェクト
	GameObjectPtr Pistole;		//ピストル画像のオブジェクトを
	GameObjectPtr Assault;		//アサルト画像のオブジェクトを
	GameObjectPtr ShotGun;		//ショットガン画像のオブジェクトを

	double MouceX, MouceY;		//マウスのポジション
	double oldX, oldY;			//マウスの昔の位置
	const float UIMARGINE = 150.0f;	//UI画像の幅


};

#endif //TUTORIALSCENE_H_INCLUDED
