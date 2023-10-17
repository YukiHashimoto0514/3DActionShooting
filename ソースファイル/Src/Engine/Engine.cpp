//@Engine.cpp
#define _CRT_SECURE_NO_WARNINGS
#include "Engine.h"
#include "Mesh.h"
#include "Debug.h"
#include "Texture.h"
#include "VertexArray.h"
#include "ProgramPipeline.h"
#include "../Application/EasyAudio.h"
#include "../Application/TitleScene.h"
#include "../Application/MainGameScene.h"
#include "../Component/Goal.h"
#include "../Component/Warp.h"
#include "../Component/Health.h"
#include "../Component/Camera.h"
#include "../Component/Collider.h"
#include "../Component/Explosion.h"
#include "../Component/FallFloor.h"
#include "../Component/DropPowerUp.h"
#include "../Component/TreasureBox.h"
#include "../Component/MeshRenderer.h"
#include "../Component/ParticleSystem.h"

#undef APIENTRY
#include <Windows.h>

//�E�B���h�E�Y�̒�`�𖢒�`��
#undef far
#undef near

//minwinder��min�֐����g�������Ȃ�
#undef min
#undef max
#include <fstream>
#include <filesystem>

//#include "../ImGui/imgui.h"
//#include "../ImGui/imgui_impl_glfw.h"
//#include "../ImGui/imgui_impl_opengl3.h"


using namespace VecMath;

//���[���h���W�n�̏Փ˔����\���\����
struct WorldCollider
{
	//�Q�[���I�u�W�F�N�g�ƏՓ˔���̍��W��ύX����
	void AddPosition(const vec3& v)
	{
		//�Q�[���I�u�W�F�N�g�̈ʒu��ύX
		gameObject->AddPosition(v);

		for (auto& e : colliderList)
		{
			//�R���C�_�[�֐���AddPositin���Ă�
			e->AddPosition(v);
		}
	}

	//���̃��[���h�R���C�_�[�Ƃ̏Փ˔�����s��
	std::vector<Collision::ContactPoint> DetectCollision(WorldCollider& other)
	{
		std::vector<Collision::ContactPoint> cpList;

		for (auto& ca : colliderList)
		{
			for (auto& cb : other.colliderList)
			{
				Collision::ContactPoint cp;

				//�����Ȃ����̓��m�͔��肵�Ȃ�
				if (!this->gameObject->GetMovable() && !other.gameObject->GetMovable())
				{
					continue;
				}

				//�Փ˂��Ă��Ȃ�
				if (!Intersect(*ca, *cb, cp))
				{
					//���̏�����
					continue;
				}


				//�Փ˂���
				cpList.push_back(cp);

				//���W��߂�
				const vec3 v = cp.Normal * cp.Penetration * 0.5f;

				//�����Ȃ����̂Ɠ������̂̔���
				if (other.gameObject->GetMovable() && !this->gameObject->GetMovable())
				{
					//�����Ȃ����̂ƂȂ̂łQ�{���˕Ԃ�
					other.AddPosition(v * 2);
				}
				//�����Ȃ����̂Ɠ������̂̔���
				else if (!other.gameObject->GetMovable() && this->gameObject->GetMovable())
				{
					//�����Ȃ����̂ƂȂ̂łQ�{���˕Ԃ�
					AddPosition(-v * 2);
				}
				//�������̂Ɠ������̂̔���
				else if (other.gameObject->GetMovable() && this->gameObject->GetMovable())
				{
					other.AddPosition(v);
					AddPosition(-v);
				}
				
				//�@���ŏՓ˂����p�x���v�Z
				float theta = dot(cp.Normal, vec3(0, 1, 0));
				
				//��̐�����������
				if (theta <= -0.9f)
				{
					
					gameObject->only = 4;		//�n�ʔ���ɂ���t���[����
					other.gameObject->only = 4;	//�n�ʔ���ɂ���t���[����
					other.gameObject->SetJumpFlg(false);//�W�����v�ł���悤�ɂ���
					gameObject->SetJumpFlg(false);		//�W�����v�ł���悤�ɂ���
				}

			}
		}


		return cpList;
	}

	GameObject* gameObject;	//�R���C�_�[�����L����Q�[���I�u�W�F�N�g
	std::vector<ColliderPtr> colliderList;	//�R���C�_�[�z��
};

//OpenGL��̃��b�Z�[�W����������R�[���o�b�N�֐�
// 
// @param source			���b�Z�[�W�̔z�M��(OpenGL,Windows,�V�F�[�_�[�Ȃ�)
// @param type				���b�Z�[�W�̎��(�G���[�x���Ȃ�)
// @param id				���b�Z�[�W����ʂɎ��ʂ���l
// @param severity			���b�Z�[�W�̏d�v�x(���A���A��A�Œ�)
// @param length			���b�Z�[�W�̕�����. �����Ȃ烁�b�Z�[�W�͂O�I�[����Ă���
// @param message			���b�Z�[�W�{��
// @param userParam			�R�[���o�b�N�ݒ莞�Ɏw�肵���|�C���^

void GLAPIENTRY DebugCallback(GLenum, GLenum type, GLenum id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	std::string s;
	if (length < 0)
	{
		s = message;
	}
	else
	{
		s.assign(message, message + length);
	}
	s += '\n';
	OutputDebugString(s.c_str());//(�o�͂��镶����)
}

//SSBO�̃T�C�Y���v�Z����
size_t CalcSsboSize(size_t n)//���ȏ�̂Q�T�U�̔{���̒l
{
	const size_t alignment = 256;//�A���C�����g�i�f�[�^�̋��E�j
	return ((n + alignment - 1) / alignment) * alignment;
};



//�G���W�������s����
// 	   @retval 0 ����Ɏ��s����������(retval=�߂�l)
// 	   @retval 0�ȊO�@�G���[����������

int Engine::Run()
{
	int ret = Initialize();//������
	if (ret == 0)
	{
		ret = MainLoop();//���C�����[�v
	}
	Finalize();//�I��
	return ret;
}

//�z�u�f�[�^��ǂݍ���

TileMap Engine::LoadTileMap(const char* filename)
{
	//�t�@�C�����J��
	std::ifstream file;
	file.open(filename);
	if (!file)
	{
		return { };	//���s�i�t�@�C�������Ⴄ�H�j
	}

	//�ǂݍ��ݐ�ƂȂ�ϐ�������
	TileMap tileMap;
	tileMap.data.reserve(bgSizeX * bgSizeY);	//�K���ȗe�ʂ�\��

	//�t�@�C����ǂݍ���
	while (!file.eof())
	{
		//��s�ǂݍ���
		std::string line;
		std::getline(file, line);	//�t�@�C������ǂݍ���

		if (line.size() <= 0)
		{
			continue;	//�f�[�^�̖����s�͖���
		}
		line.push_back(',');	//�I�[��ǉ�

		//�J���}��؂蕶����𐔒l�ɕϊ�
		tileMap.sizeX = 0;
		const char* start = line.data();
		const char* end = start + line.size();

		for (const char* p = start; p != end; ++p)
		{
			if (*p == ',')
			{
				const int n = atoi(start);	//������𐔎���
				tileMap.data.push_back(n);
				start = p + 1;				//���̃J���}��؂��ڎw���悤�ɍX�V
				++tileMap.sizeX;			//���̃^�C�������X�V
			}
		}
		++tileMap.sizeY;//�c�̃^�C�����X�V
	}

	//�ǂݍ��񂾔z�u�f�[�^��Ԃ�
	return tileMap;
}


