//@file TutorialScene.h 

#include "TutorialScene.h"
#include "../Component/MeshRenderer.h"
#include "../Engine/Collision.h"
#include "../Component/Collider.h"
#include "../Engine/Debug.h"
#include "../Component/Chase.h"
#include "../Component/DamageSource.h"
#include "../Component/Player.h"
#include "../Component/BulletMove.h"
#include "../Component/Goal.h"
#include "../Component/Explosion.h"
#include "../Component/Health.h"
#include "../Component/MegaExplosion.h"
#include "../Component/DropPowerUp.h"
#include "../Application/DeviationEnemy.h"
#include "../Application/JumpingEnemy.h"
#include "../Application/Boss.h"
#include "../Engine/AudioSettings.h"
#include "../Component/Text.h"
#include "../Component/UnderBoder.h"
#include "../Component/TreasureBox.h"
#include "../Component/HPBar.h"
#include "../Component/RedHPBar.h"
#include "EasyAudio.h"
using namespace VecMath;


//初期化処理
bool TutorialScene::Initialize(Engine& engine)
{
	//ゲームオブジェクトを削除
	engine.ClearGameObjectList();
	engine.ClearUILayers();
	engine.RemoveParticleEmitterAll();

	//ウィンドウの取得
	window = engine.GetWindow();

	//カメラの取得
	cameraobj = engine.GetMainCameraObject();
	cameraobj->SetRotation({ 0,0,-1 });

	//マウスをウィンドウに固定し表示させない
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//マップの読み込み
	auto gameObjList = engine.LoadGameObjectMap("Res/TutorialMap.json", vec3(0, 0, 0), vec3(1));

	//見た目を描画
	PlayerObj = engine.Create<Player>("player");
	auto CapsuleRenderer = PlayerObj->AddComponent<MeshRenderer>();
	CapsuleRenderer->mesh = engine.LoadOBJ("Capsule2");
	CapsuleRenderer->scale = vec3(1);
	PlayerObj->name = "player";

	//HP
	PlayerObj->SetHP(5);

	//カプセルの当たり判定を割り当て
	auto capcollider = PlayerObj->AddComponent<CapsuleCollider>();
	capcollider->capsule.Seg.Start = vec3(0, 1, 0);
	capcollider->capsule.Seg.End = vec3(0, -1, 0);
	capcollider->capsule.Radius = std::min({
		CapsuleRenderer->scale.x,
		CapsuleRenderer->scale.y,
		CapsuleRenderer->scale.z });

	//位置を調整
	PlayerObj->SetPos({ 0,10,0 });
	PlayerObj->SetRotation({ 0, 0, 0 });

	cameraobj->SetRotation({ 0,radians(180),0 });
	//宝箱の生成
	auto treasure = engine.SearchGameObject("Golden_Chest");
	auto  chest = treasure->AddComponent<TreasureBox>();
	chest->SetTarget(PlayerObj);
	chest->SetTutorialFlg(true);

	auto Treasure = engine.SearchGameObject("Golden_Chest (1)");
	auto  Chest = Treasure->AddComponent<TreasureBox>();
	Chest->SetTarget(PlayerObj);
	Chest->SetTutorialFlg(true);

	//ゴールの生成
	auto GoalObj = engine.SearchGameObject("Goal");
	auto goal = GoalObj->AddComponent<Goal>();
	goal->SetEngine(&engine);
	goal->SetTarget(PlayerObj);

	//画面の中心にエイムを表示
	size_t mouce = engine.AddUILayer("Res/Bullet_1.tga", GL_LINEAR, 10);
	GameObjectPtr uimouce = engine.CreateUI<GameObject>(mouce, "aim", 367, 50);
	uimouce->AddSprite({ 0,0,1,1 });
	uimouce->SetPos(vec3(640, 360, 0));


		//ピストル画像の設定
	size_t SelectImg = engine.AddUILayer("Res/Select.tga", GL_LINEAR, 10);
	Select = engine.CreateUI<GameObject>(SelectImg, "hp", 0, 300);
	Select->AddSprite({ 0,0,1,1 });
	Select->SetPos(vec3{ 70,546,0 });
	
	//武器フレーム画像の配置
	size_t WeaponFrameImg = engine.AddUILayer("Res/WeaponFrame.tga", GL_LINEAR, 10);
	for (int i = 0; i < 3; i++)
	{
		GameObjectPtr WeaponFrame = engine.CreateUI<GameObject>(WeaponFrameImg, "hp", 0, 300);
		WeaponFrame->AddSprite({ 0,0,1,1 });
		WeaponFrame->SetPos(vec3{ 70.0f + UIMARGINE * i,546,0 });
	}

	//ピストル画像の設定
	size_t PistoleImg = engine.AddUILayer("Res/Pistole.tga", GL_LINEAR, 10);
	Pistole = engine.CreateUI<GameObject>(PistoleImg, "hp", 0, 300);
	Pistole->AddSprite({ 0,0,1,1 });
	Pistole->SetPos(vec3{ 70,546,0 });

	//アサルト画像の設定
	size_t AssaultImg = engine.AddUILayer("Res/Assault.tga", GL_LINEAR, 10);
	Assault = engine.CreateUI<GameObject>(AssaultImg, "hp", 0, 300);
	Assault->AddSprite({ 0,0,1,1 });
	Assault->SetPos(vec3{ 220,546,0 });
	Assault->SetAlpha(0);

	//ショットガン画像の設定
	size_t ShotGunImg = engine.AddUILayer("Res/ShotGun.tga", GL_LINEAR, 10);
	ShotGun = engine.CreateUI<GameObject>(ShotGunImg, "hp", 0, 300);
	ShotGun->AddSprite({ 0,0,1,1 });
	ShotGun->SetPos(vec3{ 370,546,0 });
	ShotGun->SetAlpha(0);

	//フェードインをする
	engine.SetFadeRule("Res/fade_rule.tga");
	engine.StartFadeIn();


	//落下した時の床作成
	auto UnderGround = engine.Create<GameObject>("Ground");
	UnderGround->name = "floor";
	UnderGround->SetPos({ 0,-40,0 });
	UnderGround->SetMoveFlg(false);

	//コライダーを割り当てる
	auto Gcollider = UnderGround->AddComponent<BoxCollider>();
	Gcollider->box.Scale = vec3({ 250,1,350 });
	auto fall = UnderGround->AddComponent<UnderBorder>();

	//BGMを再生
	Audio::Play(AudioPlayer::bgm, BGM::stage01, 1, true);

	//強化パーティクルを配置s
	for (int i = 0; i < 3; i++)
	{
		auto ParticleObje = engine.Create<ItemObject>(
			"particle explosion", vec3{ -10.0f + i * 10.0f,1.0f,356.0f });
		ParticleObje->player = PlayerObj;	//対象の設a定
		ParticleObje->SetSpot(i);		//強化する部位の設定

		auto patcollider = ParticleObje->AddComponent<CapsuleCollider>();
		patcollider->capsule.Seg.Start = vec3(0, 1, 0);
		patcollider->capsule.Seg.End = vec3(0, -1, 0);
		patcollider->capsule.Radius = 1.0f;

		auto ps = ParticleObje->AddComponent<ParticleSystem>();
		ps->Emitters.resize(1);
		auto& emitter = ps->Emitters[0];
		emitter.ep.ImagePath = "Res/face.tga";
		emitter.ep.Duration = 0.5f;				//放出時間
		emitter.ep.EmissionsPerSecond = 10;	//秒間放出数
		emitter.ep.RandomizeRotation = 1;		//角度をつける
		emitter.ep.RandomizeDirection = 0;
		emitter.ep.RandomizeSize = 1;			//大きさをランダムに
		emitter.pp.LifeTime = 0.4f;				//生存時間
		emitter.pp.rotation.Set(90.0f, 0);

		if (i == 0)
		{
			emitter.pp.color.Set({ 255, 1, 1, 1 }, { 1, 1, 1.5f, 0 });	//色付け
		}
		else if (i == 1)
		{
			emitter.pp.color.Set({ 2, 255, 1, 1 }, { 1, 1, 1.5f, 0 });	//色付け
		}
		else if (i == 2)
		{
			emitter.pp.color.Set({ 2, 1, 255.5f, 1 }, { 1, 1, 1.5f, 0 });	//色付け
		}
		emitter.pp.scale.Set({ 0.01f,0.01f }, { 0.05f,0.05f });	//サイズを徐々にへ変更させる
		emitter.pp.radial.Set(0, 10);			//角度方向に速度をつける

		emitter.ep.Loop = true;	//ループ再生させる

	}

	//変数の初期化
	MouceX = 0;
	MouceY = 0;
	oldX = 0;
	oldY = 0;
	return true;
}

