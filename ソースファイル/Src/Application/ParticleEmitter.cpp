//@file Particle.c

#include "Particle.h"
#include "../Engine/Debug.h"
#include <algorithm>

using namespace VecMath;

//���_�f�[�^�\����
struct ParticleVertex
{
	float x, y;	//���W(x,y)
	float u, v;	//�e�N�X�`�����W
};

//�摜�\���p�f�[�^�����i�[����\����(float*16��=64�o�C�g)
struct ParticleData
{
	vec4 Position;		//���W(w�͖��g�p)
	vec2 Scale;			//�g��k��
	float Cos;			//z����]��Cos
	float Sin;			//z����]��Sin
	vec4 Color;			//�F�Ɠ����x
	Texcoord texcoord;	//�e�N�X�`�����W
};

//�R���X�g���N�^
//@param ep�@�@�@�@�G�~�b�^�[�̏������p�����[�^
//@param pp�@�@�@�@�p�[�e�B�N���̏������p�����[�^
//@param pManager �Ǘ��I�u�W�F�N�g�̃A�h���X

ParticleEmitter::ParticleEmitter(const ParticleEmitterParameter& ep,
	const ParticleParameter& pp, ParticleManager* pManager)
	:ep(ep), pp(pp), pManager(pManager), Interval(1.0f / ep.EmissionsPerSecond)
{
	//�e�N�X�`���̍쐬
	texture = Texture::Create(ep.ImagePath.c_str(), GL_LINEAR,
		Texture::Usage::for2D);
}

//�G�~�b�^�[�̊Ǘ����ɂ���p�[�e�B�N���̏�Ԃ��X�V����
//@param matView   �`��Ɏg�p����r���[�s��
//@param deltaTime�@�O��̍X�V����̌o�ߎ��ԁi�b�j

void ParticleEmitter::Update(const mat4& matView, float deltaTime)
{
	//�Ǘ��I�u�W�F�N�g�����ݒ�Ȃ牽�����Ȃ�
    if (!pManager)
	{
		return;
	}

	//���Ԃ��o�߂�����
	Timer += deltaTime;

	//���o���Ԃ��߂�����
	if (Timer >= ep.Duration)
	{
		//���[�v��������
		if (ep.Loop)
		{
			//���Ԃ����Z�b�g
			Timer -= ep.Duration;
			EmissionTimer -= ep.Duration;
		}
		else
		{
			//�I��
			Timer = ep.Duration;
		}
	}

	//�\�[�g�Ɏg���u���_����̐[�x�n�v���v�Z
	ViewDepth = (matView * vec4(ep.position, 1)).z;

	//�p�[�e�B�N���̕��o
	for (; Timer - EmissionTimer >= Interval; EmissionTimer += Interval)
	{
		//�p�[�e�B�N�����쐬
		AddParticle();
	}

	//�p�[�e�B�N���̍X�V
	for (auto e : Particles)
	{
		//����ł����玟�̐l��
		if (e->LifeTime <= 0)
		{
			continue;
		}

		//�������Ԃ��o�߂�����
		e->LifeTime -= deltaTime;

		//���x���v�Z
		const float t = e->GetLifeTimeRatio();		//�������Ԃ̊���
		vec3 velocity = e->velocity.Interporate(t);	//�C�[�W���O�œ���

		//���a�����̑��x���v�Z
		velocity += e->RadialDirection * e->pp->radial.Interporate(t);

		//�d�͂ɂ������x�Ƒ��x���X�V
		e->GravityAccel += ep.Gravity * deltaTime;		//�����x
		e->GravitySpeed += e->GravityAccel * deltaTime;	//���x
		velocity.y += e->GravitySpeed;

		//���W���X�V
		e->position += velocity * deltaTime;

		//�\�[�g�Ɏg���u���_����̐[�x�n�v���v�Z
		e->ViewDepth = (matView * vec4(e->position, 1)).z;
	}

	//����ł���p�[�e�B�N�����폜
	//�擪�ɐ����Ă���l���W�߂�
	auto itr = std::partition(Particles.begin(), Particles.end(),
		[](const Particle* p) {return !p->IsDead(); });

	for (auto i = itr; i != Particles.end(); ++i)
	{
		//�p�[�e�B�N����ԋp
		pManager->DeallocateParticle(*i);
	}
	Particles.erase(itr, Particles.end());

	//�p�[�e�B�N�����\�[�g
	std::sort(Particles.begin(), Particles.end(),
		[](const Particle* a, const Particle* b) {return a->ViewDepth< b->ViewDepth; });
}

