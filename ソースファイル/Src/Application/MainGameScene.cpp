//@file MainGameScene.h 

#include "EasyAudio.h"
#include "MainGameScene.h"
#include "../Engine/Debug.h"
#include "../Engine/Collision.h"
#include "../Engine/AudioSettings.h"
#include "../Application/Boss.h"
#include "../Application/GameObject.h"
#include "../Application/JumpingEnemy.h"
#include "../Application/DeviationEnemy.h"
#include "../Component/Warp.h"
#include "../Component/Goal.h"
#include "../Component/Text.h"
#include "../Component/Chase.h"
#include "../Component/HPBar.h"
#include "../Component/Health.h"
#include "../Component/Player.h"
#include "../Component/RedHPBar.h"
#include "../Component/Collider.h"
#include "../Component/HitEffect.h"
#include "../Component/Explosion.h"
#include "../Component/UnderBoder.h"
#include "../Component/BulletMove.h"
#include "../Component/DropPowerUp.h"
#include "../Component/TreasureBox.h"
#include "../Component/MeshRenderer.h"
#include "../Component/DamageSource.h"
#include "../Component/MegaExplosion.h"
#include "../Component/BossExplosion.h"
#include "../Component/ParticleSystem.h"
using namespace VecMath;


//初期化処理
bool MainGameScene::Initialize(Engine& engine)
{
	//ゲームオブジェクトを削除
	engine.ClearGameObjectList();
	engine.ClearUILayers();
	engine.RemoveParticleEmitterAll();

	//乱数の生成
	engine.GetRandomGenerator().seed(rd());
	
	//ウィンドウの取得
	window = engine.GetWindow();
	
	float windowSize = engine.GetWindowSize().x;
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

	//ワープ（マップ作成中）
	{
		//ワープの数（２個１）
		const int WarpCount = 1;

		//ワープの設定
		for (int i = 0; i < WarpCount; ++i)
		{
			//数字を文字列に変換
			std::string No = std::to_string(i);
			std::string EnterName = "WarpEnter";	//入り口の名前
			std::string ExitName = "WarpExit";		//出口の名前

			//合体
			EnterName += No;
			ExitName += No;

			//ワープオブジェを検索し、対応させる
			auto warp = engine.SearchGameObject(EnterName);
			auto exit = engine.SearchGameObject(ExitName);
			auto enter = warp->AddComponent<Warp>();
			enter->SetExit(exit);

			auto ParticleObject = engine.Create<GameObject>(
				"particle explosion", warp->GetPos());

			
			auto ps = ParticleObject->AddComponent<ParticleSystem>();
			ps->Emitters.resize(1);
			auto& emitter = ps->Emitters[0];
			emitter.ep.ImagePath = "Res/UI/Last.tga";

			emitter.ep.RandomizeRotation = 1;		//角度をつける
			emitter.ep.RandomizeDirection = 1;
			emitter.ep.RandomizeSize = 1;			//大きさをランダムに
			emitter.ep.Duration = 0.1f;				//放出時間
			emitter.ep.EmissionsPerSecond = 100;	//秒間放出数

			emitter.pp.LifeTime = 0.4f;				//生存時間
			emitter.pp.color.Set({ 5, 1, 5.5f, 1 }, { 1, 2, 1.5f, 0 });	//色付け
			emitter.pp.scale.Set({ 0.3f,0.1f }, { 0.05f,0.02f });	//サイズを徐々にへ変更させる

			emitter.ep.Loop = true;

		}
	}

	//画面の中心にエイムを表示
	mouce = engine.AddUILayer("Res/UI/Bullet_1.tga", GL_LINEAR, 10);
	uimouce = engine.CreateUI<GameObject>(mouce, "aim", 367, 50);
	uimouce->AddSprite({ 0,0,1,1 });
	uimouce->SetPos(vec3(640, 360, 0)); 

	//ボス出現UI
	size_t bossUI = engine.AddUILayer("Res/UI/BossUI.tga", GL_LINEAR, 10);
	BossUI = engine.CreateUI<GameObject>(bossUI, "aim", 300, 500);
	BossUI->AddSprite({ 0,0,1,1 });
	BossUI->SetPos(vec3(windowSize * 0.5f, 680, 0));

	//フェードインをする
	engine.SetFadeRule("Res/Fade/fade_rule3.tga");
	engine.StartFadeIn();

	//セレクト画像の設定
	size_t SelectImg = engine.AddUILayer("Res/UI/Select.tga", GL_LINEAR, 10);
	Select = engine.CreateUI<GameObject>(SelectImg, "select", 0, 300);
	Select->AddSprite({ 0,0,1,1 });
	Select->SetPos(vec3{ 70,546,0 });

	//武器を持った時に背景を半透明にする
	size_t HalfImg = engine.AddUILayer("Res/UI/half.tga", GL_LINEAR, 10);
	for (int i = 0; i < 3; ++i)
	{
		Half[i] = engine.CreateUI<GameObject>(HalfImg, "select", 0, 300);
		Half[i]->AddSprite({ 0,0,1,1 });
		Half[i]->SetPos(vec3{ 70.0f + UIMARGINE * i,546,0 });
		Half[i]->SetAlpha(0);
	}

	//ピストルは最初から持ってるので
	Half[0]->SetAlpha(1);

	//武器フレーム画像の配置
	size_t WeaponFrameImg = engine.AddUILayer("Res/UI/WeaponFrame.tga", GL_LINEAR, 10);
	for (int i = 0; i < 3; i++)
	{
		GameObjectPtr WeaponFrame = engine.CreateUI<GameObject>(WeaponFrameImg, "frame", 0, 300);
		WeaponFrame->AddSprite({ 0,0,1,1 });
		WeaponFrame->SetPos(vec3{ 70.0f + UIMARGINE * i,546,0 });
	}

	//ピストル画像の設定
	size_t PistoleImg = engine.AddUILayer("Res/UI/Pistole.tga", GL_LINEAR, 10);
	Pistole = engine.CreateUI<GameObject>(PistoleImg, "pistle", 0, 300);
	Pistole->AddSprite({ 0,0,1,1 });
	Pistole->SetPos(vec3{ 70,546,0 });

	//アサルト画像の設定
	size_t AssaultImg = engine.AddUILayer("Res/UI/Assault.tga", GL_LINEAR, 10);
	Assault = engine.CreateUI<GameObject>(AssaultImg, "assault", 0, 300);
	Assault->AddSprite({ 0,0,1,1 });
	Assault->SetPos(vec3{ 220,546,0 });
	Assault->SetAlpha(0);

	//ショットガン画像の設定
	size_t ShotGunImg = engine.AddUILayer("Res/UI/ShotGun.tga", GL_LINEAR, 10);
	ShotGun = engine.CreateUI<GameObject>(ShotGunImg, "shotgun", 0, 300);
	ShotGun->AddSprite({ 0,0,1,1 });
	ShotGun->SetPos(vec3{ 370,546,0 });
	ShotGun->SetAlpha(0);


	//赤色のHPバーの設定
	size_t RedImg = engine.AddUILayer("Res/UI/RedBar.tga", GL_LINEAR, 10);
	GameObjectPtr RedHPImg = engine.CreateUI<GameObject>(RedImg, "red", 0, 300);
	RedHPImg->AddSprite({ 0,0,1,1 });
	RedHPImg->SetPos(vec3{ 0,500,0 });

	//HPバーの設定
	size_t HPImg = engine.AddUILayer("Res/UI/HPBar.tga", GL_LINEAR, 10);
	HPBarImg = engine.CreateUI<GameObject>(HPImg, "green", 0, 300);
	HPBarImg->AddSprite({ 0,0,1,1 });
	HPBarImg->SetPos(vec3{ 0,500,0 });


	//HPバーの移動
	auto barMove = HPBarImg->AddComponent<HPBar>();
	barMove->SetPlayer(PlayerObj);
	barMove->SetMaxHP(PlayerObj->GetHP());

	//赤色バーの移動
	auto redMove = RedHPImg->AddComponent<RedHPBar>();
	redMove->SetGreenBar(HPBarImg);


	//HPバーフレームの設定
	size_t HPFrameImg = engine.AddUILayer("Res/UI/HPFrame.tga", GL_LINEAR, 10);
	GameObjectPtr HPFrame = engine.CreateUI<GameObject>(HPFrameImg, "hpframe", 0, 500);
	HPFrame->AddSprite({ 0,0,1,1 });
	HPFrame->SetPos(vec3{ 220,650,0 });

	size_t MuscleImg = engine.AddUILayer("Res/UI/Mashule.tga", GL_LINEAR, 10);
	GameObjectPtr Muscle = engine.CreateUI<GameObject>(MuscleImg, "hpframe", 0, 500);
	Muscle->AddSprite({ 0,0,1,1 });
	Muscle->SetPos(vec3{ 1020,650,0 });


	//フォント画像
	const size_t textLayer = engine.AddUILayer("Res/UI/font_04_2.tga", GL_NEAREST, 10);

	//制限時間UI
	auto uiTime = engine.CreateUI<GameObject>(textLayer, "time", 540, 600);
	TimeManager = uiTime->AddComponent<TimeLimit>();
	TimeManager->SetTextComponent(uiTime->AddComponent<Text>());	//テキストコンポーネントを入れる

	//PowerUPUI
	auto uistate = engine.CreateUI<GameObject>(textLayer, "state", 540, 600);
	StateManager[0] = uistate->AddComponent<StateUI>();
	StateManager[0]->SetTextComponent(uistate->AddComponent<Text>());	//テキストコンポーネントを入れる
	uistate->SetPos({ 885,615,0 });

	//SpeedUPUI
	auto uispeed = engine.CreateUI<GameObject>(textLayer, "state", 540, 600);
	StateManager[1] = uispeed->AddComponent<StateUI>();
	StateManager[1]->SetState(1);
	StateManager[1]->SetTextComponent(uispeed->AddComponent<Text>());	//テキストコンポーネントを入れる
	uispeed->SetPos({ 1100,615,0 });
	
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
	//画像等の更新
	UIUpdate();

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
			BossObj->SetHP(180);
			
			auto ex = BossObj->AddComponent<BossExplosion>();

			//個別に色を変えるために、マテリアルをコピー
			CapsuleRenderer2->materials.push_back(
				std::make_shared<Mesh::Material>(*CapsuleRenderer2->mesh->materials[0]));

			//ダメージを与える対象
			auto damage = BossObj->AddComponent<DamageSource>();
			damage->targetName = "player";

			//当たり判定
			auto capcollider2 = BossObj->AddComponent<CapsuleCollider>();
			capcollider2->capsule.Seg.Start = vec3(0, 2, 0);
			capcollider2->capsule.Seg.End = vec3(0, -2, 0);
			capcollider2->capsule.Radius = std::min({
				CapsuleRenderer2->scale.x,
				CapsuleRenderer2->scale.y,
				CapsuleRenderer2->scale.z });

			//出現位置
			BossObj->SetPos({ 10,5,-30 });

			//ボスの出現を知らせるフラグ
			BossShow = true;
		}

		BossUI->SetDeadFlg(true);
	}
	else	//制限時間を減らしていく
	{
		//制限時間の適応
		LimitTime -= deltaTime * engine.SlowSpeed;
		TimeManager->SetLimitTime(LimitTime);
	}

	//マウスを取得してカメラを回転させる
	CameraRot(window, cameraobj, deltaTime, MouceX, MouceY);

	//プレイヤーのアップデート
	PlayerUpdate(engine, PlayerObj, deltaTime);

	//カメラをプレイヤーに合わせる
	cameraobj->SetPos({ PlayerObj->GetPos().x,PlayerObj->GetPos().y+0.5f,PlayerObj->GetPos().z });

	//cameraobj->SetPos(PlayerObj->GetPos() - PlayerObj->GetForward() * 2);
	//ｙ軸を合わせる
	PlayerObj->SetRotation(vec3(0,cameraobj->GetRotation().y,0));


	//ボスが出現したら、雑魚を沸かせない
	if (BossShow)
	{
		return;
	}

	//エネミーを生成するクールタイム
	EnemyCreate += deltaTime;

	//残り時間が少ない程大量に生成される
	if (EnemyCreate >= LimitTime * 0.08f)
	{
		EnemyCreate = 0;

		//乱数の生成
		const int rand = std::uniform_int_distribution<>(0, 100)(engine.GetRandomGenerator());//0~100

		if (rand == 100)//キングスライム
		{
			auto king = engine.Create<JumpingEnemy>("enemy");
			king->SetTarget(PlayerObj);

			//ｈPの設定
			const float hp = std::uniform_real_distribution<float>(200, 500)(engine.GetRandomGenerator());//50~100
			king->SetHP(hp);

			//モデル描画
			auto kingRenderer = king->AddComponent<MeshRenderer>();
			kingRenderer->mesh = engine.LoadOBJ("Res/Model/SlimeKing.obj");
			kingRenderer->scale = vec3(1);

			//出現位置
			const float x = std::uniform_real_distribution<float>(-100, 100)(engine.GetRandomGenerator());//-100~100
			const float z = std::uniform_real_distribution<float>(-100, 100)(engine.GetRandomGenerator());//-100~100
			king->SetPos(vec3(x, 1, z));

			//個別に色を変えるために、マテリアルをコピー
			for (const auto& e : kingRenderer->mesh->materials) {
				kingRenderer->materials.push_back(
					std::make_shared<Mesh::Material>(*e));
			}

			//本体の色を変える
			const float r = std::uniform_real_distribution<float>(0, 1)(engine.GetRandomGenerator());//0~1
			const float g = std::uniform_real_distribution<float>(0, 1)(engine.GetRandomGenerator());//0~1
			const float b = std::uniform_real_distribution<float>(0, 1)(engine.GetRandomGenerator());//0~1
			kingRenderer->materials[1]->baseColor = vec4(r, g, b, 1);


			//コライダーを割り当てる
			auto kingCollider = king->AddComponent<SphereCollider>();
			kingCollider->sphere.Center = kingRenderer->translate;
			kingCollider->sphere.Radius = std::max({
				kingRenderer->scale.x,
				kingRenderer->scale.y,
				kingRenderer->scale.z });

			auto he = king->AddComponent<HitEffect>();	//エフェクト
			auto hh = king->AddComponent<Health>();		//HP
			auto ee = king->AddComponent<Explosion>();	//爆発
			auto dp = king->AddComponent<DropPowerUp>();//アイテム
			dp->target = PlayerObj;
			dp->KingFlg = true;
		}
		else if (rand >= 50)//偏差撃ちをする敵
		{
			auto ene = engine.Create<DeviationEnemy>("enemy");
			ene->SetTarget(PlayerObj);

			//ｈPの設定
			const float hp = std::uniform_real_distribution<float>(2, 4)(engine.GetRandomGenerator());//2~4
			ene->SetHP(hp);

			//モデル読み込み
			auto eneRenderer = ene->AddComponent<MeshRenderer>();
			eneRenderer->mesh = engine.LoadOBJ("Res/Model/Drone_01.obj");
			eneRenderer->scale = vec3(1);

			//スポーン位置
			const float x = std::uniform_real_distribution<float>(-100, 100)(engine.GetRandomGenerator());//-100~100
			const float z = std::uniform_real_distribution<float>(-100, 100)(engine.GetRandomGenerator());//-100~100
			ene->SetPos(vec3(x, 1, z));

			//個別に色を変えるために、マテリアルをコピー
			for (const auto& e : eneRenderer->mesh->materials) {
				eneRenderer->materials.push_back(
					std::make_shared<Mesh::Material>(*e));
			}

			//本体の色を変える
			const float r = std::uniform_real_distribution<float>(0, 1)(engine.GetRandomGenerator());//0~1
			const float g = std::uniform_real_distribution<float>(0, 1)(engine.GetRandomGenerator());//0~1
			const float b = std::uniform_real_distribution<float>(0, 1)(engine.GetRandomGenerator());//0~1
			eneRenderer->materials[0]->baseColor = vec4(r, g, b, 1);


			//コライダーを割り当てる
			auto enecollider = ene->AddComponent<SphereCollider>();
			enecollider->sphere.Center = eneRenderer->translate;
			enecollider->sphere.Radius = std::max({
				eneRenderer->scale.x,
				eneRenderer->scale.y,
				eneRenderer->scale.z });


			auto he = ene->AddComponent<HitEffect>();	//エフェクト
			auto hh = ene->AddComponent<Health>();		//HP
			auto ee = ene->AddComponent<Explosion>();	//爆発
			auto dp = ene->AddComponent<DropPowerUp>();	//アイテム
			dp->target = PlayerObj;
		}
		else//スライム
		{
			auto ene = engine.Create<JumpingEnemy>("enemy");
			ene->SetTarget(PlayerObj);
			
			//ｈPの設定
			const float hp = std::uniform_real_distribution<float>(2, 3)(engine.GetRandomGenerator());//2~3
			ene->SetHP(hp);
			
			//モデルの読み込み
			auto eneRenderer = ene->AddComponent<MeshRenderer>();
			eneRenderer->mesh = engine.LoadOBJ("Res/Model/Slime_02_blender.obj");//ウサギっぽいスライム
			eneRenderer->scale = vec3(1);

			//スポーン位置
			const float x = std::uniform_real_distribution<float>(-100, 100)(engine.GetRandomGenerator());//-100~100
			const float z = std::uniform_real_distribution<float>(-100, 100)(engine.GetRandomGenerator());//-100~100
			ene->SetPos(vec3(x, 1, z));

			//個別に色を変えるために、マテリアルをコピー
			for (const auto& e : eneRenderer->mesh->materials) {
				eneRenderer->materials.push_back(
					std::make_shared<Mesh::Material>(*e));
			}

			//本体の色を変える
			const float r = std::uniform_real_distribution<float>(0, 1)(engine.GetRandomGenerator());//0~1
			const float g = std::uniform_real_distribution<float>(0, 1)(engine.GetRandomGenerator());//0~1
			const float b = std::uniform_real_distribution<float>(0, 1)(engine.GetRandomGenerator());//0~1
			eneRenderer->materials[1]->baseColor = vec4(r, g, b, 1);


			//コライダーを割り当てる
			auto enecollider = ene->AddComponent<SphereCollider>();
			enecollider->sphere.Center = eneRenderer->translate;
			enecollider->sphere.Radius = std::max({
				eneRenderer->scale.x,
				eneRenderer->scale.y,
				eneRenderer->scale.z });

			auto hh = ene->AddComponent<Health>();			//HP
			auto co = ene->AddComponent<HitEffect>();		//エフェクト
			auto ee = ene->AddComponent<MegaExplosion>();	//爆発
			auto pp = ene->AddComponent<DropPowerUp>();		//アイテム
			pp->target = PlayerObj;

		}
	}


}

