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


//����������
bool TutorialScene::Initialize(Engine& engine)
{
	//�Q�[���I�u�W�F�N�g���폜
	engine.ClearGameObjectList();
	engine.ClearUILayers();
	engine.RemoveParticleEmitterAll();

	//�E�B���h�E�̎擾
	window = engine.GetWindow();

	//�J�����̎擾
	cameraobj = engine.GetMainCameraObject();
	cameraobj->SetRotation({ 0,0,-1 });

	//�}�E�X���E�B���h�E�ɌŒ肵�\�������Ȃ�
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//�}�b�v�̓ǂݍ���
	auto gameObjList = engine.LoadGameObjectMap("Res/TutorialMap.json", vec3(0, 0, 0), vec3(1));

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
	PlayerObj->SetRotation({ 0, 0, 0 });

	cameraobj->SetRotation({ 0,radians(180),0 });
	//�󔠂̐���
	auto treasure = engine.SearchGameObject("Golden_Chest");
	auto  chest = treasure->AddComponent<TreasureBox>();
	chest->SetTarget(PlayerObj);
	chest->SetTutorialFlg(true);

	auto Treasure = engine.SearchGameObject("Golden_Chest (1)");
	auto  Chest = Treasure->AddComponent<TreasureBox>();
	Chest->SetTarget(PlayerObj);
	Chest->SetTutorialFlg(true);

	//�S�[���̐���
	auto GoalObj = engine.SearchGameObject("Goal");
	auto goal = GoalObj->AddComponent<Goal>();
	goal->SetEngine(&engine);
	goal->SetTarget(PlayerObj);

	//��ʂ̒��S�ɃG�C����\��
	size_t mouce = engine.AddUILayer("Res/Bullet_1.tga", GL_LINEAR, 10);
	GameObjectPtr uimouce = engine.CreateUI<GameObject>(mouce, "aim", 367, 50);
	uimouce->AddSprite({ 0,0,1,1 });
	uimouce->SetPos(vec3(640, 360, 0));


		//�s�X�g���摜�̐ݒ�
	size_t SelectImg = engine.AddUILayer("Res/Select.tga", GL_LINEAR, 10);
	Select = engine.CreateUI<GameObject>(SelectImg, "hp", 0, 300);
	Select->AddSprite({ 0,0,1,1 });
	Select->SetPos(vec3{ 70,546,0 });
	
	//����t���[���摜�̔z�u
	size_t WeaponFrameImg = engine.AddUILayer("Res/WeaponFrame.tga", GL_LINEAR, 10);
	for (int i = 0; i < 3; i++)
	{
		GameObjectPtr WeaponFrame = engine.CreateUI<GameObject>(WeaponFrameImg, "hp", 0, 300);
		WeaponFrame->AddSprite({ 0,0,1,1 });
		WeaponFrame->SetPos(vec3{ 70.0f + UIMARGINE * i,546,0 });
	}

	//�s�X�g���摜�̐ݒ�
	size_t PistoleImg = engine.AddUILayer("Res/Pistole.tga", GL_LINEAR, 10);
	Pistole = engine.CreateUI<GameObject>(PistoleImg, "hp", 0, 300);
	Pistole->AddSprite({ 0,0,1,1 });
	Pistole->SetPos(vec3{ 70,546,0 });

	//�A�T���g�摜�̐ݒ�
	size_t AssaultImg = engine.AddUILayer("Res/Assault.tga", GL_LINEAR, 10);
	Assault = engine.CreateUI<GameObject>(AssaultImg, "hp", 0, 300);
	Assault->AddSprite({ 0,0,1,1 });
	Assault->SetPos(vec3{ 220,546,0 });
	Assault->SetAlpha(0);

	//�V���b�g�K���摜�̐ݒ�
	size_t ShotGunImg = engine.AddUILayer("Res/ShotGun.tga", GL_LINEAR, 10);
	ShotGun = engine.CreateUI<GameObject>(ShotGunImg, "hp", 0, 300);
	ShotGun->AddSprite({ 0,0,1,1 });
	ShotGun->SetPos(vec3{ 370,546,0 });
	ShotGun->SetAlpha(0);

	//�t�F�[�h�C��������
	engine.SetFadeRule("Res/fade_rule.tga");
	engine.StartFadeIn();


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

	//�����p�[�e�B�N����z�us
	for (int i = 0; i < 3; i++)
	{
		auto ParticleObje = engine.Create<ItemObject>(
			"particle explosion", vec3{ -10.0f + i * 10.0f,1.0f,356.0f });
		ParticleObje->player = PlayerObj;	//�Ώۂ̐�a��
		ParticleObje->SetSpot(i);		//�������镔�ʂ̐ݒ�

		auto patcollider = ParticleObje->AddComponent<CapsuleCollider>();
		patcollider->capsule.Seg.Start = vec3(0, 1, 0);
		patcollider->capsule.Seg.End = vec3(0, -1, 0);
		patcollider->capsule.Radius = 1.0f;

		auto ps = ParticleObje->AddComponent<ParticleSystem>();
		ps->Emitters.resize(1);
		auto& emitter = ps->Emitters[0];
		emitter.ep.ImagePath = "Res/face.tga";
		emitter.ep.Duration = 0.5f;				//���o����
		emitter.ep.EmissionsPerSecond = 10;	//�b�ԕ��o��
		emitter.ep.RandomizeRotation = 1;		//�p�x������
		emitter.ep.RandomizeDirection = 0;
		emitter.ep.RandomizeSize = 1;			//�傫���������_����
		emitter.pp.LifeTime = 0.4f;				//��������
		emitter.pp.rotation.Set(90.0f, 0);

		if (i == 0)
		{
			emitter.pp.color.Set({ 255, 1, 1, 1 }, { 1, 1, 1.5f, 0 });	//�F�t��
		}
		else if (i == 1)
		{
			emitter.pp.color.Set({ 2, 255, 1, 1 }, { 1, 1, 1.5f, 0 });	//�F�t��
		}
		else if (i == 2)
		{
			emitter.pp.color.Set({ 2, 1, 255.5f, 1 }, { 1, 1, 1.5f, 0 });	//�F�t��
		}
		emitter.pp.scale.Set({ 0.01f,0.01f }, { 0.05f,0.05f });	//�T�C�Y�����X�ɂ֕ύX������
		emitter.pp.radial.Set(0, 10);			//�p�x�����ɑ��x������

		emitter.ep.Loop = true;	//���[�v�Đ�������

	}

	//�ϐ��̏�����
	MouceX = 0;
	MouceY = 0;
	oldX = 0;
	oldY = 0;
	return true;
}

//�X�V����
void TutorialScene::Update(Engine& engine, float deltaTime)
{
	UIUpdate();

	//�}�E�X���擾���ăJ��������]������
	CameraRot(window, cameraobj, deltaTime, MouceX, MouceY);

	PlayerUpdate(engine, PlayerObj, deltaTime);

	cameraobj->SetPos(PlayerObj->GetPos());

	//���������킹��
	PlayerObj->SetRotation(vec3(0, cameraobj->GetRotation().y, 0));
}

void TutorialScene::CameraRot(GLFWwindow* window, GameObjectPtr camera, float deltaTime, double MouceX, double MouceY)
{
	glfwGetCursorPos(window, &MouceX, &MouceY);//XY�Ƀ}�E�X�ʒu���i�[

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


	oldX = MouceX;
	oldY = MouceY;
}

void TutorialScene::PlayerUpdate(Engine& engine, PlayerPtr player, float deltaTime)
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

void TutorialScene::UIUpdate()
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