//�p�[�e�B�N����ǉ�
void ParticleEmitter::AddParticle()
{
	//�p�[�e�B�N�����m�ۂł��Ȃ���Ή������Ȃ�
	Particle* particle = pManager->AllocateParticle();

	if (!particle)
	{
		return;
	}

	//���[�J�����o���W���v�Z
	vec4 pos = { 0,0,0,1 };

	switch (ep.Shape)
	{
	case ParticleEmitterShape::sphere:
	{
		//cos�@sin�@�΂����߂�
		const float cosTheta = pManager->RandomFloat(-1, 1);
		const float sinTheta = std::sqrt(1 - cosTheta * cosTheta);
		const float phi = pManager->RandomFloat(0, 2 * pi);

		//���̑̐ς̌���4��r^3/3����A���a���Ƃ̕��o�p�x�͔��a��3�捪�ŋ��߂���
		//thickness��3�悷�邱�Ƃ�3�捪����������ŏ��l��Thick�ƈ�v����
		const float t = ep.sphere.Thickness;
		const float r0 = pManager->RandomFloat(1 - t * t * t, 1);
		const float r = std::cbrt(r0) * ep.sphere.Radius;//���a��3��

		pos.x = r * sinTheta * std::cos(phi);
		pos.y = r * sinTheta * std::sin(phi);
		pos.z = r * cosTheta;

		break;
	}
	case ParticleEmitterShape::box:
	{
		pos.x = pManager->RandomFloat(-ep.box.size.x, ep.box.size.x);
		pos.y = pManager->RandomFloat(-ep.box.size.y, ep.box.size.y);
		pos.z = pManager->RandomFloat(-ep.box.size.z, ep.box.size.z);

		break;
	}
	}

	//Z->X->Y�̏��ŉ�]��K�p���A���o�����֌�����s����v�Z
	mat4 matRot = mat4::RotateY(ep.rotation.y) *
		mat4::RotateX(ep.rotation.x) * mat4::RotateZ(ep.rotation.z);

	//�p�[�e�B�N���̃����o�ϐ���������
	particle->pp = &pp;
	particle->LifeTime = pp.LifeTime;
	particle->position = ep.position + vec3(matRot * pos);
	particle->RadialDirection = normalize(particle->position - ep.position);
	particle->GravityAccel = 0;
	particle->GravitySpeed = 0;
	particle->ViewDepth = 0;

	//�����������_���W�����O�łȂ���΁A�ړ������Ƀ����_����������
	if (ep.RandomizeDirection)
	{
		//�ړ������������_��������s����v�Z
		const float RandmRamge = pi * ep.RandomizeDirection;

		const mat4 matRandom =
			mat4::RotateY(pManager->RandomFloat(-RandmRamge, RandmRamge)) *
			mat4::RotateX(pManager->RandomFloat(-RandmRamge, RandmRamge)) *
			mat4::RotateZ(pManager->RandomFloat(-RandmRamge, RandmRamge));

		//��]�ƃ����_��������
		matRot *= matRandom;

		//���a�����������_����
		particle->RadialDirection = vec3(matRandom * vec4(particle->RadialDirection, 1));
	}

	//���x�x�N�g������]
	particle->velocity.start = vec3(matRot * vec4(pp.velocity.start, 1));
	particle->velocity.end = vec3(matRot * vec4(pp.velocity.end, 1));

	//��]�p�x�̃����_���W�����O�łȂ���΁A��]�p�x�Ƀ����_����������
	particle->rotation = pp.rotation;

	if (ep.RandomizeRotation)
	{
		const float randomRange = pi * ep.RandomizeRotation;
		const float a = pManager->RandomFloat(-randomRange, randomRange);
		particle->rotation.start += a;
		particle->rotation.end += a;
	}

	//�g�k�̃����_���W�����O�łȂ���΁A�傫���Ƀ����_����������
	particle->scale = pp.scale;

	if (ep.RandomizeSize)
	{
		const float randomSize = pi * ep.RandomizeSize;
		const float a = pManager->RandomFloat(-randomSize, randomSize);
		particle->scale.start += a;
		particle->scale.end += a;
	}


	//�^�C�������P��葽���ꍇ�A�����_���Ƀ^�C����I������
	if (ep.tiles.x > 1 || ep.tiles.y > 1)
	{
		const vec2 invTiles = 1.0f / ep.tiles;

		const int u = pManager->RandomInt(0, static_cast<int>(ep.tiles.x) - 1);
		const int v = pManager->RandomInt(0, static_cast<int>(ep.tiles.y) - 1);
	
		particle->tc.u = static_cast<float>(u) * invTiles.x;
		particle->tc.v = static_cast<float>(v) * invTiles.y;
		particle->tc.sx = invTiles.x;
		particle->tc.sy = invTiles.y;
	}

	//�G�~�b�^�[�̊Ǘ����X�g�Ƀp�[�e�B�N����ǉ�
	Particles.push_back(particle);
}


