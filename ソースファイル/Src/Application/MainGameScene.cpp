//@file MainGameScene.h 

#include "MainGameScene.h"
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
bool MainGameScene::Initialize(Engine& engine)
{
	//ゲームオブジェクトを削除
	engine.ClearGameObjectList();
	engine.ClearUILayers();

	//乱数の生成
	engine.GetRandomGenerator().seed(rd());
	
	//ウィンドウの取得
	window = engine.GetWindow();

	//カメラの取得
	cameraobj = engine.GetMainCameraObject();
	cameraobj->SetRotation({ 0,0,-1 });

	//マウスをウィンドウに固定し表示させない
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//マップの読み込み
	auto gameObjList = engine.LoadGameObjectMap("Res/mapmodel.json", vec3(0, 0, 0), vec3(1));

	//プレイヤーの情報
	PlayerInit(engine);

	//宝箱の生成
	auto treasure = engine.SearchGameObject("Golden_Chest");
	auto  chest = treasure->AddComponent<TreasureBox>();
	chest->SetTarget(PlayerObj);

	auto Treasure = engine.SearchGameObject("Golden_Chest (1)");
	auto  Chest = Treasure->AddComponent<TreasureBox>();
	Chest->SetTarget(PlayerObj);

	//画面の中心にエイムを表示
	mouce = engine.AddUILayer("Res/Bullet_1.tga", GL_LINEAR, 10);
	uimouce = engine.CreateUI<GameObject>(mouce, "aim", 367, 50);
	uimouce->AddSprite({ 0,0,1,1 });
	uimouce->SetPos(vec3(640, 360, 0)); 

	//フェードインをする
	engine.SetFadeRule("Res/fade_rule3.tga");
	engine.StartFadeIn();


	//赤色のHPバーの設定
	size_t RedImg = engine.AddUILayer("Res/RedBar.tga", GL_LINEAR, 10);
	GameObjectPtr RedHPImg = engine.CreateUI<GameObject>(RedImg, "hp", 0, 300);
	RedHPImg->AddSprite({ 0,0,1,1 });
	RedHPImg->SetPos(vec3{ 0,500,0 });

	//HPバーの設定
	size_t HPImg = engine.AddUILayer("Res/HPBar.tga", GL_LINEAR, 10);
	HPBarImg = engine.CreateUI<GameObject>(HPImg, "hp", 0, 300);
	HPBarImg->AddSprite({ 0,0,1,1 });
	HPBarImg->SetPos(vec3{ 0,500,0 });


	//HPバーの移動
	auto aaa = HPBarImg->AddComponent<HPBar>();
	aaa->SetPlayer(PlayerObj);
	aaa->SetMaxHP(PlayerObj->GetHP());

	//赤色バーの移動
	auto bbb = RedHPImg->AddComponent<RedHPBar>();
	bbb->SetGreenBar(HPBarImg);


	//HPバーフレームの設定
	size_t HPFrameImg = engine.AddUILayer("Res/HPFrame.tga", GL_LINEAR, 10);
	GameObjectPtr HPFrame = engine.CreateUI<GameObject>(HPFrameImg, "hpframe", 0, 500);
	HPFrame->AddSprite({ 0,0,1,1 });
	HPFrame->SetPos(vec3{ 230,650,0 });


	//フォント画像
	const size_t textLayer = engine.AddUILayer("Res/font_04_2.tga", GL_NEAREST, 10);

	//制限時間UI
	auto uiTime = engine.CreateUI<GameObject>(textLayer, "text", 940, 600);
	TimeManager = uiTime->AddComponent<TimeLimit>();
	TimeManager->SetTextComponent(uiTime->AddComponent<Text>());	//テキストコンポーネントを入れる
	
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

	//変数の初期化
	MouceX = 0;
	MouceY = 0;
	oldX = 0;
	oldY = 0;
	return true;
}

