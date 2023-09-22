//@file Engine.h

#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED
#include "glad/glad.h"
#include "Sprite.h"
#include "../Application/Scene.h"
#include "../Application/TileMap.h"
#include "../Application/Particle.h"
#include "../Application/GameObject.h"
#include "VecMath.h"
#include "../Component/Camera.h"
#include "FramebufferObject.h"
#include "BufferObject.h"
#include "VertexArray.h"
#include <GLFW/glfw3.h>
#include <string>
#include <random>
#include <memory>


//��s�錾
class ProgramPipeline;
using ProgramPipelinePtr = std::shared_ptr<ProgramPipeline>;
class Texture;
using TexturePtr = std::shared_ptr<Texture>;

namespace Mesh
{
	class PrimitiveBuffer;
	using PrimitiveBufferPtr = std::shared_ptr<PrimitiveBuffer>;

	class MeshBuffer;
	using MeshBufferPtr = std::shared_ptr<MeshBuffer>;

	struct StaticMesh;
	using StaticMeshPtr = std::shared_ptr<StaticMesh>;
}

//�Q�[���G���W��

class Engine
{
public:
	int Run();			//���C���𓮂���

	float SlowSpeed = 1;//�Q�[���I�u�W�F�N�g����deltaTime��������

	//�Q�b�^�[�R�c
	GLFWwindow* GetWindow() { return window; }
	std::mt19937& GetRandomGenerator() { return rg; }
	int GetMapLevel() { return MapLevel; }
	float GetblurStrength() { return blurStrength; }


	void SetMapLevel(int _MapLevel) { MapLevel = _MapLevel; }
	void SetblurStrength(float _strength) { blurStrength = _strength; }



	//3D���f���z�u�f�[�^�̓ǂݍ���
	GameObjectList LoadGameObjectMap(const char* filename,
		const VecMath::vec3& translate = VecMath::vec3(0),
		const VecMath::vec3& scale = VecMath::vec3(1));

	//���̃V�[����ݒ肷��
	template<typename T>
	void SetNextScene() { nextScene = std::make_shared<T>(); }

	//�V�[���J�ڃG�t�F�N�g���
	enum class FadeState
	{
		Fading,	//�t�F�[�h�C���E�A�E�g�����s��
		Open,	//�t�F�[�h�C������
		Closed,	//�t�F�[�h�A�E�g����
	};
	//�V�[���J�ڃG�t�F�N�g�̏�Ԃ��擾
	FadeState GetFadeState() const { return fadeState; }

	static constexpr float DefaultFadeTime = 1;

	//�t�F�[�h�C���E�A�E�g�̊J�n
	void StartFadeIn(float time = DefaultFadeTime);
	void StartFadeOut(float time = DefaultFadeTime);

	//�t�F�[�h�̎d���̉摜��ݒ�
	void SetFadeRule(std::string _rule)
	{
		texFadeRule = Texture::Create(_rule,
			GL_LINEAR, Texture::Usage::for3DLinear);
	}
	
	//�t�F�[�h�̐F��ύX
	void SetFadeColor(std::string _rule)
	{
		texFadeColor = Texture::Create(_rule,
			GL_LINEAR, Texture::Usage::for3DLinear);
	}

	//�Q�[���I�u�W�F�N�g���쐬����
	template<typename T>
	std::shared_ptr<T> Create(const std::string& name,
		const VecMath::vec3& position = { 0,0,0 })//�Q�[���I�u�W�F�N�g���X�g�ɐ������āA�ǉ�����
	{
		std::shared_ptr<T> p = std::make_shared<T>();
		p->engine = this;
		p->name = name;
		p->SetPos(position);
		gameObjectList.push_back(p);
		return p;
	}

	//�Q�[���I�u�W�F�N�g���폜����
	void ClearGameObjectList() { gameObjectList.clear(); }

	//�`��𑜓x
	VecMath::vec2 GetViewSize() const { return { 1280,720 }; }

	//�E�B���h�E�T�C�Y
	VecMath::vec2 GetWindowSize() const { return { 1280,720 }; }