//�p�[�e�B�N���Ǘ��I�u�W�F�N�g���쐬����
//@param maxParticleCount �\���\�ȃp�[�e�B�N���̍ő吔
ParticleManagerPtr ParticleManager::Create(size_t maxParticleCount)
{
	return std::make_shared<ParticleManager>(maxParticleCount);
}

//�R���X�g���N�^
//@param maxParticleCount
ParticleManager::ParticleManager(size_t maxParticleCount)
	:MaxParticleCount(maxParticleCount)
{
	const ParticleVertex vertexData[] = {
		{-0.5f, -0.5f, 0, 1},
		{ 0.5f, -0.5f, 1, 1},
		{-0.5f,  0.5f, 1, 0},
		{-0.5f,  0.5f, 0, 0},
	};

	//vbo�̍쐬
	vbo = BufferObject::Create(sizeof(vertexData), vertexData);

	//ibo�̍쐬
	const GLushort indexData[] = { 0,1,2,2,3,0 };
	ibo = BufferObject::Create(sizeof(indexData), indexData);

	//vbo�̍쐬
	vao = VertexArray::Create();

	glBindVertexArray(*vao);
	glBindBuffer(GL_ARRAY_BUFFER, *vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ibo);

	//vert�ɕϐ���n��
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), 0);

	//vert�ɕϐ���n��
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex),
		(const void*)offsetof(ParticleVertex, u));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	program = ProgramPipeline::Create("Res/particle.vert", "Res/particle.frag");
	ssbo = MappedBufferObject::Create(maxParticleCount * sizeof(ParticleData),
		GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);

	//�K�v�ȃ������̈���m��
	Emitters.reserve(100);
	Particles.resize(maxParticleCount);

	//���g�p�p�[�e�B�N�����X�g���쐬
	FreeParticles.resize(maxParticleCount);
	auto itr = Particles.end();

	for (auto& e : FreeParticles)
	{
		--itr;
		e = &*itr;
	}

	//������������
	std::random_device rd;
	RandomEngine.seed(rd());
}

//�f�X�g���N�^
ParticleManager::~ParticleManager()
{
	//���ׂẴG�~�b�^�[����}�l�[�W���̃A�h���X����������
	for (auto& e : Emitters)
	{
		e->pManager = nullptr;
	}
}