//更新処理
void MainGameScene::Update(Engine& engine,float deltaTime)
{
	//残り時間が無くなったら
	if (LimitTime < 0)
	{
		if (!BossShow)
		{
			//ボスの出現
			BossObj = engine.Create<Boss>("enemy");
			auto CapsuleRenderer2 = BossObj->AddComponent<MeshRenderer>();
			CapsuleRenderer2->mesh = engine.LoadOBJ("Capsule2");
			CapsuleRenderer2->scale = vec3(2);
			BossObj->SetHP(10);

			auto ex = BossObj->AddComponent<Explosion>();
			auto h = BossObj->AddComponent<Health>();

			//個別に色を変えるために、マテリアルをコピー
			CapsuleRenderer2->materials.push_back(
				std::make_shared<Mesh::Material>(*CapsuleRenderer2->mesh->materials[0]));


			auto t = BossObj->AddComponent<DamageSource>();
			t->targetName = "player";

			auto capcollider2 = BossObj->AddComponent<CapsuleCollider>();
			capcollider2->capsule.Seg.Start = vec3(0, 2, 0);
			capcollider2->capsule.Seg.End = vec3(0, -2, 0);
			capcollider2->capsule.Radius = std::min({
				CapsuleRenderer2->scale.x,
				CapsuleRenderer2->scale.y,
				CapsuleRenderer2->scale.z });

			BossObj->SetPos({ 10,5,-30 });


			BossShow = true;
		}
	}
	else	//制限時間を減らしていく
	{
		LimitTime -= deltaTime * engine.SlowSpeed;
		TimeManager->SetLimitTime(LimitTime);
	}

	//マウスを取得してカメラを回転させる
	CameraRot(window, cameraobj, deltaTime, MouceX, MouceY);

	
	PlayerUpdate(engine, PlayerObj, deltaTime);

	cameraobj->SetPos(PlayerObj->GetPos());

	//ｙ軸を合わせる
	PlayerObj->SetRotation(vec3(0,cameraobj->GetRotation().y,0));



	//ボスが出現したら、雑魚を沸かせない
	if (BossShow)
	{
		return;
	}

	enemycreate += deltaTime;

	//3秒に一回敵を生成（雑処理）
	if (enemycreate >= 3)
	{
		enemycreate = 0;

		//乱数の生成
		const int rand = std::uniform_int_distribution<>(0, 100)(engine.GetRandomGenerator());//0~100

		if (rand == 100)//キングスライム
		{
			auto king = engine.Create<JumpingEnemy>("enemy");
			king->SetTarget(PlayerObj);

			auto kingRenderer = king->AddComponent<MeshRenderer>();
			kingRenderer->mesh = engine.LoadOBJ("Res/Model/SlimeKing.obj");
			
			kingRenderer->scale = vec3(1);
			const float x = std::uniform_real_distribution<>(-100, 100)(engine.GetRandomGenerator());//-100~100
			const float z = std::uniform_real_distribution<>(-100, 100)(engine.GetRandomGenerator());//-100~100
			king->SetPos(vec3(x, 1, z));

			//個別に色を変えるために、マテリアルをコピー
			for (const auto& e : kingRenderer->mesh->materials) {
				kingRenderer->materials.push_back(
					std::make_shared<Mesh::Material>(*e));
			}

			//本体の色を変える
			const double r = std::uniform_real_distribution<>(0, 1)(engine.GetRandomGenerator());//0~1
			const double g = std::uniform_real_distribution<>(0, 1)(engine.GetRandomGenerator());//0~1
			const double b = std::uniform_real_distribution<>(0, 1)(engine.GetRandomGenerator());//0~1
			kingRenderer->materials[1]->baseColor = vec4(r, g, b, 1);


			//コライダーを割り当てる
			auto kingcollider = king->AddComponent<SphereCollider>();
			kingcollider->sphere.Center = kingRenderer->translate;
			kingcollider->sphere.Radius = std::max({
				kingRenderer->scale.x,
				kingRenderer->scale.y,
				kingRenderer->scale.z });

			auto hh = king->AddComponent<Health>();		//HP
			auto ee = king->AddComponent<Explosion>();	//爆発
			auto pp = king->AddComponent<DropPowerUp>();	//アイテム
			pp->target = PlayerObj;
		}
		else if(rand>=50)//偏差撃ちをする敵
		{
			auto ene = engine.Create<DeviationEnemy>("enemy");
			ene->SetTarget(PlayerObj);

			//ｈPの設定
			ene->SetHP(3);

			auto eneRenderer = ene->AddComponent<MeshRenderer>();
			eneRenderer->mesh = engine.LoadOBJ("Res/Model/Drone_01.obj");
			eneRenderer->scale = vec3(1);

			//スポーン位置
			const float x = std::uniform_real_distribution<>(-100, 100)(engine.GetRandomGenerator());//-100~100
			const float z = std::uniform_real_distribution<>(-100, 100)(engine.GetRandomGenerator());//-100~100
			ene->SetPos(vec3(x, 1, z));

			//個別に色を変えるために、マテリアルをコピー
			for (const auto& e : eneRenderer->mesh->materials) {
				eneRenderer->materials.push_back(
					std::make_shared<Mesh::Material>(*e));
			}

			//本体の色を変える
			const double r = std::uniform_real_distribution<>(0, 1)(engine.GetRandomGenerator());//0~1
			const double g = std::uniform_real_distribution<>(0, 1)(engine.GetRandomGenerator());//0~1
			const double b = std::uniform_real_distribution<>(0, 1)(engine.GetRandomGenerator());//0~1
			eneRenderer->materials[0]->baseColor = vec4(r, g, b, 1);


			//コライダーを割り当てる
			auto enecollider = ene->AddComponent<SphereCollider>();
			enecollider->sphere.Center = eneRenderer->translate;
			enecollider->sphere.Radius = std::max({
				eneRenderer->scale.x,
				eneRenderer->scale.y,
				eneRenderer->scale.z });

			auto hh = ene->AddComponent<Health>();		//HP
			auto ee = ene->AddComponent<Explosion>();	//爆発
			auto pp = ene->AddComponent<DropPowerUp>();//アイテム
			pp->target = PlayerObj;
		}
		else//スライム
		{
			auto ene = engine.Create<JumpingEnemy>("enemy");
			ene->SetTarget(PlayerObj);

			auto eneRenderer = ene->AddComponent<MeshRenderer>();
			eneRenderer->mesh = engine.LoadOBJ("Res/Model/Slime_02_blender.obj");//ウサギっぽいスライム
			eneRenderer->scale = vec3(1);

			//スポーン位置
			const float x = std::uniform_real_distribution<>(-100, 100)(engine.GetRandomGenerator());//-100~100
			const float z = std::uniform_real_distribution<>(-100, 100)(engine.GetRandomGenerator());//-100~100
			ene->SetPos(vec3(x, 1, z));

			//個別に色を変えるために、マテリアルをコピー
			for (const auto& e : eneRenderer->mesh->materials) {
				eneRenderer->materials.push_back(
					std::make_shared<Mesh::Material>(*e));
			}

			//本体の色を変える
			const double r = std::uniform_real_distribution<>(0, 1)(engine.GetRandomGenerator());//0~1
			const double g = std::uniform_real_distribution<>(0, 1)(engine.GetRandomGenerator());//0~1
			const double b = std::uniform_real_distribution<>(0, 1)(engine.GetRandomGenerator());//0~1
			eneRenderer->materials[1]->baseColor = vec4(r, g, b, 1);


			//コライダーを割り当てる
			auto enecollider = ene->AddComponent<SphereCollider>();
			enecollider->sphere.Center = eneRenderer->translate;
			enecollider->sphere.Radius = std::max({
				eneRenderer->scale.x,
				eneRenderer->scale.y,
				eneRenderer->scale.z });

			auto hh = ene->AddComponent<Health>();			//HP
			auto ee = ene->AddComponent<MegaExplosion>();	//爆発
			auto pp = ene->AddComponent<DropPowerUp>();		//アイテム
			pp->target = PlayerObj;

		}
	}


}

void MainGameScene::CameraRot(GLFWwindow* window, GameObjectPtr camera, float deltaTime, double MouceX, double MouceY)
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

void MainGameScene::PlayerUpdate(Engine& engine, PlayerPtr player, float deltaTime)
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

void MainGameScene::PlayerInit(Engine& engine)
{
	//見た目を描画
	PlayerObj = engine.Create<Player>("player");
	auto CapsuleRenderer = PlayerObj->AddComponent<MeshRenderer>();
	CapsuleRenderer->mesh = engine.LoadOBJ("Capsule2");
	CapsuleRenderer->scale = vec3(1);
	PlayerObj->name = "player";

	//HP
	PlayerObj->SetHP(5);

	//個別に色を変えるために、マテリアルをコピー
	CapsuleRenderer->materials.push_back(
		std::make_shared<Mesh::Material>(*CapsuleRenderer->mesh->materials[0]));

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
	PlayerObj->SetRotation({ 90, 0, -90 });
}