	//�L�[��������Ă�����true,������Ă��Ȃ����false
	bool GetKey(int key)const
	{
		return glfwGetKey(window, key) == GLFW_PRESS;//�E�B���h�E�A�h���X�A�L�[�ԍ�
	}

	//�z�u�f�[�^�̓ǂݍ���
	TileMap LoadTileMap(const char* filename);	//�t�@�C���̓ǂݍ���

	size_t AddUILayer(const char* image, GLenum filterMode, size_t reserve);
	void ClearUILayers();         

	//UI�p�Q�[���I�u�W�F�N�g���쐬����

	//@param layerNo �ǉ���̃��C���[�ԍ�
	//@param		 name�Q�[���I�u�W�F�N�g�̖��O
	//@param		 x�Q�[���I�u�W�F�N�g��z�u����X���W
	//@param		 y�Q�[���I�u�W�F�N�g��z�u����Y���W
	//@return �쐬����UI�p�Q�[���I�u�W�F�N�g

	template<typename T>
	std::shared_ptr<T> CreateUI(size_t layerNo, const std::string& name,
		float x = 0, float y = 0)
	{
		if (layerNo >= uiLayerList.size())
		{
			return nullptr;
		}
		std::shared_ptr<T> p = std::make_shared<T>();
		p->engine = this;
		p->name = name;
		p->SetPos(vec3(x, y, 1));
		uiLayerList[layerNo].gameObjectList.push_back(p);

		return p;
	}

	Mesh::StaticMeshPtr LoadOBJ(const char* filename);

	//���C���J�������擾
	GameObjectPtr GetMainCameraObject() const { return cameraObject; }

	std::shared_ptr<Camera> GetMainCamera() const {
		return std::static_pointer_cast<Camera>(cameraObject->componentList[0]);}

	//�v���[����臒l�̎擾�E�ݒ�
	float GetBloomThreshold() const { return bloomThreshold; }
	void SetBloomThreshold(float _t) { bloomThreshold = _t; }

	//�v���[���̋����̎擾�E�ݒ�
	float GetBloomStrength() const { return bloomStrength; }
	void SetBloomStrength(float _s) { bloomStrength = _s; }

	//�p�[�e�B�N���G�~�b�^�̑���
	ParticleEmitterPtr AddParticleEmitter(
		const ParticleEmitterParameter& ep, const ParticleParameter& pp);

	//�p�[�e�B�N���̌���
	ParticleEmitterPtr FindParticleEmitter(const std::string& name) const;

	//�p�[�e�B�N���̍폜
	void RemoveParticleEmitter(const ParticleEmitterPtr&);
	void RemoveParticleEmitterAll();

	//�Q�[���I�u�W�F�N�g���X�g���疼�O����
	GameObjectPtr SearchGameObject(std::string name)
	{
		for (auto obj : gameObjectList)
		{
			if (obj->name == name)
			{
				return obj;
			}
		}
		return nullptr;
	}

private:
	int Initialize();	//������
	int MainLoop();		//���C�����[�v
	int Finalize();		//�I��

	void UpdateGameObject(float deltaTime);	//�A�b�v�f�[�g
	void CollideGameObject();				//�Փ˔���
	void RemoveGameObject(GameObjectList& gameObjectList);										//�Q�[���I�u�W�F�N�g������
	void MakeSpriteList(GameObjectList& gameObjectList,SpriteList& spriteList);					//�X�v���C�g���X�g�����
	void DrawSpriteList(const SpriteList& spriteList, GLuint tex, size_t& spriteSsboOffset);	//�X�v���C�g���X�g�̕`��
	void DrawStaticMesh(GameObjectList& gameObjectList);	//���b�V���`��
	
	VecMath::mat4 CreateDepthMap(GameObjectList& gameObjectList,
		const VecMath::vec3& eye, const VecMath::vec3& target,
		float range, float zNear, float zFar);

	void DrawBloomEffect();//�u���[���G�t�F�N�g�`��p�֐�

	GLFWwindow* window = nullptr;				//�E�B���h�E�I�u�W�F�N�g
	const std::string title = "Last Trigger";	//�E�B���h�E�^�C�g��

	GLuint vs = 0;					//���_�V�F�[�_�̊Ǘ��ԍ�
	GLuint fs = 0;					//�t���O�����g�V�F�[�_�Ǘ��ԍ�
	BufferObjectPtr vbo;			//���_�o�b�t�@�̊Ǘ��ԍ�
	BufferObjectPtr ibo;			//�C���f�b�N�X�o�b�t�@�̊Ǘ��ԍ�
	VertexArrayPtr  vao;			//�\�����̊Ǘ��ԍ�
	ProgramPipelinePtr progSprite;	//�v���O�����I�u�W�F�N�g�̊Ǘ��ԍ�

	int MapLevel = 0;
	const size_t maxSpriteCount = 100'000;		//�X�v���C�g�̍ő吔


	MappedBufferObjectPtr ssboSprite;//�}�b�v���Ȃ�buffer�I�u�W�F�N�g


	std::mt19937 rg;			//�^�������𐶐�����I�u�W�F�N�g�i�����j

	const size_t bgSizeX = 17;	//���̃^�C����
	const size_t bgSizeY = 110;	//�c�̃^�C����

	float bgBaseX = 16;				//�w�i�̔z�u�J�n�����W
	float bgBaseY = 16;				//�w�i�̔z�u�J�ny���W

	//3D���f���p�̕ϐ�
	ProgramPipelinePtr progStandard3D;
	Mesh::PrimitiveBufferPtr primitiveBuffer;
	GameObjectPtr cameraObject;
	Mesh::MeshBufferPtr meshBuffer;

	FramebufferObjectPtr fboGameWindow;	//�Q�[���E�B���h�E�pFBO

	ScenePtr scene;		//���s���̃V�[��
	ScenePtr nextScene;	//���̃V�[��
	GameObjectList gameObjectList;	//�Q�[���I�u�W�F�N�g�z��

	//3D���f���̉e�`��p�̕ϐ�
	FramebufferObjectPtr fboShadow;
	ProgramPipelinePtr progShadow;

	//�X�J�C�{�b�N�X�p�̕ϐ�
	TexturePtr texSkyBox;
	ProgramPipelinePtr progSkyBox;

	//�G�t�F�N�g�p�̕ϐ�
	ProgramPipelinePtr progGaussianBlur;	//�ڂ����V�F�[�_
	float blurStrength = 0;					//�ڂ������x(0 ~ 1)

	ProgramPipelinePtr progHighPassFilter;	//���P�x�𒊏o����V�F�[�_�[
	ProgramPipelinePtr progDownSampling;	//�k���V�F�[�_�\
	ProgramPipelinePtr progUpSampling;		//�g��V�F�[�_�\

	FramebufferObjectPtr fboBloom[6];		//�ڂ���FBO

	float bloomThreshold = 1.0f;	//���P�x�Ƃ݂Ȃ����邳(0~�������)
	float bloomStrength = 4;		//�u���[���̋���

	//�V�[���J�ڃG�t�F�N�g�p�̕ϐ�
	ProgramPipelinePtr progFade;	//�t�F�[�h�p�V�F�[�_�[
	TexturePtr texFadeColor;		//�t�F�[�h�̃J���[�摜
	TexturePtr texFadeRule;			//�t�F�[�h�̃��[���摜
	float FadeFrom = 0;				//�t�F�[�h�̊J�n��臒l
	float FadeTo = 1;				//�t�F�[�h�̏I������臒l
	float FadeTotalTime = 1;		//�t�F�[�h�ɂ����鎞��
	float FadeTimer = 0;			//�t�F�[�h�̌o�ߎ���

	FadeState fadeState = FadeState::Open;//�G�t�F�N�g���

	//UI�p�Q�[���I�u�W�F�N�g�̃��C���[�\��
	struct UILayer {
		TexturePtr tex;
		GameObjectList gameObjectList;
		std::vector<Sprite> spriteList;
	};
	std::vector<UILayer> uiLayerList;	//UI���C���[�z��

	//�p�[�e�B�N���Ǘ�
	ParticleManagerPtr particleManager;
};
#endif //ENGINE_H_INCLUDED