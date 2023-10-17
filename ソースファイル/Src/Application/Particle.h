//@file Particle.h

#ifndef PARTICLE_H_INCLUDED
#define PARTICLE_H_INCLUDED
#include "glad/glad.h"
#include "../Engine/Texture.h"
#include "../Engine/BufferObject.h"
#include "../Engine/VertexArray.h"
#include "../Engine/ProgramPipeline.h"
#include "../Engine/Sprite.h"
#include "../Engine/VecMath.h"
#include <vector>
#include <memory>
#include <random>

//��s�錾
class Particle;
class ParticleEmitter;
class ParticleManager;

using ParticleEmitterPtr = std::shared_ptr<ParticleEmitter>;
using ParticleManagerPtr = std::shared_ptr<ParticleManager>;


//�l�̕�ԕ��@
enum class InterporationType
{
	Linear,		//���`���
	WaseIn,		//���X�ɉ���
	EaseOut,	//���X�Ɍ���
	EaseInOut,	//���X�ɉ������A���X�Ɍ�������
	None,		//��Ԃ��Ȃ�
};

//�l���Ԃ���@�\�����N���X
template<typename T>
class Interporator
{
public:
	Interporator() = default;	//�R���X�g���N�^
	~Interporator() = default;	//�f�X�g���N�^

	explicit Interporator(const T& start) : start(start), end(start) {}
	Interporator(const T& start, const T& end) : start(start), end(end) {}

	//�l��ݒ肷��
	void Set(const T& start, const T& end)
	{
		this->start = start;
		this->end = end;
	}

	//����t(0.0~1.0)�ŕ�Ԃ����l��Ԃ�(��Œǉ��\��)
	T Interporate(float t)const
	{
		switch (type)
		{
		default:
		case InterporationType::Linear: return start + t * (end - start);
		case InterporationType::None:  return start;
		}
	}

	T start;	//��ԊJ�n���̒l
	T end;		//��ԏI�����̒l

	InterporationType type = InterporationType::Linear;//��ԕ��@

};

using Interporator1f = Interporator<float>;
using Interporator2f = Interporator<VecMath::vec2>;
using Interporator3f = Interporator<VecMath::vec3>;
using Interporator4f = Interporator<VecMath::vec4>;


//�p�[�e�B�N���̓�����`����p�����[�^
struct ParticleParameter
{
	float LifeTime = 1;//��������
	Texcoord tc = { 0,0,1,1 };//�e�N�X�`�����W
	Interporator1f radial = Interporator1f(10);				//���a�����̑��x(�G�~�b�^�[�̒��S������ˏ�ɕ��o�������ꍇ�ɐݒ�)
	Interporator3f velocity = Interporator3f({ 0,0,0 });	//���x(����̌����ɕ��o�������ꍇ�ɐݒ�)
	Interporator2f scale = Interporator2f({ 1,1 });			//�傫��
	Interporator1f rotation = Interporator1f(0);			//��]
	Interporator4f color = Interporator4f({ 1,1,1,1 });		//�F�ƕs�����x
};

//�G�~�b�^�[�̌`��
enum class ParticleEmitterShape
{
	sphere,	//��
	box,	//������
};

//�p�[�e�B�N���E�G�~�b�^�[�̃p�����[�^�[
struct ParticleEmitterParameter
{

	//�R���X�g���N�^
	ParticleEmitterParameter() : sphere({ 1,1 }){}

	//�f�X�g���N�^
	~ParticleEmitterParameter() = default;

	//���o�͈͂ɋ����w�肷��
	void SetSphereShape(float radius, float thickness)
	{
		Shape = ParticleEmitterShape::sphere;
		sphere.Radius = radius;
		sphere.Thickness = thickness;
	}

	//���o�͈͂ɒ����̂��w�肷��
	void SetBoxShape(const VecMath::vec3& size)
	{
		Shape = ParticleEmitterShape::box;
		box.size = size;
	}

	
	std::string name;		//�G�~�b�^�[��
	float Gravity = -9.8f;	//�d��

	//�摜�ƃu�����h�W��
	std::string ImagePath;						//�e�N�X�`���E�t�@�C����
	VecMath::vec2 tiles = { 1,1 };				//�e�N�X�`���̏c�Ɖ��̕�����
	GLenum srcFactor = GL_SRC_ALPHA;			//�u�����h�W��
	GLenum dstFactor = GL_ONE_MINUS_SRC_ALPHA;	//�u�����h�W��

	//���o���Ԃƕ��o�p�x
	float Duration = 1.0f;			//�p�[�e�B�N������o����b��
	bool Loop = false;				//Duration�o�ߌ�̓���(true=���[�v�@false=��~)
	float EmissionsPerSecond = 10;	//�b������̃p�[�e�B�N�����o��

	//���o�̌`��
	VecMath::vec3 position = { 0,0,0 };			//���W
	VecMath::vec3 rotation = { 0,0,0 };			//��]

	ParticleEmitterShape Shape = ParticleEmitterShape::sphere;	//�`��
	float RandomizeDirection = 0;		//���o�����̗�����(0=�Ȃ��@1=���S�܂ł�������)
	float RandomizeRotation = 0;		//��]�p�x�̗�����(0=�Ȃ��@1=���S�܂ł�������)
	float RandomizeSize = 0;		    //�g�k�̗�����(0=�Ȃ��@1=���S�܂ł�������)

	union 
	{
		struct Sphere
		{
			float Radius = 1;//���̔��a
			float Thickness = 1;//���k�̌���(0=�Ŕ��@1=���S�܂ł�������)
		}sphere;

		struct Box
		{
			VecMath::vec3 size = { 1,1,1 };//�����̂̃T�C�Y
		};
		Box box = { VecMath::vec3(1) };
	};
}; 

//�p�[�e�B�N��
class Particle
{
public:
	friend ParticleManager;
	friend ParticleEmitter;

	Particle() = default;	//�R���X�g���N�^
	~Particle() = default;	//�f�X�g���N�^

	//�������Ԃ̔䗦
	float GetLifeTimeRatio()const
	{
		const float t = 1 - LifeTime / pp->LifeTime;

		//0~1�̒l�Ɏ��߂�
		return std::clamp(t, 0.0f, 1.0f);
	}

	//true=���S�@false=����
	bool IsDead()const 
	{
		return LifeTime <= 0;
	}


private:
	//�ω����Ȃ��p�����[�^
	const ParticleParameter* pp = 0;	//��{�p�����[�^
	VecMath::vec3 RadialDirection = { 1,0,0 };	//���S����̌���
	Interporator3f velocity = Interporator3f({ 0,0,0 });	//���x
	Interporator1f rotation = Interporator1f(0, 0);			//��]
	Interporator2f scale = Interporator2f({ 1,1 });			//�傫��

	Texcoord tc = { 0,0,1,1 };	//�e�N�X�`�����W

	//�ω�����p�����[�^
	float LifeTime = 0;		//��������
	VecMath::vec3 position = { 0,0,0 };	//���W
	float GravityAccel = 0;	//�d�͂ɂ������x�̍��v
	float GravitySpeed = 0;	//�d�͂ɂ�鑬�x�̍��v
	float ViewDepth = 0;	//���_����̐[�x
};

//�p�[�e�B�N�����o�N���X
class ParticleEmitter
{
public:
	friend ParticleManager;

	//�R���X�g���N�^
	ParticleEmitter(const ParticleEmitterParameter& ep,
		const ParticleParameter& pp, ParticleManager* pManager);

	//�f�X�g���N�^
	~ParticleEmitter() = default;

	//�R�s�[�Ƒ���̋֎~
	ParticleEmitter(const ParticleEmitter&) = delete;
	ParticleEmitter& operator=(const ParticleEmitter&) = delete;

	//�X�V
	void Update(const VecMath::mat4& matView, float deltaTime);