//更新処理
void TutorialScene::Update(Engine& engine, float deltaTime)
{
	UIUpdate();

	//マウスを取得してカメラを回転させる
	CameraRot(window, cameraobj, deltaTime, MouceX, MouceY);

	PlayerUpdate(engine, PlayerObj, deltaTime);

	cameraobj->SetPos(PlayerObj->GetPos());

	//ｙ軸を合わせる
	PlayerObj->SetRotation(vec3(0, cameraobj->GetRotation().y, 0));
}

void TutorialScene::CameraRot(GLFWwindow* window, GameObjectPtr camera, float deltaTime, double MouceX, double MouceY)
{
	glfwGetCursorPos(window, &MouceX, &MouceY);//XYにマウス位置を格納

	const float scensi = 8.0f;


	//横の回転
	camera->AddRotaion(vec3(0,
		static_cast<float>((oldX - MouceX)) * scensi * 0.01f * deltaTime,
		0));

	//縦の回転
	camera->AddRotaion(vec3(static_cast<float>((oldY - MouceY)) * scensi * 0.01f * deltaTime,
		0, 0));

	//上下の視点移動に制限をつける
	camera->SetRotation(vec3(std::clamp(camera->GetRotation().x, -1.5f, 1.5f),
		camera->GetRotation().y,
		camera->GetRotation().z));


	oldX = MouceX;
	oldY = MouceY;
}