//3D���f���z�u�t�@�C����ǂݍ���

//@param filename   3D���f���z�u�t�@�C��
//@param translate	���ׂĂ�3D���f�����W�ɉ��Z���镽�s�ړ���
//@param scale		���ׂĂ�3D���f���Ɋ|����g�嗦
//@return �t�@�C������ǂݍ��񂾃Q�[���I�u�W�F�N�g�̔z��

GameObjectList Engine::LoadGameObjectMap(const char* filename,
	const vec3& translate, const vec3& scale)
{
	//�t�@�C�����J��
	std::ifstream ifs(filename);

	if (!ifs)
	{
		LOG_WARNING("%s���J���܂���", filename);
		return{};
	}

	GameObjectList gameObjectList;
	gameObjectList.reserve(400);//�K���Ȑ���\��

	//�s�P�ʂœǂݍ���
	while (!ifs.eof())
	{
		std::string line;
		std::getline(ifs, line);

		const char* p = line.c_str();

		int readByte = 0;

		//�f�[�^�s�̊J�n����
		char name[256];

					  //���������e����
		if (sscanf(p, R"( { "name" : "%255[^"]" %n)", name, &readByte) != 1)
		{
			continue;	//�f�[�^�s�ł͂Ȃ�
		}
		name[255] = '\0';
		p += readByte;	//�ǂݎ��ʒu���X�V

		//���O�ȊO�̗v�f��ǂݍ���
		char baseMeshName[256];

		if (sscanf(p, R"(, "mesh" : "%255[^"]" %n)", baseMeshName, &readByte) != 1)
		{
			continue;
		}
		baseMeshName[255] = '\0';
		p += readByte;	//�ǂݎ��ʒu���X�V

		vec3 t(0);
		if (sscanf(p, R"(, "translate" : [ %f, %f, %f ] %n)",
			&t.x, &t.y, &t.z, &readByte) != 3)
		{
			continue;
		}
		p += readByte;	//�ǂݎ��ʒu���X�V

		vec3 r(0);
		if (sscanf(p, R"(, "rotate" : [ %f, %f, %f ] %n)",
			&r.x, &r.y, &r.z, &readByte) != 3)
		{
			continue;
		}

		p += readByte;	//�ǂݎ��ʒu���X�V

		vec3 s(1);
		if (sscanf(p, R"(, "scale" : [ %f, %f, %f ] %n)",
			&s.x, &s.y, &s.z, &readByte) != 3)
		{
			continue;
		}

		p += readByte;	//�ǂݎ��ʒu���X�V
		
		char tag[256];
		if (sscanf(p, R"(, "tag" : "%255[^"]" %n } )", tag, &readByte) != 1)
		{
			continue;
		}
		tag[255] = '\0';
		p += readByte;	//�ǂݎ��ʒu���X�V


		//���b�V����ǂݍ���
		const std::string meshName = std::string("Res/Model/") + baseMeshName + ".obj";
		auto mesh = LoadOBJ(meshName.c_str());

		if (!mesh)
		{
			LOG_WARNING("���b�V���t�@�C��%s�̓ǂݍ��݂Ɏ��s", meshName.c_str());
			continue;
		}

		//���W�̊g��k���ƕ��s�ړ�
		t = t * scale + translate;
		s *= scale;

		//�Q�[���I�u�W�F�N�g���쐬
		auto gameObject = Create<GameObject>(meshName);
		gameObject->SetPos(t);

		//Unity�͍�����W�n�Ȃ̂ŁA��]�������E����W�n�ɕϊ�����
		gameObject->SetRotation(vec3(radians(-r.x), radians(-r.y), radians(r.z)));


		auto renderer = gameObject->AddComponent<MeshRenderer>();
		renderer->mesh = mesh;
		renderer->scale = s * 0.5f;	//Unity�ŏo�͂��Ă��邽�߃T�C�Y���킹�邽�ߔ�����

		//Unity����G�N�X�|�[�Ƃ���OBJ��X�������]���Ă���̂ŁAY����180�x��]������
		renderer->rotation.y = radians(180);

		std::string Tag = tag;
		//�n�ʂ������ꍇ
		if (Tag == "Floor")
		{
			//�R���C�_�[�����蓖�Ă�
			auto Gcollider = gameObject->AddComponent<BoxCollider>();
			Gcollider->box.Scale = renderer->scale;

			gameObject->SetMoveFlg(false);

		}
		else if (Tag == "Rock")//���������̏ꍇ
		{
			//�R���C�_�[�����蓖�Ă�
			auto Gcollider = gameObject->AddComponent<BoxCollider>();
			Gcollider->box.Scale = renderer->scale;

			gameObject->SetMoveFlg(true);
		}
		else if (Tag == "FallFloor")//�������̏ꍇ
		{
			//�R���C�_�[�����蓖�Ă�
			auto Gcollider = gameObject->AddComponent<BoxCollider>();
			Gcollider->box.Scale = renderer->scale;

			gameObject->AddComponent<FallFloor>();
			gameObject->SetMoveFlg(false);
		}
		else if (Tag == "Tree")
		{
			auto capcollider = gameObject->AddComponent<CapsuleCollider>();

			//���f�����ƂɊ��̑������Ⴄ���߁A���̕��@�őΏ�
			if (meshName == "Res/Model/Tree_01.obj")
			{
				capcollider->capsule.Radius = std::min({
					renderer->scale.x,
					renderer->scale.y,
					renderer->scale.z }) * 3;
			}
			else
			{
				capcollider->capsule.Radius = std::min({
					renderer->scale.x,
					renderer->scale.y,
					renderer->scale.z });

			}
			capcollider->capsule.Seg.Start = vec3(0, 1 * capcollider->capsule.Radius, 0);
			capcollider->capsule.Seg.End = vec3(0, -1 * capcollider->capsule.Radius, 0);

			gameObject->SetMoveFlg(false);

		}
		else if (Tag == "Treasure")
		{
			std::string Name = name;
			gameObject->name = Name;

			//�R���C�_�[�����蓖�Ă�
			auto Gcollider = gameObject->AddComponent<BoxCollider>();
			Gcollider->box.Scale = renderer->scale;

			gameObject->SetMoveFlg(false);

		}
		else if (Tag == "Goal")
		{
			std::string Name = name;
			gameObject->name = Name;

			//�R���C�_�[�����蓖�Ă�
			auto Gcollider = gameObject->AddComponent<BoxCollider>();
			Gcollider->box.Scale = renderer->scale;

			auto ParticleObject = Create<GameObject>(
				"particle explosion", gameObject->GetPos());
			

			auto ps = ParticleObject->AddComponent<ParticleSystem>();
			ps->Emitters.resize(1);
			auto& emitter = ps->Emitters[0];
			emitter.ep.ImagePath = "Res/Last.tga";

			emitter.ep.RandomizeRotation = 1;		//�p�x������
			emitter.ep.RandomizeDirection = 1;
			emitter.ep.RandomizeSize = 1;			//�傫���������_����
			emitter.ep.Duration = 0.1f;				//���o����
			emitter.ep.EmissionsPerSecond = 100;	//�b�ԕ��o��

			emitter.pp.LifeTime = 0.4f;				//��������
			emitter.pp.color.Set({ 5, 1, 5.5f, 1 }, { 1, 2, 1.5f, 0 });	//�F�t��
			emitter.pp.scale.Set({ 0.3f,0.1f }, { 0.05f,0.02f });	//�T�C�Y�����X�ɂ֕ύX������

			emitter.ep.Loop = true;
			gameObject->SetMoveFlg(false);

		}
		else if (Tag == "WarpEnter")
		{
			std::string Name = name;
			gameObject->name = name;

			
			//�R���C�_�[�����蓖�Ă�
			auto Gcollider = gameObject->AddComponent<BoxCollider>();
			Gcollider->box.Scale = renderer->scale;
			gameObject->SetMoveFlg(false);

		}
		else if (Tag == "WarpExit")
		{
			std::string Name = name;
			gameObject->name = Name;

		}
		else if (Tag == "ScareCrow")
		{
			gameObject->name = "enemy";

			//�R���C�_�[��ݒ�
			auto collider = gameObject->AddComponent<SphereCollider>();
			collider->sphere.Center = renderer->translate;
			collider->sphere.Radius = 1.5f;

			auto hh = gameObject->AddComponent<Health>();		//HP
			auto ee = gameObject->AddComponent<Explosion>();	//����

		}
		//�Q�[���I�u�W�F�N�g��ǉ�
		gameObjectList.push_back(gameObject);
	}


	//�쐬�����Q�[���I�u�W�F�N�g�z���Ԃ�
	LOG("3D���f���z�u�t�@�C��%s��ǂݍ��݂܂���", filename);

	return gameObjectList;
}

//�t�F�[�h�C�����J�n����
//@param time �t�F�[�h�ɂ����鎞��

void Engine::StartFadeIn(float time)
{
	//��Ԃ��t�F�[�h���s���ɂ���
	fadeState = FadeState::Fading;

	//�G�t�F�N�g�ɂ����鎞�Ԃ�ݒ肵�A�o�ߎ��Ԃ����Z�b�g����
	FadeTotalTime = time;
	FadeTimer = 0;

	//臒l���O�`�P�ɕω�����悤�ɐݒ�
	FadeFrom = 0;
	FadeTo = 1;
}

//�t�F�[�h�A�E�g���J�n����
//@param time �t�F�[�h�ɂ����鎞��

void Engine::StartFadeOut(float time)
{
	//��Ԃ��t�F�[�h���s���ɂ���
	fadeState = FadeState::Fading;

	//�G�t�F�N�g�ɂ����鎞�Ԃ�ݒ肵�A�o�ߎ��Ԃ����Z�b�g����
	FadeTotalTime = time;
	FadeTimer = 0;

	//臒l���P�`�O�ɕω�����悤�ɐݒ�
	FadeFrom = 1;
	FadeTo = 0;
}

//UI���C���[��ǉ�����

//@param image      ���C���[�Ŏg�p����e�N�X�`���摜
//@param filterMode �e�N�X�`���ɐݒ肷��g��k���t�B���^
//@param reserve    ���C���[�p�ɗ\�񂷂�Q�[���I�u�W�F�N�g��
//@return �ǉ��������C���[�ԍ�

size_t Engine::AddUILayer(const char* image, GLenum filterMode, size_t reserve)
{
	TexturePtr tex = Texture::Create(image, filterMode,
		Texture::Usage::for2D);
	uiLayerList.push_back({ tex });
	uiLayerList.back().gameObjectList.reserve(reserve);
	uiLayerList.back().spriteList.reserve(reserve);

	return uiLayerList.size() - 1;
}

//���ׂẴ��C���[���폜����
void Engine::ClearUILayers()
{
	uiLayerList.clear();
}

//OBJ�t�@�C����ǂݍ���
//@param filename �ǂݍ���OBJ�t�@�C����
//@return�쐬�������b�V���̃A�h���X

Mesh::StaticMeshPtr Engine::LoadOBJ(const char* filename)
{
	return meshBuffer->LoadOBJ(filename);
}

//�G�~�b�^�[��ǉ�����
//@param ep	�G�~�b�^�[�̏������p�����[�^
//@param pp �p�[�e�B�N���̏������p�����[�^

ParticleEmitterPtr Engine::AddParticleEmitter(
	const ParticleEmitterParameter& ep, const ParticleParameter& pp)
{
	return particleManager->AddEmitter(ep, pp);
}

//�w�肳�ꂽ���O�����G�~�b�^�[����������
//@param  name ��������id
//@return ����id�ƈ�v����id�����G�~�b�^�[

ParticleEmitterPtr Engine::FindParticleEmitter(const std::string& name)const
{
	return particleManager->FindEmitter(name);
}

//�w�肳�ꂽ�G�~�b�^�[���폜����
void Engine::RemoveParticleEmitter(const ParticleEmitterPtr& p)
{
	particleManager->RemoveEmitter(p);
}

//���ׂẴG�~�b�^�[���폜����
void Engine::RemoveParticleEmitterAll()
{
	particleManager->RemoveEmitterAll();
}

//�Q�[���G���W��������������
//		@retval 0 ����ɏ��������ꂽ
//		@retval 0�ȊO�@�G���[����������

int Engine::Initialize()
{
	//GLFW�̏�����
	if (glfwInit() != GLFW_TRUE)
	{
		return 1;
	}


	//�`��E�B���h�E�̍쐬
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);//	�q���g�̎�ށA�q���g�ɐݒ肷��l

	//�t���[���o�b�t�@��sRGB�Ή��ɂ���
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
	window = glfwCreateWindow(1280, 720, title.c_str(), nullptr, nullptr);

	if (!window)
	{
		glfwTerminate();
		return 1;
	}

	//OpenGL�R���e�L�X�g�̍쐬
	glfwMakeContextCurrent(window);

	//OpenGL�֐��̃A�h���X���擾����
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		glfwTerminate();
		return 1;
	}

	//���b�Z�[�W�R�[���o�b�N�̐ݒ�
	glDebugMessageCallback(DebugCallback, nullptr);//(�Ăяo���֐��̃A�h���X�AuserParam�����ɓn�����l)

	//�V�F�[�_�[�t�@�C��(2D)�̓ǂݍ���
	progSprite = ProgramPipeline::Create(
		"Res/standard_2D.vert", "Res/standard_2D.frag");

	//�V�F�[�_�[�t�@�C��(3D)�̓ǂݍ���
	progStandard3D = ProgramPipeline::Create(
		"Res/standard_3D.vert", "Res/standard_3D.frag");

	//�e�p�I�u�W�F�N�g���쐬
	fboShadow = FramebufferObject::Create(1024, 1024, FboType::depth);
	progShadow = ProgramPipeline::Create("Res/shadow.vert", "Res/shadow.frag");

	//�X�J�C�{�b�N�X�p�I�u�W�F�N�g�𐶐�
	const std::string skyBoxPathList[6] = {
		"Res/SkyBox/px2.tga","Res/SkyBox/nx2.tga",
		"Res/SkyBox/py2.tga","Res/SkyBox/ny2.tga",
		"Res/SkyBox/pz2.tga","Res/SkyBox/nz2.tga",
	};

	texSkyBox = Texture::CreateCubeMap(skyBoxPathList);
	progSkyBox = ProgramPipeline::Create("Res/skybox.vert", "Res/skybox.frag");

	//�Q�[���E�B���h�E�pFBO���쐬
	const vec2 viewSize = GetViewSize();
	fboGameWindow = FramebufferObject::Create(
		static_cast<int>(viewSize.x), static_cast<int>(viewSize.y), FboType::colorDepth);
	
	//�ڂ����V�F�[�_��ǂݍ���
	progGaussianBlur = ProgramPipeline::Create(
		"Res/standard_2D.vert", "Res/gaussian_blur.frag");

	//�u���[���p�V�F�[�_�\��ǂݍ���
	progHighPassFilter = ProgramPipeline::Create(
		"Res/simple_2D.vert", "Res/high_pass_filter.frag");
	progDownSampling = ProgramPipeline::Create(
		"Res/simple_2D.vert", "Res/down_sampling.frag");
	progUpSampling = ProgramPipeline::Create(
		"Res/simple_2D.vert", "Res/up_sampling.frag");

	//�V�[���J�ڃG�t�F�N�g�p�̃t�@�C����ǂݍ���
	progFade = ProgramPipeline::Create("Res/simple_2D.vert", "Res/fade.frag");
	texFadeColor = Texture::Create("Res/fade_color.tga",
		GL_LINEAR, Texture::Usage::for3D);
	texFadeRule = Texture::Create("Res/fade_rule.tga",
		GL_LINEAR, Texture::Usage::for3DLinear);

	//���W�ϊ��s������j�t�H�[���ϐ��ɃR�s�[
	//�g�p���钸�_���W��-0.5~0.5�̃T�C�Y�Ȃ̂�2�{����
	//�X�v���C�g��FBO�̃e�N�X�`�����W�͋t�Ȃ̂�Y���}�C�i�X�ɂ���
	const mat4 matBloomM = {//���f���s��
		vec4(2,  0, 0, 0),
		vec4(0, -2, 0, 0),
		vec4(0,  0, 2, 0),
		vec4(0,  0, 0, 1),
	};

	const mat4 matBloomVP = mat4(1);
	const GLuint bloomProgramIdList[] = {
		*progHighPassFilter,*progDownSampling,*progUpSampling,*progFade };

	for (auto prog : bloomProgramIdList)
	{
		glProgramUniformMatrix4fv(prog, 0, 1, GL_FALSE, &matBloomM[0][0]);
		glProgramUniformMatrix4fv(prog, 1, 1, GL_FALSE, &matBloomVP[0][0]);
	}

	//���C����ʂ̃u���[���G�t�F�N�g�pFBO���쐬
	int bloomX = static_cast<int>(viewSize.x);
	int bloomY = static_cast<int>(viewSize.y);

	for (auto& e : fboBloom)
	{
		bloomX /= 2;
		bloomY /= 2;

		e = FramebufferObject::Create(bloomX, bloomY, FboType::color);
	}


	//���_�f�[�^��GPU�������ɃR�s�[
	struct Vertex {
		float x, y, z;	//���_���W
		float u, v;		//�e�N�X�`�����[
	};

	//�摜�̈ʒu���W�����߂�
	const Vertex vertexData[] = {
		{-0.5f ,-0.5f, 0 , 0 , 1},
		{ 0.5f , -0.5f, 0 , 1 , 1},
		{ 0.5f ,  0.5f, 0 , 1 , 0},
		{-0.5f ,  0.5f, 0 , 0, 0},
	};

	vbo = BufferObject::Create(sizeof(vertexData), vertexData);

	//�C���f�b�N�X�f�[�^��GPU�������ɃR�s�[
	const GLshort indexData[] = {
		0,1,2,2,3,0,
	};

	ibo = BufferObject::Create(sizeof(indexData), indexData);

	//���_�f�[�^�̍\������ݒ�
	vao = VertexArray::Create();
	glBindVertexArray(*vao);
	glBindBuffer(GL_ARRAY_BUFFER, *vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ibo);
	glEnableVertexAttribArray(0);		//���_������L���ɂ���	(�L���ɂ��钸�_�����z��̃C���f�b�N�X)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);//GPU�ɓ`�B����	(���_�����z��̃C���f�b�N�X�A�f�[�^�̗v�f���A�f�[�^�̌^�A���K���̗L���A���̃f�[�^�܂ł̃o�C�g���A�ŏ��̃f�[�^�ʒu)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(const void*)offsetof(Vertex, u));




	//�X�v���C�g�p�̂r�r�a�n���쐬
	ssboSprite = MappedBufferObject::Create(
		CalcSsboSize(maxSpriteCount * sizeof(Sprite)),
		GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);

	//3D���f���p�̃o�b�t�@���쐬
	meshBuffer = Mesh::MeshBuffer::Create(
		sizeof(Mesh::Vertex) * 300'000, sizeof(uint16_t) * 900'000);
	meshBuffer->CreateBox("Box");
	meshBuffer->CreateSphere("Sphere");
	for (int i = 1; i <= 8; ++i)
	{
		std::string s = "Capsule";
		s += std::to_string(i);
		meshBuffer->CreateCapsule(s.c_str(), static_cast<float>(i));
	}

	primitiveBuffer = Mesh::PrimitiveBuffer::Create(
		sizeof(Mesh::Vertex) * 250'000, sizeof(uint16_t) * 650'000);

	//���C���J�������쐬
	cameraObject = Create<GameObject>("Main Camera");
	cameraObject->AddComponent<Camera>();

	//������������
	std::random_device rd;	//�^�̗����𐶐�����I�u�W�F�N�g�i�x���j
	rg.seed(rd());			//�^���������u�^�̗����v�ŏ�����

	//�p�[�e�B�N���}�l�[�W���[���쐬
	particleManager = ParticleManager::Create(100'000);

	//�Q�[���I�u�W�F�N�g�z��̗e�ʂ�\��
	gameObjectList.reserve(1000);	//1000���\�񂵂��B

	//�������C�u������������
	if (!Audio::Initialize())
	{
		return 1;	//���������s
	}


	//IMGUI_CHECKVERSION();
	//ImGui::CreateContext();
	//ImGuiIO& io = ImGui::GetIO(); (void)io;

	//ImGui::StyleColorsDark();

	//ImGui_ImplGlfw_InitForOpenGL(window, true);
	//ImGui_ImplOpenGL3_Init("#verison 450");


	//�ŏ��̃V�[�����쐬���ď�����
	scene = std::make_shared<TitleScene>();
	if (scene)
	{
		scene->Initialize(*this);
	}


	return 0;//����ɏ��������ꂽ
}

//���C�����[�v�̎��s

int Engine::MainLoop()
{
	//ImGui_ImplOpenGL3_NewFrame();
	//ImGui_ImplGlfw_NewFrame();
	//ImGui::NewFrame();

	//FPS�v���p�̕ϐ�
	double fpsTime = glfwGetTime();		//�o�ߎ��Ԃ��擾
	double fpsCount = 0;				//�v���p
	double prevTime = glfwGetTime();	//�O��̃��[�v�Ōv����������

	//���C�����[�v
	while (!glfwWindowShouldClose(window))
	{
		//FPS�v��
		const double curTime = glfwGetTime();	//�o�ߎ��Ԃ��擾
		const double differTime = curTime - fpsTime;
		++fpsCount;

		//�P�b���Ƃ�FPS��\��
		if (differTime >= 1)
		{
			const std::string fps = std::to_string(fpsCount / differTime);		//�o�Ă����l�𕶎���ɕϊ�
			const std::string newTitle = title + "-FPS:" + fps;						//�^�C�g���̏�������
			glfwSetWindowTitle(window, newTitle.c_str());							//�^�C�g���̍ĕ\��

			//�v���p�ϐ������Z�b�g
			fpsTime = curTime;
			fpsCount = 0;
		}

		//�o�ߎ���(�f���^�^�C��)���v��
		float deltaTime = static_cast<float>(curTime - prevTime) * SlowSpeed;
		if (deltaTime > 1) {
			deltaTime = 1.0f / 60;
		}
		prevTime = curTime;

		//�V�[���̐؂�ւ�
		if (nextScene)
		{
			if (scene)
			{
				scene->Finalize(*this);
			}
			nextScene->Initialize(*this);
			scene = std::move(nextScene);
		}

		//�V�[�����X�V
		if (scene)
		{
			scene->Update(*this, deltaTime);
		}

		//Escape�L�[�������ƁA�}�E�X���ԋp�����
		if (this->GetKey(GLFW_KEY_ESCAPE))
		{
			//�}�E�X��Ԃ��Ă��炤
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}


		//�Q�[���I�u�W�F�N�g���X�V
		UpdateGameObject(deltaTime);		//�A�b�v�f�[�g
		CollideGameObject();				//�Փ˔���
		RemoveGameObject(gameObjectList);	//�Q�[���I�u�W�F�N�g������

		for (auto& layer : uiLayerList)
		{
			RemoveGameObject(layer.gameObjectList);
		}

		for (auto& layer : uiLayerList)
		{
			MakeSpriteList(layer.gameObjectList, layer.spriteList);
		}


		//GPU���o�b�t�@���g���I���̂�҂�
		ssboSprite->WaitSync();


		//�X�v���C�g�pSSBO�̃A�h���X���擾
		uint8_t* p = ssboSprite->GetMappedAddress();


		//�Q�[���E�B���h�E�p�̃X�v���C�g�f�[�^��SSBO�ɃR�s�[
		SpriteList spriteListPostProcess;

		if (!gameObjectList.empty())
		{
			//�|�X�g�v���Z�X�p�X�v���C�g��ǉ�
			const vec2 gameWindowPos = GetWindowSize() * 0.5f;
			spriteListPostProcess.push_back(
				Sprite{ gameWindowPos.x,gameWindowPos.y,0,1,
				Texcoord{0,1,1,-1}, 1, 1,1,1 });

			const size_t size = spriteListPostProcess.size() * sizeof(Sprite);
			memcpy(p, spriteListPostProcess.data(), size);
			p += CalcSsboSize(size);
		}

		//UI���C���[�̃X�v���C�g�f�[�^��SSBO�ɃR�s�[
		for (const auto& layer : uiLayerList)
		{
			memcpy(p, layer.spriteList.data(), layer.spriteList.size() * sizeof(Sprite));
			p += CalcSsboSize(layer.spriteList.size() * sizeof(Sprite));
		}

		//�p�[�e�B�N�����X�V
		//�J����z�������������悤�Ƀr���[�s����쐬
		const vec3 eye = cameraObject->GetPos();
		mat4 m =
			mat4::RotateY(cameraObject->GetRotation().y) *
			mat4::RotateX(cameraObject->GetRotation().x) *
			mat4::RotateZ(cameraObject->GetRotation().z);

		const mat3 matBillboard(m);

		const vec3 target = vec3(m * vec4(0, 0, -1, 1));

		const mat4 matParticleView = mat4::LookAt(eye, eye + target, vec3(0, 1, 0));
		particleManager->Update(matParticleView, deltaTime);
		particleManager->UpdateSSBO();

		//�`�����Q�[���E�B���h�E�ɕύX
		glBindFramebuffer(GL_FRAMEBUFFER, *fboGameWindow);
		glDisable(GL_FRAMEBUFFER_SRGB);	//�K���}�␳�𖳌��ɂ���
		const vec2 viewSize = GetViewSize();

		glViewport(0, 0,
			static_cast<int>(viewSize.x), static_cast<int>(viewSize.y));

		glBindVertexArray(*vao);

		//�o�b�N�o�b�t�@���N���A
		glClearColor(0.1f, 0.2f, 0.6f, 0.0f);//��,��,��,�����x(0~1�̒l����)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//�X�J�C�{�b�N�X��`��
		{
			//�[�x�o�b�t�@�ւ̏������݂��s��Ȃ����ƂŁA�ł������Ǝv�킹��
			glDisable(GL_DEPTH_TEST);

			//�����̗̂�����`�悵�����̂ŁA�J�����O�Ώۂ�\���ɂ���
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);

			//�A���t�@�u�����h�͎g��Ȃ��̂ŕ`��̍������̂��߂ɖ�����
			glDisable(GL_BLEND);

			//�L���[�u�}�b�v�̂Ȃ��ڂ�ڗ����Ȃ�����@�\��L����
			glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

			//�r���[�|�[�g��ݒ�
			Camera& camera = static_cast<Camera&>(*cameraObject->componentList[0]);
			glViewport(
				camera.viewport.x, camera.viewport.y,
				camera.viewport.width, camera.viewport.height);

			//�X�J�C�{�b�N�X�p�̃J��������ݒ�
			glProgramUniform4f(*progSkyBox, 3,
				camera.fovScaleX, camera.fovScaleY, camera.A, camera.B);

			glProgramUniform4f(*progSkyBox, 5,
				sin(cameraObject->GetRotation().x), cos(cameraObject->GetRotation().x),
				sin(cameraObject->GetRotation().y), cos(cameraObject->GetRotation().y));
		
			//�����̂ɃL���[�u�}�b�v��`��
			const auto& skybox = meshBuffer->LoadOBJ("Res/Model/box.obj");
			const auto& prim = skybox->primitives[0];
			glUseProgram(*progSkyBox);
			glBindVertexArray(*meshBuffer->GetVAO());
			glBindTextureUnit(0, *texSkyBox);

			glDrawElementsBaseVertex(prim.mode, prim.count,
				GL_UNSIGNED_SHORT, prim.indices, prim.baseVertex);

			glCullFace(GL_BACK);	//�J�����O�Ώۂ����ɖ߂�
		}

		//�`��Ɏg���V�F�[�_���w��
		glUseProgram(*progSprite);

		//�[�x�e�X�g�𖳌���
		glDisable(GL_DEPTH_TEST);

		//���ʃJ�����O�𖳌���
		glDisable(GL_CULL_FACE);

		//�A���t�@�u�����h��L���ɂ���
		glEnable(GL_BLEND);		//�w�肵���@�\��L���ɂ���
		glBlendEquation(GL_FUNC_ADD);	//�����������̂���	
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	//�����������̂���

#if 0
		//���j�t�H�[���ϐ��Ƀf�[�^���R�s�[����
		const float timer = static_cast<float>(glfwGetTime());//�N��������̎��Ԍo�߂��擾
		glProgramUniform1f(progSprite, 0, timer);//�v���O�����I�u�W�F�N�g�̊Ǘ��ԍ��A�����̃��P�[�V�����ԍ��A����f�[�^
#endif

		//�X�v���C�g�`��p��VAO��ݒ�
		glBindVertexArray(*vao);

		//2D�w�i��`��
		size_t spriteSsboOffset = 0;


		//3D���b�V����`��
		DrawStaticMesh(gameObjectList);
		
		//�p�[�e�B�N����`��
		{
			//�`��͈͂�ݒ�
			const Camera& camera = static_cast<Camera&>(*cameraObject->componentList[0]);
			glViewport(camera.viewport.x, camera.viewport.y,
				camera.viewport.width, camera.viewport.height);

			//�v���W�F�N�V�����s����쐬
			const float w = static_cast<float>(camera.viewport.width);
			const float h = static_cast<float>(camera.viewport.height);

			const mat4 marParticleProj = mat4::Perspective(
				radians(camera.fovY), w / h, camera.near, camera.far);

			//�p�[�e�B�N����`��
			particleManager->Draw(marParticleProj, matParticleView, matBillboard);
		}

		//�u���[����`��
		glBindVertexArray(*vao);
		DrawBloomEffect();

		//�f�t�H���g�t���[���o�b�t�@�ɖ߂�
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_FRAMEBUFFER_SRGB);	//�K���}�␳��L���ɂ���
		glViewport(0, 0, 1280, 720);
		glProgramUniform4f(*progSprite, 2, 2.0f / 1280, 2.0f / 720, -1, -1);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//�Q�[���E�B���h�E��`��
		glUseProgram(*progSprite);
		glProgramUniform4f(*progSprite, 2, 2.0f / 1280, 2.0f / 720, -1, -1);

		if (!gameObjectList.empty())
		{
			//�ڂ������x���O���傫����΂ڂ����V�F�[�_���g��
			if (blurStrength > 0)
			{
				glUseProgram(*progGaussianBlur);
				const vec2 windowSize = GetWindowSize();

				glProgramUniform4f(*progGaussianBlur, 2,
					2 / windowSize.x, 2 / windowSize.y, -1, -1);

				glProgramUniform1f(*progGaussianBlur, 100, blurStrength);

				DrawSpriteList(spriteListPostProcess,
					*fboGameWindow->GetColorTexture(), spriteSsboOffset);

				glUseProgram(*progSprite);
			}
			else
			{
				DrawSpriteList(spriteListPostProcess,
					*fboGameWindow->GetColorTexture(), spriteSsboOffset);
			}
		}

		//UI���C���[�̃X�v���C�g�z���`��
		for (const auto& layer : uiLayerList)
		{
			DrawSpriteList(layer.spriteList, *layer.tex, spriteSsboOffset);
		}

		//�V�[���J�ڃG�t�F�N�g

		//�t�F�[�h���J���Ă��Ȃ�������
		if (fadeState != FadeState::Open)
		{
			glUseProgram(*progFade);
			glBindVertexArray(*vao);

			//�e�N�X�`����OpenGl�R���e�L�X�g�Ɋ��蓖�Ă�
			const GLuint tex[] = { *texFadeColor,*texFadeRule };
			glBindTextures(0, 2, tex);
			
			//���P�[�V�����ԍ�
			const GLint locFadeThreshold = 100;

			//�t�F�[�h��臒l
			const float FadeThreshold =
				FadeFrom + (FadeTo - FadeFrom) * (FadeTimer / FadeTotalTime);

			//�V�F�[�_�[�ɂ�����
			glProgramUniform1f(*progFade, locFadeThreshold, FadeThreshold);

			//�G�t�F�N�g��`��
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

			glBindTextures(0, 1, nullptr);
			glBindVertexArray(0);
			glUseProgram(0);

			//�^�C�}�[�̍X�V
			FadeTimer += deltaTime;

			//�^�C�}�[���������쎞�Ԉȏ�ɂȂ������Ԃ��X�V
			if (FadeTimer >= FadeTotalTime)
			{
				FadeTimer = FadeTotalTime;

				//臒l�̕ω�������0~1�Ȃ�open,����ȊO��closed��Ԃɂ���
				if (FadeFrom < FadeTo)
				{
					fadeState = FadeState::Open;
				}
				else
				{
					fadeState = FadeState::Closed;
				}
			}
		}

		//�X�v���C�g�pSSBO�̊��蓖�Ă���������
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, 0, 0, 0);//������SSBO�̊Ǘ��ԍ����O�ɂ���

		//�X�v���C�g�`��̒���Ƀt�F���X���쐬
		ssboSprite->SwapBuffers();


//�������C�u�������X�V
		Audio::Update();
		
		//ImGui::Render();
		//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	return 0;
}

//�Q�[���G���W�����I������
//		@retval 0�@�@�@����ɏI������
//		@retval 0�ȊO�@�G���[����������

int Engine::Finalize()
{
	//�������C�u�������I��
	Audio::Finalize();
	//ImGui_ImplOpenGL3_Shutdown();
	//ImGui_ImplGlfw_Shutdown();
	//ImGui::DestroyContext();
	//GLFW�̏I��
	glfwTerminate();

	return 0;
}

//�Q�[���I�u�W�F�N�g�̏�Ԃ��X�V
void Engine::UpdateGameObject(float deltaTime)
{
	//�������ɔz�񂪕ω����Ȃ��悤�ɃR�s�[���쐬
	std::vector<GameObject*> list(gameObjectList.size());
	std::transform(gameObjectList.begin(), gameObjectList.end(), list.begin(),//�ϊ��͈͂̐擪�A�I�[�A�R�s�[��̐擪
		[](const GameObjectPtr& e) {return e.get(); });//�ϊ��֐�


	//UI���C���[�̃Q�[���I�u�W�F�N�g�̃R�s�[���쐬
	for (auto& layer : uiLayerList)
	{
		const size_t prevListSize = list.size();
		list.resize(prevListSize + layer.gameObjectList.size());
		std::transform(layer.gameObjectList.begin(), layer.gameObjectList.end(),
			list.begin() + prevListSize,
			[](const GameObjectPtr& e) {return e.get(); });


	}
	//�J�����I�u�W�F�N�g��ǉ�
	if (cameraObject)
	{
		list.push_back(cameraObject.get());
	}

	//Start�C�x���g
	for (auto& e : list)
	{
		e->Start();
	}

	//Update�C�x���g
	for (auto& e : list)
	{
		if (!e->GetDead())
		{
			e->Update(deltaTime);
		}
	}
}

//�Q�[���I�u�W�F�N�g�̏Փ˔���
void Engine::CollideGameObject()
{

	//���[���h���W�̃R���C�_�[���v�Z
	std::vector<WorldCollider> tmp;
	tmp.reserve(gameObjectList.size());

	for (auto& e : gameObjectList)
	{
		//�X�^�[�g�����Ă��Ȃ��A�܂��͎���ł����͔��肵�Ȃ�
		if (!e->IsStarted() || e->GetDead())
		{
			continue;
		}

		//���[���h���W�����[�J�����W�{�Q�[���I�u�W�F�N�g�̍��W
		const auto& local = e->colliderList;

		//�������R���C�_�[�������Ă���Q�[���I�u�W�F�N�g�����Ȃ�������
		if (local.empty())
		{
			//��������
			continue;
		}

		const mat3 matRotation = mat3(mat4::RotateY(e->GetRotation().y) *
			mat4::RotateX(e->GetRotation().x) * mat4::RotateZ(e->GetRotation().z));

		WorldCollider world;
		world.gameObject = e.get();
		world.colliderList.resize(local.size());
		
		for (size_t i = 0; i < local.size(); ++i)
		{
			world.colliderList[i] = local[i]->GetTransformedCollider(e->GetPos(), matRotation, e->GetScale());
		}
		tmp.push_back(world);
	}

	for (auto pa = tmp.begin(); pa != tmp.end(); ++pa)
	{
		if (pa->gameObject->only > 0)
		{
			--pa->gameObject->only;
		}
	}

	//�R���C�_�[�̏Փ˔���
	for (auto pa = tmp.begin(); pa != tmp.end(); ++pa)
	{
		for (auto pb = pa + 1; pb != tmp.end(); ++pb)
		{

			if (pb->gameObject->GetDead())
			{
				continue;//�Փ˂��Ă��Ȃ�
			}

			auto cpList = pa->DetectCollision(*pb);

			if (!cpList.empty())
			{

				//�Փ˂��Ă���̂ŁA�Փ˃C�x���g�֐������s
				pa->gameObject->OnCollision(*pb->gameObject);
				pb->gameObject->OnCollision(*pa->gameObject);
				
				if (pa->gameObject->GetDead())
				{
					break;//pa������ł���Ȃ烋�[�v��ł��؂�
				}
			}
		}
		
	}

	//�n�ʂɒ��n���Ă��Ȃ������󒆔���Ƃ���
	for (auto pa = tmp.begin(); pa != tmp.end(); ++pa)
	{
		if (pa->gameObject->only <= 0)
		{
			pa->gameObject->SetAir(true);
		}
		else
		{
			pa->gameObject->SetAir(false);
		}
	}

}

//���S��Ԃ̃Q�[���I�u�W�F�N�g���폜
void Engine::RemoveGameObject(GameObjectList& gameObjectList)
{
	//�����Ă���I�u�W�F�N�g�Ǝ���ł���I�u�W�F�N�g�ɕ�����
	const auto i = std::stable_partition(gameObjectList.begin(), gameObjectList.end(),//�͈͂̐擪�A�I���A�ӂ�킯����
		[](const GameObjectPtr& e) { return !e->GetDead(); }//�֐�����т������߂ɁAremove�ł͂Ȃ��Apartition
	);

	//����ł���Q�[���I�u�W�F�N�g��ʂ̔z��Ɉړ�
	GameObjectList deadList(
		std::make_move_iterator(i),
		std::make_move_iterator(gameObjectList.end())
	);

	//�z�񂩂�ړ��ς݃I�u�W�F�N�g������
	gameObjectList.erase(i, gameObjectList.end());

	//����ł���I�u�W�F�N�g�ɑ΂���Destoroy�C�x���g�����s
	for (auto& e : deadList)
	{
		e->OnDestroy();
	}
}

//�Q�[���I�u�W�F�N�g����X�v���C�g�z����쐬
void Engine::MakeSpriteList(GameObjectList& gameObjectList, SpriteList& spriteList)
{
	//�����̓������̗p�̃X�v���C�g�z�����ɂ���
	spriteList.clear();

	//�D�揇�ʂ��傫�����Ԃɕ��ѕς���
	std::stable_sort(
		gameObjectList.begin(), gameObjectList.end(),//�擪�A�I�[
		[](const GameObjectPtr& a, const GameObjectPtr& b)//���וς�����
		{return a->GetPriority() > b->GetPriority(); }
	);
	

	//�Q�[���I�u�W�F�N�g�̃X�v���C�g�f�[�^���X�v���C�g�z��ɒǉ�
	//�����Ƀ��[���h���W�֕ϊ�����
	for (const auto& e : gameObjectList)
	{
		for (const auto& sprite : e->spriteList)
		{
			spriteList.push_back(sprite);
			Sprite& s = spriteList.back();
			s.x += e->GetPos().x;
			s.y += e->GetPos().y;
			s.z = e->GetRotation().x;
			s.w = e->GetAlpha();
			s.size = e->GetScale().x;
			s.red = 1;
			s.green = 1;
			s.blue = 1;
		}
	}
}

//�X�v���C�g�z���`�悷��

//@param spriteList			�`�悷��X�v���C�g�z��
//@param tex				�`��Ɏg�p����e�N�X�`��
//@param spriteSsboOffset	SSBO���̑f�����ƃf�[�^�J�n�ʒu

void Engine::DrawSpriteList(const SpriteList& spriteList, GLuint tex, size_t& spriteSsboOffset)
{

	const size_t size = CalcSsboSize(spriteList.size() * sizeof(Sprite));

	if (size == 0)
	{
		return;	//�`��f�[�^���Ȃ���΃X�L�b�v
	}


	//�e�N�X�`�����w�肷��
	glBindTextures(0, 1, &tex);//���蓖�ĊJ�n�C���f�b�N�X�A���蓖�Ă���A�e�N�X�`���Ǘ��ԍ��z��̃A�h���X

	//�X�v���C�g�pSSBO�����蓖�Ă�
	ssboSprite->Bind(0, spriteSsboOffset, size);
	spriteSsboOffset += size;

	//�}�`��`��
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0,
		static_cast<GLsizei>(spriteList.size()));

}

//3D���b�V����`��

void Engine::DrawStaticMesh(GameObjectList& gameObjectList)
{

	//���s����
	struct DirectionalLight
	{
		vec3 color;			//�F
		float intensity;	//���邳
		vec3 direction;		//����
	};
	
	DirectionalLight directionalLight = {
		vec3(1.0f,0.9f,1.0f),
		5, 
		vec3(0.57735f,-0.57735f,-0.57735f)
	};

	//�[�x�}�b�v�쐬�p�̒����X���W
	const vec2 viewSize = GetViewSize();
	const vec3 shadowTarget =
		vec3(viewSize.x * 0.5f, viewSize.y * 0.5f, 0);

	//�[�x�}�b�v�쐬�p�̎��_���W
	const vec3 shadowEye =
		shadowTarget - directionalLight.direction * 1000;

	//�[�x�}�b�v���쐬
	const mat4 matShadow = CreateDepthMap(
		gameObjectList, shadowEye, shadowTarget, 750, 100, 2000);

	//�Q�[���E�B���h�E�pFBO�ɕ`��
	glBindFramebuffer(GL_FRAMEBUFFER, *fboGameWindow);
	glUseProgram(*progStandard3D);
	glBindVertexArray(*meshBuffer->GetVAO());

	//�[�x�e�X�g��L����
	glEnable(GL_DEPTH_TEST);

	//���ʃJ�����O��L����
	glEnable(GL_CULL_FACE);

	//�A���t�@�u�����h��L����
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//�e�`��p�̐[�x�}�b�v���e�N�X�`���Ƃ��Ċ��蓖�Ă�
	const GLuint texDepth = *fboShadow->GetDepthTexture();
	glBindTextures(2, 1, &texDepth);

	//�e���W����e�N�X�`�����W�ɕϊ�����s��
	const mat4 matTexture = {
		vec4(0.5f, 0.0f, 0.0f, 0.0f),
		vec4(0.0f, 0.5f, 0.0f, 0.0f),
		vec4(0.0f, 0.0f, 0.5f, 0.0f),
		vec4(0.5f, 0.5f, 0.5f, 1.0f),
	};
	const mat4 matShadowTexture = matTexture * matShadow;
	glProgramUniformMatrix4fv(*progStandard3D, 1, 1, GL_FALSE, &matShadowTexture[0][0]);
	
	//���s�����̏���ݒ�
	const vec3 color = directionalLight.color * directionalLight.intensity;
	glProgramUniform3f(*progStandard3D, 100, color.x, color.y, color.z);

	glProgramUniform3f(*progStandard3D, 101,
		directionalLight.direction.x,
		directionalLight.direction.y,
		directionalLight.direction.z);

	//�����̏���ݒ�
	const vec3 ambientLight = vec3(0.2f, 0.2f, 0.3f);
	glProgramUniform3f(*progStandard3D, 104,
		ambientLight.x, ambientLight.y, ambientLight.z);

	//3D�\���p�̃J��������ݒ�

	//����p���̏��
	Camera& camera = static_cast<Camera&>(*cameraObject->componentList[0]);
	glProgramUniform4f(*progStandard3D, 3,
		camera.fovScaleX, camera.fovScaleY, camera.A, camera.B);

	//�J�����̍��W
	glProgramUniform3fv(*progStandard3D, 4,
		1,const_cast<GLfloat*>(&cameraObject->GetPos().x));

	//�J�����̉�]�p(x)
	glProgramUniform2f(*progStandard3D, 5,
		sin(cameraObject->GetRotation().x), cos(cameraObject->GetRotation().x));

	//�J�����̉�]�p(y)
	glProgramUniform2f(*progStandard3D, 6,
		sin(cameraObject->GetRotation().y), cos(cameraObject->GetRotation().y));

	//�r���[�|�[�g��ݒ�
	glViewport(
		camera.viewport.x, camera.viewport.y,
		camera.viewport.width, camera.viewport.height);

	//���b�V�������_���������ׂẴQ�[���I�u�W�F�N�g��`��
	for (const auto& gameObject : gameObjectList)
	{
		gameObject->DrawStaticMesh(*progStandard3D);
	}

	glBindTextures(0, 2, nullptr);
	glBindVertexArray(0);
	glUseProgram(0);

}

//�[�x�}�b�v���쐬����

//@param gameObjectList �`�悷��Q�[���I�u�W�F�N�g�̔z��
//@param eye            �e�`��p�̎��_���W
//@param target         �e�`��p�̒����_�̍��W
//@param range          eye����e�`��͈͂̏�[�A���[�A���[�A�E�[�܂ł̋���
//@param zNear          eye����e�`��͈͂̎�O�[�܂ł̋���
//@param zFar           eye����e�`��͈͂����[�܂ł̋���

//@return �e�`��p�r���[�v���W�F�N�V�����s��

mat4 Engine::CreateDepthMap(GameObjectList& gameObjectList,
	const vec3& eye, const vec3& target,
	float range, float zNear, float zFar)
{
	//�`�����e�p�e�a�n�ɕύX
	glBindFramebuffer(GL_FRAMEBUFFER, *fboShadow);
	glClear(GL_DEPTH_BUFFER_BIT);

	//�r���[�|�[�g���e�a�n�T�C�Y�ɍ��킹��
	const GLuint texDepth = *fboShadow->GetDepthTexture();
	GLint w, h;
	glGetTextureLevelParameteriv(texDepth, 0, GL_TEXTURE_WIDTH, &w);//�e�pFBO�T�C�Y
	glGetTextureLevelParameteriv(texDepth, 0, GL_TEXTURE_WIDTH, &h);
	glViewport(0, 0, w, h);

	//�`��p�����[�^��ݒ�
	glBindVertexArray(*meshBuffer->GetVAO());	//3D���f���pVAO���o�C���h
	glUseProgram(*progShadow);	//�e�p�`��p�V�F�[�_���w��
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);		//�A���t�@�u�����h�𖳌���

	//�e�p�r���[�s����쐬
	const mat4 matShadowView = mat4::LookAt(eye, target, vec3(0, 1, 0));

	//�e�p�v���W�F�N�V�����s����쐬
	const mat4 matShadowProj = mat4::Orthogonal(
		-range, range,
		-range, range,
		zNear, zFar);

	//�e�p�r���[�v���W�F�N�V�����s����쐬
	const mat4 matShadow = matShadowProj * matShadowView;

	//matShadow�����j�t�H�[���ϐ��ɃR�s�[
	glProgramUniformMatrix4fv(*progShadow, 1, 1, GL_FALSE, &matShadow[0][0]);

	//���b�V�������_���������ׂẴQ�[���I�u�W�F�N�g��`��
	for (const auto& gameObject : gameObjectList)
	{
		gameObject->DrawStaticMesh(*progShadow);
	}

	return matShadow;
}

