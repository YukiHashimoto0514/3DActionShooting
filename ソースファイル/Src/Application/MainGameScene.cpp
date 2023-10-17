//@file MainGameScene.h 

#include "EasyAudio.h"
#include "MainGameScene.h"
#include "../Engine/Debug.h"
#include "../Engine/Collision.h"
#include "../Engine/AudioSettings.h"
#include "../Application/Boss.h"
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

using namespace VecMath;


//����������
bool MainGameScene::Initialize(Engine& engine)
{
	//�Q�[���I�u�W�F�N�g���폜
	engine.ClearGameObjectList();
	engine.ClearUILayers();
	engine.RemoveParticleEmitterAll();

	//�����̐���
	engine.GetRandomGenerator().seed(rd());
	
	//�E�B���h�E�̎擾
	window = engine.GetWindow();

	//�J�����̎擾
	cameraobj = engine.GetMainCameraObject();
	cameraobj->SetRotation({ 0,0,-1 });

	//�}�E�X���E�B���h�E�ɌŒ肵�\�������Ȃ�
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//�}�b�v�̓ǂݍ���
	auto gameObjList = engine.LoadGameObjectMap("Res/mapmodel.json", vec3(0, 0, 0), vec3(1));

	//�v���C���[�̏��
	PlayerInit(engine);

	//�󔠂̐���
	auto treasure = engine.SearchGameObject("Golden_Chest");
	auto  chest = treasure->AddComponent<TreasureBox>();
	chest->SetTarget(PlayerObj);

	auto Treasure = engine.SearchGameObject("Golden_Chest (1)");
	auto  Chest = Treasure->AddComponent<TreasureBox>();
	Chest->SetTarget(PlayerObj);

	//���[�v�i�}�b�v�쐬���j
	{
		////���[�v�̐��i�Q�P�j
		//const int WarpCount = 2;

		////���[�v�̐ݒ�
		//for (int i = 0; i < WarpCount; ++i)
		//{
		//	//�����𕶎���ɕϊ�
		//	std::string No = std::to_string(i);
		//	std::string EnterName = "WarpEnter";	//������̖��O
		//	std::string ExitName = "WarpExit";		//�o���̖��O

		//	//����
		//	EnterName += No;
		//	ExitName += No;

		//	//���[�v�I�u�W�F���������A�Ή�������
		//	auto warp = engine.SearchGameObject(EnterName);
		//	auto exit = engine.SearchGameObject(ExitName);
		//	auto enter = warp->AddComponent<Warp>();
		//	enter->SetExit(exit);

		//}
	}

	//��ʂ̒��S�ɃG�C����\��
	mouce = engine.AddUILayer("Res/Bullet_1.tga", GL_LINEAR, 10);
	uimouce = engine.CreateUI<GameObject>(mouce, "aim", 367, 50);
	uimouce->AddSprite({ 0,0,1,1 });
	uimouce->SetPos(vec3(640, 360, 0)); 

	//�t�F�[�h�C��������
	engine.SetFadeRule("Res/fade_rule3.tga");
	engine.StartFadeIn();

	//�Z���N�g�摜�̐ݒ�
	size_t SelectImg = engine.AddUILayer("Res/Select.tga", GL_LINEAR, 10);
	Select = engine.CreateUI<GameObject>(SelectImg, "select", 0, 300);
	Select->AddSprite({ 0,0,1,1 });
	Select->SetPos(vec3{ 70,546,0 });
	
	//����t���[���摜�̔z�u
	size_t WeaponFrameImg = engine.AddUILayer("Res/WeaponFrame.tga", GL_LINEAR, 10);
	for (int i = 0; i < 3; i++)
	{
		GameObjectPtr WeaponFrame = engine.CreateUI<GameObject>(WeaponFrameImg, "frame", 0, 300);
		WeaponFrame->AddSprite({ 0,0,1,1 });
		WeaponFrame->SetPos(vec3{ 70.0f + UIMARGINE * i,546,0 });
	}

	//�s�X�g���摜�̐ݒ�
	size_t PistoleImg = engine.AddUILayer("Res/Pistole.tga", GL_LINEAR, 10);
	Pistole = engine.CreateUI<GameObject>(PistoleImg, "pistle", 0, 300);
	Pistole->AddSprite({ 0,0,1,1 });
	Pistole->SetPos(vec3{ 70,546,0 });

	//�A�T���g�摜�̐ݒ�
	size_t AssaultImg = engine.AddUILayer("Res/Assault.tga", GL_LINEAR, 10);
	Assault = engine.CreateUI<GameObject>(AssaultImg, "assault", 0, 300);
	Assault->AddSprite({ 0,0,1,1 });
	Assault->SetPos(vec3{ 220,546,0 });
	Assault->SetAlpha(0);

	//�V���b�g�K���摜�̐ݒ�
	size_t ShotGunImg = engine.AddUILayer("Res/ShotGun.tga", GL_LINEAR, 10);
	ShotGun = engine.CreateUI<GameObject>(ShotGunImg, "shotgun", 0, 300);
	ShotGun->AddSprite({ 0,0,1,1 });
	ShotGun->SetPos(vec3{ 370,546,0 });
	ShotGun->SetAlpha(0);


	//�ԐF��HP�o�[�̐ݒ�
	size_t RedImg = engine.AddUILayer("Res/RedBar.tga", GL_LINEAR, 10);
	GameObjectPtr RedHPImg = engine.CreateUI<GameObject>(RedImg, "red", 0, 300);
	RedHPImg->AddSprite({ 0,0,1,1 });
	RedHPImg->SetPos(vec3{ 0,500,0 });

	//HP�o�[�̐ݒ�
	size_t HPImg = engine.AddUILayer("Res/HPBar.tga", GL_LINEAR, 10);
	HPBarImg = engine.CreateUI<GameObject>(HPImg, "green", 0, 300);
	HPBarImg->AddSprite({ 0,0,1,1 });
	HPBarImg->SetPos(vec3{ 0,500,0 });


	//HP�o�[�̈ړ�
	auto barMove = HPBarImg->AddComponent<HPBar>();
	barMove->SetPlayer(PlayerObj);
	barMove->SetMaxHP(PlayerObj->GetHP());

	//�ԐF�o�[�̈ړ�
	auto redMove = RedHPImg->AddComponent<RedHPBar>();
	redMove->SetGreenBar(HPBarImg);


	//HP�o�[�t���[���̐ݒ�
	size_t HPFrameImg = engine.AddUILayer("Res/HPFrame.tga", GL_LINEAR, 10);
	GameObjectPtr HPFrame = engine.CreateUI<GameObject>(HPFrameImg, "hpframe", 0, 500);
	HPFrame->AddSprite({ 0,0,1,1 });
	HPFrame->SetPos(vec3{ 230,650,0 });


	//�t�H���g�摜
	const size_t textLayer = engine.AddUILayer("Res/font_04_2.tga", GL_NEAREST, 10);

	//��������UI
	auto uiTime = engine.CreateUI<GameObject>(textLayer, "time", 940, 600);
	TimeManager = uiTime->AddComponent<TimeLimit>();
	TimeManager->SetTextComponent(uiTime->AddComponent<Text>());	//�e�L�X�g�R���|�[�l���g������
	
	//�����������̏��쐬
	auto UnderGround = engine.Create<GameObject>("Ground");		
	UnderGround->name = "floor";
	UnderGround->SetPos({ 0,-40,0 });
	UnderGround->SetMoveFlg(false);

	//�R���C�_�[�����蓖�Ă�
	auto Gcollider = UnderGround->AddComponent<BoxCollider>();
	Gcollider->box.Scale = vec3({ 250,1,350 });

	auto fall = UnderGround->AddComponent<UnderBorder>();

	//BGM���Đ�
	Audio::Play(AudioPlayer::bgm, BGM::stage01, 1, true);

	//�ϐ��̏�����
	MouceX = 0;
	MouceY = 0;
	oldX = 0;
	oldY = 0;
	return true;
}