void TutorialScene::PlayerUpdate(Engine& engine, PlayerPtr player, float deltaTime)
{

	//今向いている向きを取得
	float radY = cameraobj->GetRotation().y;
	float radX = cameraobj->GetRotation().x;


	//正面ベクトル
	vec3 forward = vec3(1);

	//x軸回転させる
	forward *= vec3(1,
		sin(radX),
		cos(radX));

	//y軸で回転をさせる
	forward *= vec3(sin(radY),
		1,
		cos(radY));

	//反転させる
	forward *= vec3{ -1,1,-1 };

	//正規化する
	forward = normalize(forward);

	//外積で、右方向ベクトルを取得
	vec3 right = cross(forward, vec3{ 0,1,0 });

	//正規化する
	right = normalize(right);

	//設定する
	player->SetForward(forward);
	player->SetRight(right);


}

void TutorialScene::UIUpdate()
{
	//一番左の定位置
	vec3 Pos = vec3{ 70,546,0 };

	switch (PlayerObj->GetShotStyle())
	{
	case 0:
		//セレクト画像を移動させる
		Select->SetPos(Pos);

		//選択具合で透明度を変える
		Pistole->SetAlpha(1.0f);
		Assault->SetAlpha(0.5f);
		ShotGun->SetAlpha(0.5f);
		break;

	case 1:
		//セレクト画像を移動させる
		Select->SetPos(vec3{ Pos.x + UIMARGINE,Pos.y,Pos.z });

		//選択具合で透明度を変える
		Pistole->SetAlpha(0.5f);
		Assault->SetAlpha(1.0f);
		ShotGun->SetAlpha(0.5f);

		break;

	case 2:
		//セレクト画像を移動させる
		Select->SetPos(vec3{ Pos.x + UIMARGINE * 2,Pos.y,Pos.z });

		//選択具合で透明度を変える
		Pistole->SetAlpha(0.5f);
		Assault->SetAlpha(0.5f);
		ShotGun->SetAlpha(1.0f);

		break;
	default:
		break;
	}

	//プレイヤーの武器の取得状況に合わせて透明度を変える
	if (!PlayerObj->GetShooterFlg())
	{
		Assault->SetAlpha(0.0f);
	}
	if (!PlayerObj->GetShotGunFlg())
	{
		ShotGun->SetAlpha(0.0f);
	}


}