//�p�[�e�B�N���̏�Ԃ��X�V����
//@param   matView  �`��Ɏg�p����r���[�s�� 
//@param deltaTime  �O��̍X�V����̌o�ߎ���(�b)
void ParticleManager::Update(const mat4& matView, float deltaTime)
{
	if (Emitters.empty())
	{
		return;
	}

	//�G�~�b�^�[�̍X�V
	for (auto& e : Emitters)
	{
		e->Update(matView, deltaTime);
	}

	//�������牺�͑�̃p�[�e�B�N���ƈꏏ

	//����ł���G�~�b�^�[���폜�@
	auto itr = std::stable_partition(Emitters.begin(), Emitters.end(),
		[](const ParticleEmitterPtr& e) {return !e->IsDead(); });

	for (auto i = itr; i != Emitters.end(); ++i)
	{
		(*i)->pManager = nullptr;
	}
	Emitters.erase(itr, Emitters.end());

	//�G�~�b�^�[���\�[�g
	std::stable_sort(Emitters.begin(), Emitters.end(),
		[](const ParticleEmitterPtr& a, const ParticleEmitterPtr& b)
		{return a->ViewDepth < b->ViewDepth; });
}

//SSBO�̓��e���X�V����
void ParticleManager::UpdateSSBO()
{
	ssbo->WaitSync();

	//���_���W�n�ŉ��ɂ���G�~�b�^�[����f�[�^��ݒ�
	auto* begin = reinterpret_cast<ParticleData*>(ssbo->GetMappedAddress());
	auto* pData = begin;

	for (auto& e : Emitters)
	{
		//�G�~�b�^�[�̕`��p�̃f�[�^�ϐ����X�V
		e->DataOffset = (pData - begin) * sizeof(ParticleData);		//�f�[�^�̈ʒu
		e->DataCount = static_cast<GLsizei>(e->Particles.size());	//�f�[�^�̐�
	
		//SSBO�Ƀf�[�^��ݒ�
		for (const auto particle : e->Particles)
		{
			const float t = particle->GetLifeTimeRatio();
			pData->Position = vec4(particle->position, 1);
			pData->Scale = particle->pp->scale.Interporate(t);

			const float rotation = particle->rotation.Interporate(t);
			pData->Cos = std::cos(rotation);
			pData->Sin = std::sin(rotation);
			pData->Color = particle->pp->color.Interporate(t);
			pData->texcoord = particle->tc;

			++pData;
		}

		//���̃I�t�Z�b�g��256�o�C�g���E�ɂȂ�悤�ɒ���
		pData += e->DataCount % 4;//64�o�C�g�Ȃ̂łS��256�o�C�g
	}
}

//�p�[�e�B�N����`�悷��
//@param matProj �`��Ɏg�p����v���W�F�N�V�����s��
//@param matView �`��Ɏg�p����r���[�s��
void ParticleManager::Draw(const mat4& matProj, const mat4& matView, const VecMath::mat3& matBillboard)
{
	//��Ȃ疳��
	if (Emitters.empty())
	{
		return;
	}

	//�[�x�e�X�g�L���A�[�x�o�b�t�@�ւ̏������݂͍s��Ȃ��悤�ɐݒ�
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	glBindVertexArray(*vao);
	glUseProgram(*program);

	//�V�F�[�_�[�Ƀr���[�v���W�F�N�V�����s���ݒ�
	const GLint locMatVP = 1;
	const mat4 matVP = matProj * matView;

	glProgramUniformMatrix4fv(*program,
		locMatVP, 1, GL_FALSE, &matVP[0][0]);

	//�V�F�[�_�[�Ƀr���{�[�h�b���ݒ�(��ɃJ�����̕����������v���~�e�B�u�̂���)
	const GLint locMatBilboard = 2;
	//const mat3 matBillboard2 = inverse(transpose(inverse(mat3(matView))));

	glProgramUniformMatrix3fv(*program,
		locMatBilboard, 1, GL_FALSE, &matBillboard[0][0]);

	//�p�[�e�B�N����`��
	for(auto & e: Emitters)
	{
		//�`������Ȃ��Ȃ疳��
		if (e->DataCount <= 0)
		{
			continue;
		}

		glBlendFunc(e->ep.srcFactor, e->ep.dstFactor);
		GLuint tex = *e->texture;
		glBindTextures(0, 1, &tex);
		ssbo->Bind(0, e->DataOffset, e->DataCount * sizeof(ParticleData));
		glDrawElementsInstanced(
			GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0, e->DataCount);
	}
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, 0, 0, 0);
	ssbo->SwapBuffers();

	glBindTextures(0, 1, nullptr);
	glUseProgram(0);
	glBindVertexArray(0);

	//�u�����h�ݒ�A�[�x�o�b�t�@�ւ̏������݂��f�t�H���g�ɖ߂�
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_TRUE);	//�[�x�o�b�t�@�ւ̏������݂�����
}