//�X�V����
void MainGameScene::Update(Engine& engine,float deltaTime)
{
	//�摜���̍X�V
	UIUpdate();

	//�c�莞�Ԃ������Ȃ�����
	if (LimitTime < 0)
	{
		if (!BossShow)
		{
			//�{�X�̏o��
			BossObj = engine.Create<Boss>("enemy");
			auto CapsuleRenderer2 = BossObj->AddComponent<MeshRenderer>();
			CapsuleRenderer2->mesh = engine.LoadOBJ("Capsule2");
			CapsuleRenderer2->scale = vec3(2);
			BossObj->SetHP(50);

			auto ex = BossObj->AddComponent<BossExplosion>();

			//�ʂɐF��ς��邽�߂ɁA�}�e���A�����R�s�[
			CapsuleRenderer2->materials.push_back(
				std::make_shared<Mesh::Material>(*CapsuleRenderer2->mesh->materials[0]));

			//�_���[�W��^����Ώ�
			auto damage = BossObj->AddComponent<DamageSource>();
			damage->targetName = "player";

			//�����蔻��
			auto capcollider2 = BossObj->AddComponent<CapsuleCollider>();
			capcollider2->capsule.Seg.Start = vec3(0, 2, 0);
			capcollider2->capsule.Seg.End = vec3(0, -2, 0);
			capcollider2->capsule.Radius = std::min({
				CapsuleRenderer2->scale.x,
				CapsuleRenderer2->scale.y,
				CapsuleRenderer2->scale.z });

			//�o���ʒu
			BossObj->SetPos({ 10,5,-30 });

			//�{�X�̏o����m�点��t���O
			BossShow = true;
		}
	}
	else	//�������Ԃ����炵�Ă���
	{
		//�������Ԃ̓K��
		LimitTime -= deltaTime * engine.SlowSpeed;
		TimeManager->SetLimitTime(LimitTime);
	}

	//�}�E�X���擾���ăJ��������]������
	CameraRot(window, cameraobj, deltaTime, MouceX, MouceY);

	//�v���C���[�̃A�b�v�f�[�g
	PlayerUpdate(engine, PlayerObj, deltaTime);

	//�J�������v���C���[�ɍ��킹��
	cameraobj->SetPos(PlayerObj->GetPos());

	//���������킹��
	PlayerObj->SetRotation(vec3(0,cameraobj->GetRotation().y,0));


	//�{�X���o��������A�G���𕦂����Ȃ�
	if (BossShow)
	{
		return;
	}

	//�G�l�~�[�𐶐�����N�[���^�C��
	EnemyCreate += deltaTime;

	//�c�莞�Ԃ����Ȃ�����ʂɐ��������
	if (EnemyCreate >= LimitTime/20)
	{
		EnemyCreate = 0;

		//�����̐���
		const int rand = std::uniform_int_distribution<>(0, 100)(engine.GetRandomGenerator());//0~100

		if (rand == 100)//�L���O�X���C��
		{
			auto king = engine.Create<JumpingEnemy>("enemy");
			king->SetTarget(PlayerObj);

			//��P�̐ݒ�
			const float hp = std::uniform_real_distribution<float>(50, 100)(engine.GetRandomGenerator());//50~100
			king->SetHP(hp);

			//���f���`��
			auto kingRenderer = king->AddComponent<MeshRenderer>();
			kingRenderer->mesh = engine.LoadOBJ("Res/Model/SlimeKing.obj");
			kingRenderer->scale = vec3(1);

			//�o���ʒu
			const float x = std::uniform_real_distribution<float>(-100, 100)(engine.GetRandomGenerator());//-100~100
			const float z = std::uniform_real_distribution<float>(-100, 100)(engine.GetRandomGenerator());//-100~100
			king->SetPos(vec3(x, 1, z));

			//�ʂɐF��ς��邽�߂ɁA�}�e���A�����R�s�[
			for (const auto& e : kingRenderer->mesh->materials) {
				kingRenderer->materials.push_back(
					std::make_shared<Mesh::Material>(*e));
			}

			//�{�̂̐F��ς���
			const float r = std::uniform_real_distribution<float>(0, 1)(engine.GetRandomGenerator());//0~1
			const float g = std::uniform_real_distribution<float>(0, 1)(engine.GetRandomGenerator());//0~1
			const float b = std::uniform_real_distribution<float>(0, 1)(engine.GetRandomGenerator());//0~1
			kingRenderer->materials[1]->baseColor = vec4(r, g, b, 1);


			//�R���C�_�[�����蓖�Ă�
			auto kingCollider = king->AddComponent<SphereCollider>();
			kingCollider->sphere.Center = kingRenderer->translate;
			kingCollider->sphere.Radius = std::max({
				kingRenderer->scale.x,
				kingRenderer->scale.y,
				kingRenderer->scale.z });

			auto hh = king->AddComponent<Health>();		//HP
			auto ee = king->AddComponent<Explosion>();	//����
			auto dp = king->AddComponent<DropPowerUp>();//�A�C�e��
			dp->target = PlayerObj;
		}
		else if(rand>=50)//�΍�����������G
		{
			auto ene = engine.Create<DeviationEnemy>("enemy");
			ene->SetTarget(PlayerObj);

			//��P�̐ݒ�
			const float hp = std::uniform_real_distribution<float>(2, 4)(engine.GetRandomGenerator());//2~4
			ene->SetHP(hp);

			//���f���ǂݍ���
			auto eneRenderer = ene->AddComponent<MeshRenderer>();
			eneRenderer->mesh = engine.LoadOBJ("Res/Model/Drone_01.obj");
			eneRenderer->scale = vec3(1);

			//�X�|�[���ʒu
			const float x = std::uniform_real_distribution<float>(-100, 100)(engine.GetRandomGenerator());//-100~100
			const float z = std::uniform_real_distribution<float>(-100, 100)(engine.GetRandomGenerator());//-100~100
			ene->SetPos(vec3(x, 1, z));

			//�ʂɐF��ς��邽�߂ɁA�}�e���A�����R�s�[
			for (const auto& e : eneRenderer->mesh->materials) {
				eneRenderer->materials.push_back(
					std::make_shared<Mesh::Material>(*e));
			}

			//�{�̂̐F��ς���
			const float r = std::uniform_real_distribution<float>(0, 1)(engine.GetRandomGenerator());//0~1
			const float g = std::uniform_real_distribution<float>(0, 1)(engine.GetRandomGenerator());//0~1
			const float b = std::uniform_real_distribution<float>(0, 1)(engine.GetRandomGenerator());//0~1
			eneRenderer->materials[0]->baseColor = vec4(r, g, b, 1);


			//�R���C�_�[�����蓖�Ă�
			auto enecollider = ene->AddComponent<SphereCollider>();
			enecollider->sphere.Center = eneRenderer->translate;
			enecollider->sphere.Radius = std::max({
				eneRenderer->scale.x,
				eneRenderer->scale.y,
				eneRenderer->scale.z });

			
			auto he = ene->AddComponent<HitEffect>();	//�G�t�F�N�g
			auto hh = ene->AddComponent<Health>();		//HP
			auto ee = ene->AddComponent<Explosion>();	//����
			auto dp = ene->AddComponent<DropPowerUp>();	//�A�C�e��
			dp->target = PlayerObj;
		}
		else//�X���C��
		{
			auto ene = engine.Create<JumpingEnemy>("enemy");
			ene->SetTarget(PlayerObj);
			
			//��P�̐ݒ�
			const float hp = std::uniform_real_distribution<float>(2, 3)(engine.GetRandomGenerator());//2~3
			ene->SetHP(hp);
			
			//���f���̓ǂݍ���
			auto eneRenderer = ene->AddComponent<MeshRenderer>();
			eneRenderer->mesh = engine.LoadOBJ("Res/Model/Slime_02_blender.obj");//�E�T�M���ۂ��X���C��
			eneRenderer->scale = vec3(1);

			//�X�|�[���ʒu
			const float x = std::uniform_real_distribution<float>(-100, 100)(engine.GetRandomGenerator());//-100~100
			const float z = std::uniform_real_distribution<float>(-100, 100)(engine.GetRandomGenerator());//-100~100
			ene->SetPos(vec3(x, 1, z));

			//�ʂɐF��ς��邽�߂ɁA�}�e���A�����R�s�[
			for (const auto& e : eneRenderer->mesh->materials) {
				eneRenderer->materials.push_back(
					std::make_shared<Mesh::Material>(*e));
			}

			//�{�̂̐F��ς���
			const float r = std::uniform_real_distribution<float>(0, 1)(engine.GetRandomGenerator());//0~1
			const float g = std::uniform_real_distribution<float>(0, 1)(engine.GetRandomGenerator());//0~1
			const float b = std::uniform_real_distribution<float>(0, 1)(engine.GetRandomGenerator());//0~1
			eneRenderer->materials[1]->baseColor = vec4(r, g, b, 1);


			//�R���C�_�[�����蓖�Ă�
			auto enecollider = ene->AddComponent<SphereCollider>();
			enecollider->sphere.Center = eneRenderer->translate;
			enecollider->sphere.Radius = std::max({
				eneRenderer->scale.x,
				eneRenderer->scale.y,
				eneRenderer->scale.z });

			auto hh = ene->AddComponent<Health>();			//HP
			auto co = ene->AddComponent<HitEffect>();		//�G�t�F�N�g
			auto ee = ene->AddComponent<MegaExplosion>();	//����
			auto pp = ene->AddComponent<DropPowerUp>();		//�A�C�e��
			pp->target = PlayerObj;

		}
	}


}