//�J���[�e�N�X�`����FBO�ɕ`�悷��
//@param fbo      �`���FBO
//@param texture  FBO�ɕ`�悷��e�N�X�`��
//@param wrapMode �e�N�X�`���ɐݒ肷�郉�b�v���[�h
void DrawTextureToFbo(FramebufferObject& fbo,
	Texture& texture, GLenum wrapMode)
{
	const Texture& texFbo = *fbo.GetColorTexture();
	glViewport(0, 0, texFbo.GetWidth(), texFbo.GetHeight());
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	texture.SetWrapMode(wrapMode);
	glBindTextureUnit(0, texture);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

}

void Engine::DrawBloomEffect()
{
	//�u���[���G�t�F�N�g�p��GL�R���e�L�X�g��ݒ�
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	//�Q�[����ʗpFBO���獂�P�x�����𒊏o
	glUseProgram(*progHighPassFilter);
	glDisable(GL_BLEND);//�������������Ȃ�����
	{
		//���P�x������FBO�̖����������Z�����̂ŁA���̉e����ł��������߂ɖ����Ŋ���
		const float strength = bloomStrength / static_cast<float>(std::size(fboBloom));
		glProgramUniform2f(*progHighPassFilter, 100, bloomThreshold, strength);

		DrawTextureToFbo(*fboBloom[0],
			*fboGameWindow->GetColorTexture(), GL_CLAMP_TO_BORDER);

	}

	//�k���ڂ������s���A�P�i������FBO�ɃR�s�[
	glUseProgram(*progDownSampling);

	for (size_t i = 1; i < std::size(fboBloom); ++i)
	{
		DrawTextureToFbo(*fboBloom[i],
			*fboBloom[i - 1]->GetColorTexture(), GL_CLAMP_TO_BORDER);
	}
	
	//�g��ڂ������s���A�P�i�傫��FBO�ɉ��Z����
	glUseProgram(*progUpSampling);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);//���Z����(�V�F�[�_�o�́{FBO)

	for (size_t i = std::size(fboBloom) - 1; i > 0;)
	{
		--i;
		DrawTextureToFbo(*fboBloom[i],
			*fboBloom[i + 1]->GetColorTexture(), GL_CLAMP_TO_EDGE);
	}

	//�Ō�̊g��ڂ������s���A�Q�[����pFBO�ɉ��Z����
	{
		DrawTextureToFbo(*fboGameWindow,
			*fboBloom[0]->GetColorTexture(), GL_CLAMP_TO_EDGE);
	}
}