	//�p�[�e�B�N��������
	void Die() { ep.Loop = false; ep.Duration = 0; }

	//��������
	bool IsDead()const { return !ep.Loop && Timer >= ep.Duration && Particles.empty(); }

	//�p�����[�^�[�̎擾�E�ݒ�
	ParticleEmitterParameter& GetEmitterParameter() { return ep; }
	const ParticleEmitterParameter& GetEmitterParameter()const { return ep; }

	ParticleParameter& GetParticleParameter() { return pp; }
	const ParticleParameter& GetParticleParameter()const  { return pp; }


private:
	void AddParticle();

	ParticleParameter pp;				//�p�[�e�B�N���̃p�����[�^�[
	ParticleEmitterParameter ep;		//�G�~�b�^�[�̃p�����[�^�[

	//shared�ɂ���ƁA�z�Q�ƂɂȂ��Ă��܂�����
	ParticleManager* pManager = nullptr;//�Ǘ��N���X�̃A�h���X

	TexturePtr texture;			//�p�[�e�B�N���p�̃e�N�X�`��
	float Interval = 0;			//�p�[�e�B�N���̔����Ԋu(�b)
	float Timer = 0;			//�o�ߎ���(�b)
	float EmissionTimer = 0;	//�p�[�e�B�N�������^�C�}�[(�b)
	float ViewDepth = 0;		//���_����̐[�x�l

	std::vector<Particle*> Particles;	//�p�[�e�B�N�����X�g
	GLsizei DataCount = 0;				//�`�悷��p�[�e�B�N����
	size_t DataOffset = 0;				//�`��̊�ƂȂ钸�_�̃I�t�Z�b�g

};

//�p�[�e�B�N���Ǘ��N���X
class ParticleManager
{
public:
	static ParticleManagerPtr Create(size_t maxParticleCount);

	//�R���X�g���N�^
	explicit ParticleManager(size_t maxParticleCount);

	//�f�X�g���N�^
	~ParticleManager();

	//�R�s�[����̋֎~
	ParticleManager(const ParticleManager&) = delete;
	ParticleManager& operator=(const ParticleManager&) = delete;

	void Update(const VecMath::mat4& matView, float deltatime);
	void UpdateSSBO();
	void Draw(const VecMath::mat4& matProj, const VecMath::mat4& matView, const VecMath::mat3& matBillboard);

	//�G�~�b�^�[�̊Ǘ�
	ParticleEmitterPtr AddEmitter(
		const ParticleEmitterParameter& ep, const ParticleParameter& pp);
	
	ParticleEmitterPtr FindEmitter(const std::string& name)const;	//�G�~�b�^�[�̌���
	void RemoveEmitter(const ParticleEmitterPtr&);					//�G�~�b�^�[�̍폜
	void RemoveEmitterAll();	//�G�~�b�^�[�̑S�폜

	//�p�[�e�B�N���̊Ǘ�
	Particle* AllocateParticle();
	void DeallocateParticle(Particle* p);

	//�����̐���
	int RandomInt(int min, int max)
	{
		return std::uniform_int_distribution<>(min, max)(RandomEngine);
	}

	float RandomFloat(float min, float max)
	{
		return std::uniform_real_distribution<float>(min, max)(RandomEngine);
	}


private:
	std::vector<ParticleEmitterPtr> Emitters;	//�G�~�b�^�[�̃��X�g
	std::vector<Particle> Particles;			//�p�[�e�B�N���̃��X�g
	std::vector<Particle*> FreeParticles;		//���g�p�p�[�e�B�N���̃��X�g

	size_t MaxParticleCount = 0; //�ő�p�[�e�B�N����
	BufferObjectPtr vbo;
	BufferObjectPtr ibo;
	VertexArrayPtr vao;

	ProgramPipelinePtr program;
	MappedBufferObjectPtr ssbo;
	std::mt19937 RandomEngine;
};

#endif//PARTICLE_H_INCLUDED