void MainGameScene::CameraRot(GLFWwindow* window, GameObjectPtr camera, float deltaTime, double MouceX, double MouceY)
{
	glfwGetCursorPos(window, &MouceX, &MouceY);//XYにマウス位置を格納

	//感度
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

	//古い位置を更新
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
	PlayerObj->SetHP(PlayerObj->GetMaxHP());

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

void MainGameScene::UIUpdate()
{
	//一番左の定位置
	vec3 Pos = vec3{ 70,546,0 };

	StateManager[0]->SetLv(PlayerObj->GetShotDamage());
	StateManager[1]->SetLv(PlayerObj->GetMoveSpeed());
	
	const float NormalSize = 1.0f;	//通常サイズ
	const float MinimamSize = 0.6f;	//小さいサイズ

	switch (PlayerObj->GetShotStyle())
	{
	case 0:
		//セレクト画像を移動させる
		Select->SetPos(Pos);

		//選択具合で透明度を変える
		Pistole->SetAlpha(1.0f);
		Assault->SetAlpha(0.5f);
		ShotGun->SetAlpha(0.5f);

		Pistole->SetScale(vec3(NormalSize));
		//選択されていないものはサイズを小さくする
		Assault->SetScale(vec3(MinimamSize));
		ShotGun->SetScale(vec3(MinimamSize));

		break;

	case 1:
		//セレクト画像を移動させる
		Select->SetPos(vec3{ Pos.x + UIMARGINE,Pos.y,Pos.z });

		//選択具合で透明度を変える
		Pistole->SetAlpha(0.5f);
		Assault->SetAlpha(1.0f);
		ShotGun->SetAlpha(0.5f);

		Assault->SetScale(vec3(NormalSize));
		//選択されていないものはサイズを小さくする
		Pistole->SetScale(vec3(MinimamSize));
		ShotGun->SetScale(vec3(MinimamSize));

		break;

	case 2:
		//セレクト画像を移動させる
		Select->SetPos(vec3{ Pos.x + UIMARGINE * 2,Pos.y,Pos.z });

		//選択具合で透明度を変える
		Pistole->SetAlpha(0.5f);
		Assault->SetAlpha(0.5f);
		ShotGun->SetAlpha(1.0f);

		ShotGun->SetScale(vec3(NormalSize));
		//選択されていないものはサイズを小さくする
		Pistole->SetScale(vec3(MinimamSize));
		Assault->SetScale(vec3(MinimamSize));

		break;
	default:
		break;
	}

	//プレイヤーの武器の取得状況に合わせて透明度を変える
	if (!PlayerObj->GetShooterFlg())
	{
		Assault->SetAlpha(0.0f);	//持ってない
	}
	else
	{
		Half[1]->SetAlpha(1);		//持ってる
	}

	if (!PlayerObj->GetShotGunFlg())
	{
		ShotGun->SetAlpha(0.0f);	//持ってない
	}
	else
	{
		Half[2]->SetAlpha(1);		//持ってる
	}


}