void MainGameScene::CameraRot(GLFWwindow* window, GameObjectPtr camera, float deltaTime, double MouceX, double MouceY)
{
	glfwGetCursorPos(window, &MouceX, &MouceY);//XY�Ƀ}�E�X�ʒu���i�[

	//���x
	const float scensi = 8.0f;


	//���̉�]
	camera->AddRotaion(vec3(0,
		static_cast<float>((oldX - MouceX)) * scensi * 0.01f * deltaTime,
		0));

	//�c�̉�]
	camera->AddRotaion(vec3(static_cast<float>((oldY - MouceY)) * scensi * 0.01f * deltaTime,
		0, 0));

	//�㉺�̎��_�ړ��ɐ���������
	camera->SetRotation(vec3(std::clamp(camera->GetRotation().x, -1.5f, 1.5f),
		camera->GetRotation().y,
		camera->GetRotation().z));

	//�Â��ʒu���X�V
	oldX = MouceX;
	oldY = MouceY;
}

void MainGameScene::PlayerUpdate(Engine& engine, PlayerPtr player, float deltaTime)
{
	
	//�������Ă���������擾
	float radY = cameraobj->GetRotation().y;
	float radX = cameraobj->GetRotation().x;


	//���ʃx�N�g��
	vec3 forward = vec3(1);

	//x����]������
	forward *= vec3(1,
					sin(radX),
					cos(radX));

	//y���ŉ�]��������
	forward *= vec3(sin(radY),
					1,
					cos(radY));

	//���]������
	forward *= vec3{ -1,1,-1 };

	//���K������
	forward = normalize(forward);

	//�O�ςŁA�E�����x�N�g�����擾
	vec3 right = cross(forward, vec3{ 0,1,0 });

	//���K������
	right = normalize(right);

	//�ݒ肷��
	player->SetForward(forward);
	player->SetRight(right);


}