//�G�~�b�^�[��ǉ�����
//@param ep �G�~�b�^�[�̏������p�����[�^
//@param pp �p�[�e�B�N���̏������p�����[�^

//@return �ǉ������G�~�b�^�[
ParticleEmitterPtr ParticleManager::AddEmitter(const ParticleEmitterParameter& ep,
	const ParticleParameter& pp)
{
	//�|�C���^�[���쐬���A�G�~�b�^�[�ɒǉ�����
	ParticleEmitterPtr p = std::make_shared<ParticleEmitter>(ep, pp, this);
	Emitters.push_back(p);

	return p;
}

//�w�肳�ꂽ���O�����G�~�b�^�[����������
//@param name �������閼�O

//@return name�ƈ�v���閼�O�����G�~�b�^�[�ւ̃|�C���^
ParticleEmitterPtr ParticleManager::FindEmitter(const std::string& name)const
{
	//�G�~�b�^�[�̌���
	auto itr = std::find_if(Emitters.begin(), Emitters.end(),
		[name](const ParticleEmitterPtr& p)
		{return p->ep.name == name; });

	if (itr != Emitters.end())
	{
		return *itr;
	}

	return nullptr;
}

//�w�肳�ꂽ�G�~�b�^�[���폜����
void ParticleManager::RemoveEmitter(const ParticleEmitterPtr& p)
{
	auto itr = std::find(Emitters.begin(), Emitters.end(), p);

	if (itr != Emitters.end())
	{
		(*itr)->pManager = nullptr;			//�Ǘ��I�u�W�F�N�g�ւ̃|�C���^���폜
		std::swap(*itr, Emitters.back());	//���Ɠ���ւ���
		Emitters.pop_back();				//��������
	}
}

//���ׂẴG�~�b�^�[���폜����
void ParticleManager::RemoveEmitterAll()
{
	for (auto& e : Emitters)
	{
		e->pManager = nullptr;//�Ǘ��I�u�W�F�N�g�ւ̃|�C���^���폜
	}

	//�S�������h��
	Emitters.clear();
}

//�p�[�e�B�N�����m�ۂ���
//@retval nullptr      �p�[�e�B�N���̊m�ۂɎ��s
//@retval nullptr�ȊO  �m�ۂ����p�[�e�B�N���̃A�h���X

Particle* ParticleManager::AllocateParticle()
{
	//����ۂȂ�
	if (FreeParticles.empty())
	{
		return nullptr;
	}

	auto p = FreeParticles.back();	//������
	FreeParticles.pop_back();		//��������
	return p;
}

//�p�[�e�B�N�����������
//@param p �������p�[�e�B�N���̃A�h���X
void ParticleManager::DeallocateParticle(Particle* p)
{
	//�����͈͊O�Ȃ�G���[
	if (p<&Particles.front() || p>&Particles.back())
	{
		LOG_ERROR("�Ǘ��͈͊O�̃A�h���X�ł�(p=%x,�͈�%x-%x)",
			p, &Particles.front(), &Particles.back());

		return;
	}

	FreeParticles.push_back(p);
}