void MainGameScene::PlayerInit(Engine& engine)
{
	//�����ڂ�`��
	PlayerObj = engine.Create<Player>("player");
	auto CapsuleRenderer = PlayerObj->AddComponent<MeshRenderer>();
	CapsuleRenderer->mesh = engine.LoadOBJ("Capsule2");
	CapsuleRenderer->scale = vec3(1);
	PlayerObj->name = "player";

	//HP
	PlayerObj->SetHP(5);

	//�J�v�Z���̓����蔻������蓖��
	auto capcollider = PlayerObj->AddComponent<CapsuleCollider>();
	capcollider->capsule.Seg.Start = vec3(0, 1, 0);
	capcollider->capsule.Seg.End = vec3(0, -1, 0);
	capcollider->capsule.Radius = std::min({
		CapsuleRenderer->scale.x,
		CapsuleRenderer->scale.y,
		CapsuleRenderer->scale.z });

	//�ʒu�𒲐�
	PlayerObj->SetPos({ 0,10,0 });
	PlayerObj->SetRotation({ 90, 0, -90 });
}

void MainGameScene::UIUpdate()
{
	//��ԍ��̒�ʒu
	vec3 Pos = vec3{ 70,546,0 };

	switch (PlayerObj->GetShotStyle())
	{
	case 0:
		//�Z���N�g�摜���ړ�������
		Select->SetPos(Pos);

		//�I����œ����x��ς���
		Pistole->SetAlpha(1.0f);
		Assault->SetAlpha(0.5f);
		ShotGun->SetAlpha(0.5f);
		break;

	case 1:
		//�Z���N�g�摜���ړ�������
		Select->SetPos(vec3{ Pos.x + UIMARGINE,Pos.y,Pos.z });

		//�I����œ����x��ς���
		Pistole->SetAlpha(0.5f);
		Assault->SetAlpha(1.0f);
		ShotGun->SetAlpha(0.5f);

		break;

	case 2:
		//�Z���N�g�摜���ړ�������
		Select->SetPos(vec3{ Pos.x + UIMARGINE * 2,Pos.y,Pos.z });

		//�I����œ����x��ς���
		Pistole->SetAlpha(0.5f);
		Assault->SetAlpha(0.5f);
		ShotGun->SetAlpha(1.0f);

		break;
	default:
		break;
	}

	//�v���C���[�̕���̎擾�󋵂ɍ��킹�ē����x��ς���
	if (!PlayerObj->GetShooterFlg())
	{
		Assault->SetAlpha(0.0f);
	}
	if (!PlayerObj->GetShotGunFlg())
	{
		ShotGun->SetAlpha(0.0f);
	}


}

