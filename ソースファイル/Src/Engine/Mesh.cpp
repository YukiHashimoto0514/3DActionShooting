//@file Mesh.cpp

#define _CRT_SECURE_NO_WARNINGS

#include "Mesh.h"
#include "VertexArray.h"
#include "Debug.h"
#include "../MikkTSpace/mikktspace.h"
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <stdio.h>

using namespace VecMath;

namespace Mesh
{

	//MikkTSpace�p�̃��[�U�[�f�[�^
	struct UserData
	{
		std::vector<Mesh::Vertex>& vertices;
		std::vector<uint16_t>& indices;
	};

	//MikkTSpace�̃C���^�[�t�F�C�X����
	class MikkTSpace
	{
	public:
		//�R���X�g���N�^
		MikkTSpace()
		{
			//���f���̑��|���S������Ԃ�
			interface.m_getNumFaces = [](const SMikkTSpaceContext* pContext)
			{
				UserData* p = static_cast<UserData*>(pContext->m_pUserData);
				return static_cast<int>(p->indices.size() / 3);
			};

			//�|���S���̒��_����Ԃ�
			interface.m_getNumVerticesOfFace = [](const SMikkTSpaceContext* pContext,
				int iFace)
			{
				//�O�p�`�Ȃ̂łR��Ԃ�
				return 3;
			};

			//���_�̍��W��Ԃ�
			interface.m_getPosition = [](const SMikkTSpaceContext* pContext,
				float fvPosOut[], int iFace, int iVert)
			{
				UserData* p = static_cast<UserData*>(pContext->m_pUserData);

				//iFace�Ԗڂ̃|���S����Ivert�Ԗڂ̒��_�����Ԃɑ��
				const int index = p->indices[iFace * 3 + iVert];
				fvPosOut[0] = p->vertices[index].position.x;
				fvPosOut[1] = p->vertices[index].position.y;
				fvPosOut[2] = p->vertices[index].position.z;
			};

			//���_�̖@����Ԃ�
			interface.m_getNormal = [](const SMikkTSpaceContext* pContext,
				float fvNormaOut[], int iFace, int iVert)
			{
				UserData* p = static_cast<UserData*>(pContext->m_pUserData);

				const int index = p->indices[iFace * 3 + iVert];
				fvNormaOut[0] = p->vertices[index].normal.x;
				fvNormaOut[1] = p->vertices[index].normal.y;
				fvNormaOut[2] = p->vertices[index].normal.z;
			};

			//���_�̃e�N�X�`�����W��Ԃ�
			interface.m_getTexCoord = [](const SMikkTSpaceContext* pContext,
				float fvTexcOut[], int iFace, int iVert)
			{
				UserData* p = static_cast<UserData*>(pContext->m_pUserData);

				const int index = p->indices[iFace * 3 + iVert];
				fvTexcOut[0] = p->vertices[index].normal.x;
				fvTexcOut[1] = p->vertices[index].normal.y;
			};

			//�^���W�F���g�x�N�g���ƃo�C�^���W�F���g�x�N�g���̌������󂯎��
			interface.m_setTSpaceBasic = [](const SMikkTSpaceContext* pContext,
				const float fvTangent[], float fSign,int iFace, int iVert)//�x�N�g���A�����A�|���S���ԍ��A���_�ԍ�
			{
				UserData* p = static_cast<UserData*>(pContext->m_pUserData);

				const int index = p->indices[iFace * 3 + iVert];
				p->vertices[index].tangent =
					vec4(fvTangent[0], fvTangent[1], fvTangent[2], fSign);
			};

			//�g��Ȃ��̂�nullptr��ݒ�
			interface.m_setTSpace = nullptr;

		}

		//�f�X�g���N�^
		~MikkTSpace() = default;

		//�����o�ϐ�
		SMikkTSpaceInterface interface;
	};

	//primitive��`�悷��
	void Draw(const Primitive& prim)
	{
		glDrawElementsInstancedBaseVertex(prim.mode, prim.count,	//��{�}�`�̎�ށA�f�[�^��
			GL_UNSIGNED_SHORT, prim.indices, 1, prim.baseVertex);	//�C���f�b�N�X�f�[�^�̌^�A�f�[�^�̊J�n�ʒu�A�`��̂���}�`�̐��A���_�f�[�^�̊J�n�ʒu
	}

	//���b�V����`�悷��
	void Draw(const StaticMesh& mesh,const std::vector<MaterialPtr>& materials ,GLuint program)
	{
		for (const auto& prim : mesh.primitives)
		{
			//�}�e���A����ݒ�
			if (prim.materialNo >= 0 && prim.materialNo < materials.size())
			{
				const Material& material = *materials[prim.materialNo];

				if (program)
				{
					glProgramUniform4fv(program, 102, 1, &material.baseColor.x);
					glProgramUniform3f(program, 103,
						material.specularPower, material.normalizeFactor, material.receiveShadows);
				}

				//GL�R���e�L�X�g�Ƀe�N�X�`�������蓖�Ă�
				GLuint tex[2] = { 0 };
				if (material.texBaseColor)
				{
					tex[0] = *material.texBaseColor;
				}

				if (material.texNormal)
				{
					tex[1] = *material.texNormal;
				}
				glBindTextures(0, 2, tex);
			}

			Draw(prim);
		}
	}

	//MTL�t�@�C����ǂݍ��ށi�}�e���A���e���v���[�g���C�u�����j
	//@param foldername OBJ�t�@�C���̂���t�H���_��
	//@param filename   MTL�t�@�C����

	//return MTL�t�@�C���Ɋ܂܂��}�e���A���̔z��

	std::vector<MaterialPtr> MeshBuffer::LoadMTL(
		const std::string& foldername, const char* filename)
	{
		//MTL�t�@�C�����J��
		const std::string fullpath = foldername + filename;
		std::ifstream file(fullpath);

		if (!file)
		{
			LOG_ERROR("%s���J���܂���", fullpath.c_str());
			return{};
		}

		//MTL�t�@�C������͂���
		std::vector<MaterialPtr> materials;
		MaterialPtr pMaterial;

		while (!file.eof())
		{
			std::string line;
			std::getline(file, line);

			//�}�e���A����`�̓ǂݎ������݂�
			char name[1000] = { 0 };

			if (sscanf(line.data(), " newmtl %999s", name) == 1)
			{
				pMaterial = std::make_shared<Material>();
				pMaterial->name = name;
				pMaterial->texBaseColor = defaultMaterial->texBaseColor;
				materials.push_back(pMaterial);

				continue;
			}


			//�}�e���A������`����Ă��Ȃ��ꍇ�͍s�𖳎�����
			if (!pMaterial)
			{
				continue;
			}

			//��{�F�̓ǂݎ������݂�
			if (sscanf(line.data(), " Kd %f %f %f",
				&pMaterial->baseColor.x, &pMaterial->baseColor.y,
				&pMaterial->baseColor.z) == 3)
			{
				continue;
			}

			//�s�����x�̓ǂݎ������݂�
			if (sscanf(line.data(), " d %f", &pMaterial->baseColor.w) == 1)
			{
				continue;
			}

			//���ʔ��ˎw���̓ǂݎ������݂�
			if (sscanf(line.data(), " Ns %f", &pMaterial->specularPower) == 1)
			{
				//���K���W�������߂�
				pMaterial->normalizeFactor =
					(pMaterial->specularPower + 8) / (8 * pi);
				
				continue;
			}

			//��{�F�e�N�X�`�����̓ǂݎ������݂�
			char textureName[1000] = { 0 };

			if (sscanf(line.data(), " map_Kd %999[^\n]", &textureName) == 1)
			{
				pMaterial->texBaseColor =
					Texture::Create(foldername + textureName, GL_LINEAR,
						Texture::Usage::for3DLinear);

				continue;
			}

			//�@���e�N�X�`�����̓ǂݎ������݂�
			textureName[0] = 0;
			if (sscanf(line.data(), " bump %999[^\n]", &textureName) < 1)
			{
				sscanf(line.data(), "map_bump %999[^\n]", &textureName);
			}

			if (textureName[0] != 0)
			{
				pMaterial->texNormal =
					Texture::Create(foldername + textureName, GL_LINEAR,
						Texture::Usage::for3D);

				continue;
			}

		}
		//�ǂݍ��񂾃}�e���A���z���Ԃ�
		return materials;
	}

	//�R���X�g���N�^
	//@param vboCapacity�@VBO�̍ő�e�ʁi�o�C�g���j
	//@param iboCapacity�@IBO�̍ő�e�ʁi�o�C�g���j

	PrimitiveBuffer::PrimitiveBuffer(size_t vboCapacity, size_t iboCapacity) :
		vboCapacity(vboCapacity), iboCapacity(iboCapacity)
	{
		const GLenum flags = GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT;

		//VBO���쐬
		glCreateBuffers(1, &vbo);
		glNamedBufferStorage(vbo, vboCapacity, nullptr, flags);
		pVbo = static_cast<uint8_t*>(glMapNamedBufferRange(vbo, 0, vboCapacity, flags));

		//IBO���쐬
		glCreateBuffers(1, &ibo);
		glNamedBufferStorage(ibo, iboCapacity, nullptr, flags);
		pIbo = static_cast<uint8_t*>(glMapNamedBufferRange(ibo, 0, iboCapacity, flags));
	
		//VAO���쐬���AOpenGL�R���e�L�X�g�Ɋ��蓖�Ă�
		vao = VertexArray::Create();
		glBindVertexArray(*vao);

		//VAO�ɁAVBO��IBO�����蓖�Ă�
		//���_��A�g���r���[�g�ݒ�O�ɍs���K�v������
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

		//���_�A�g���r���[�g��ݒ�
		const auto setAttribute = [](GLuint index, GLint size, size_t offset)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, sizeof(Vertex),
				reinterpret_cast<void*>(offset));
		};

		//�V�F�[�_�\�ɏ��𑗂�
		setAttribute(0, 3, offsetof(Vertex, position));
		setAttribute(1, 2, offsetof(Vertex, texcoord));
		setAttribute(2, 3, offsetof(Vertex, normal));
		setAttribute(3, 4, offsetof(Vertex, tangent));

		//OpenGL�R���e�L�X�g�ւ̊��蓖�Ă�����
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	//�f�X�g���N�^
	PrimitiveBuffer::~PrimitiveBuffer()
	{
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ibo);
	}

	//�v���~�e�B�u��ǉ�����
	//@param vertices�@�@GPU�������ɃR�s�[���钸�_�f�[�^�z��
	//@param vertexCount vertices�̗v�f��
	//@param indices�@	 GPU�������ɃR�s�[����C���f�b�N�X�f�[�^�z��
	//@param indexCount�@indices�̗v�f��
	//@param mode�@		 �v���~�e�B�u�̎��

	void PrimitiveBuffer::AddPrimitive(
		const Vertex* vertices, size_t vertexCount,
		const uint16_t* indices, size_t indexCount, GLenum mode)
	{
		Primitive prim;
		prim.mode = mode;
		prim.count = static_cast<GLsizei>(indexCount);
		prim.indices = AddIndices(indices, indexCount);
		prim.baseVertex = AddVertices(vertices, vertexCount);
		primitives.push_back(prim);
	}


	//���_�f�[�^��GPU�������ɃR�s�[
	//@param vertices    GPU�������ɃR�s�[���钸�_�f�[�^�z��
	//@param vertexCount vertices�̗v�f��

	GLint PrimitiveBuffer::AddVertices(const Vertex* vertices, size_t count)
	{
		const GLint offset = static_cast<GLint>(vboSize / sizeof(Vertex));

		const size_t verticesSize = count * sizeof(Vertex);
		memcpy(pVbo + vboSize, vertices, verticesSize);
		vboSize += verticesSize;

		return offset;
	}

	//���_�C���f�b�N�X��GPU�������[�ɃR�s�[
	//@param indices    GPU�������ɃR�s�[����C���f�b�N�X�f�[�^�z��
	//@param indexCount indices�̗v�f��

	const void* PrimitiveBuffer::AddIndices(const uint16_t* indices, size_t count)
	{
		const void* offset = reinterpret_cast<void*>(iboSize);

		const size_t indexSize = count * sizeof(uint16_t);
		memcpy(pIbo + iboSize, indices, indexSize);
		iboSize += indexSize;

		return offset;
	}

	//���ׂẴv���~�e�B�u���폜����
	void PrimitiveBuffer::Clear()
	{
		primitives.clear();

		//�擪���R�s�[�ł���悤��
		vboSize = 0;
		iboSize = 0;
	}

	//�R���X�g���N�^
	//@param vboCapacity�@VBO�̍ő�e�ʁi�o�C�g���j
	//@param iboCapacity�@IBO�̍ő�e�ʁi�o�C�g���j
	MeshBuffer::MeshBuffer(size_t vboCapacity, size_t iboCapacity) :
		primitiveBuffer(vboCapacity, iboCapacity)
	{
		//�^���W�F���g��Ԍv�Z�p�̃I�u�W�F�N�g���쐬����
		mikkTSpace = std::make_shared<MikkTSpace>();
		if (!mikkTSpace)
		{
			LOG_ERROR("MikkTSpace�̍쐬�ɂ����ς�");
		}

		//�}�e���A�����ݒ莞�Ɏg�p����}�e���A�����쐬
		defaultMaterial = std::make_shared<Material>();
		defaultMaterial->texBaseColor = Texture::Create("defaultTexture", 4, 4,
			GL_SRGB8_ALPHA8,GL_LINEAR);
		
		std::vector<uint32_t> img(4 * 4, 0xff'cc'cc'cc);
		glTextureSubImage2D(*defaultMaterial->texBaseColor, 0, 0, 0,
			4, 4, GL_BGRA, GL_UNSIGNED_BYTE, img.data());
	}

	//OBJ�t�@�C����ǂݍ���
	//@param filename OBJ�t�@�C����
	//@return filename����쐬�������b�V��

	StaticMeshPtr MeshBuffer::LoadOBJ(const char* filename)
	{
		//�ȑO�ɓǂݍ��񂾃t�@�C���Ȃ�A�쐬�ς݂̃��b�V����Ԃ�
		{
			auto itr = meshes.find(filename);

			if (itr != meshes.end())
			{
				return itr->second;
			}
		}

		//OBJ�t�@�C�����J��
		std::ifstream file(filename);

		if (!file)
		{
			LOG_ERROR("%s���J���܂���", filename);
			return nullptr;
		}

		//�t�H���_�����擾����
		std::string foldername(filename);
		{
			const size_t p = foldername.find_last_of("\\/");

			if (p != std::string::npos)
			{
				foldername.resize(p + 1);
			}
		}

		//OBJ�t�@�C������͂��āA���_�f�[�^�ƃC���f�b�N�X�f�[�^��ǂݍ���
		struct IndexSet { int v, vt, vn; };		//�C���f�b�N�X�f�[�^�̑g

		struct UseMaterial
		{
			std::string name;	//�}�e���A����
			size_t startOffset;	//���蓖�Ĕ͈͂̐擪�ʒu
		};

		std::vector<vec3> positions;
		std::vector<vec2> texcoords;
		std::vector<vec3> normals;
		std::vector<IndexSet> faceIndexSet;
		std::vector<MaterialPtr> materials;
		std::vector<UseMaterial> usemtls;

		positions.reserve(20'000);
		normals.reserve(20'000);
		faceIndexSet.reserve(20'000 * 3);	//���̐��l��Ps4�Ȃ�]�T�ASwitch�Ȃ炿�傤��
		materials.reserve(100);
		usemtls.reserve(100);

		//���f�[�^��ǉ��i�}�e���A���w�肪�Ȃ��t�@�C���΍�j
		usemtls.push_back({ std::string(),0 });

		while (!file.eof())
		{
			std::string line;
			std::getline(file, line);
			const char* p = line.c_str();

			//���_���W�̓ǂݎ������݂�
			vec3 v;

			if (sscanf(p, " v %f %f %f", &v.x, &v.y, &v.z) == 3)
			{
				positions.push_back(v);
				continue;
			}

			//�e�N�X�`�����W�̓ǂݎ������݂�
			vec2 vt;

			if (sscanf(p, " vt %f %f", &vt.x, &vt.y) == 2)
			{
				texcoords.push_back(vt);
				continue;
			}

			//�@���̓ǂݎ������݂�
			vec3 vn;

			if (sscanf(p, " vn %f %f %f", &vn.x, &vn.y, &vn.z) == 3)
			{
				normals.push_back(vn);
				continue;
			}

			//�C���f�b�N�X�f�[�^�̓ǂݎ������݂�
			IndexSet f0, f1, f2;
			int readByte;

			//���_���W�{�e�N�X�`�����W�{�@��
			if (sscanf(p, " f %u/%u/%u %u/%u/%u%n",
				&f0.v, &f0.vt,&f0.vn, &f1.v, &f1.vt,&f1.vn, &readByte) == 6)
			{
				p += readByte;	//�ǂݎ��ʒu���X�V

				for (;;)
				{
					if (sscanf(p, " %u/%u/%u%n", &f2.v, &f2.vt,&f2.vn, &readByte) != 3)
					{
						break;
					}
					p += readByte;	//�ǂݎ��ʒu���X�V
					faceIndexSet.push_back(f0);
					faceIndexSet.push_back(f1);
					faceIndexSet.push_back(f2);

					f1 = f2;	//���̎O�p�`�̂��߂Ƀf�[�^���ڍs
				}
				continue;
			}

			//���_���W�{�e�N�X�`�����W
			if (sscanf(p, " f %u/%u %u/%u%n",
				&f0.v, &f0.vt, &f1.v, &f1.vt, &readByte) == 4)
			{
				f0.vn = f1.vn = 0;	//�@������
				p += readByte;		//�ǂݎ��ʒu���X�V

				for (;;)
				{
					if (sscanf(p, " %u/%u%n", &f2.v, &f2.vt, &readByte) != 2)
					{
						break;
					}
					f2.vn = 0;		//�@���Ȃ�
					p += readByte;	//�ǂݎ��ʒu���X�V
					faceIndexSet.push_back(f0);
					faceIndexSet.push_back(f1);
					faceIndexSet.push_back(f2);

					f1 = f2;	//���̎O�p�`�̂��߂Ƀf�[�^���ڍs
				}
				continue;
			}

			//MTL�t�@�C���̓ǂݎ������݂�
			char mtlFilename[1000];

			if (sscanf(line.data(), " mtllib %999s", mtlFilename) == 1)
			{
				const auto tmp = LoadMTL(foldername, mtlFilename);
				materials.insert(materials.end(), tmp.begin(), tmp.end());

				continue;
			}

			//�g�p�}�e���A�����̓ǂݎ������݂�
			char mtlName[1000];

			if (sscanf(line.data(), " usemtl %999s", mtlName) == 1)
			{
				//�O��ƈقȂ�}�e���A�����w�肳�ꂽ�ꍇ�̂݁A�V�����}�e���A���͈�
				if (usemtls.back().name != mtlName)
				{
					usemtls.push_back({ mtlName,faceIndexSet.size() });
				}

				continue;
			}
		}

		//�����ɔԕ���ǉ�
		usemtls.push_back({ std::string(),faceIndexSet.size() });

		//�ǂݍ��񂾃f�[�^���AOpenGL�Ŏg����f�[�^�ɕϊ�
		std::vector<Vertex> vertices;
		vertices.reserve(faceIndexSet.size());

		std::vector<uint16_t> indices;
		indices.reserve(faceIndexSet.size());
		std::unordered_map<uint64_t, uint16_t> vertexMap;
		vertexMap.reserve(faceIndexSet.size());


		for (const auto& e : faceIndexSet)
		{

			//���_ID���쐬 (21�r�b�g�����炵�ĉ��Z)
			const uint64_t vertexId =
				(static_cast<uint64_t>(e.v) << 42) +
				(static_cast<uint64_t>(e.vt) << 21) +
				static_cast<uint64_t>(e.vn);

			//��v���钸�_ID������΁A���̃y�A�̃C���f�b�N�X���g��
			const auto itr = vertexMap.find(vertexId);

			if (itr != vertexMap.end())
			{
				indices.push_back(itr->second);

				continue;
			}

			//��v���钸�_ID���Ȃ���΁A�V�����C���f�b�N�X�ƒ��_�f�[�^���쐬����
			const uint16_t index = static_cast<uint16_t>(vertices.size());
			indices.push_back(index);
			vertexMap.emplace(vertexId, index);//���_ID�ƃC���f�b�N�X�̃y�A��ǉ�

			//�V�������_�f�[�^���쐬
			Vertex v;
			v.position = positions[e.v - 1];
			v.texcoord = texcoords[e.vt - 1];

			//�@�����Ȃ��ꍇ�͉��f�[�^��ݒ�
			if (e.vn == 0)
			{
				v.normal = vec3(0, 0, 1);
			}
			else
			{
				v.normal = normals[e.vn - 1];
			}

			v.tangent = vec4(0);	//�����l���O�ɂ���
			vertices.push_back(v);
		}

		//�^���W�F���g�x�N�g�����v�Z
		if (mikkTSpace)
		{
			//MikkTSpace���C�u�������g���ă^���W�F���g���v�Z
			UserData userData = { vertices,indices };
			SMikkTSpaceContext context = { &mikkTSpace->interface,&userData };
			genTangSpaceDefault(&context);
		}
		else
		{
			//�蓮�Ń^���W�F���g���v�Z
			for (size_t i = 0; i < indices.size(); ++i)
			{
				//���_���W�ƃe�N�X�`�����W�̕Ӄx�N�g�����v�Z
				const size_t offset = (i / 3) * 3;
				const int i0 = indices[(i + 0) % 3 + offset];
				const int i1 = indices[(i + 1) % 3 + offset];
				const int i2 = indices[(i + 2) % 3 + offset];

				Vertex& v0 = vertices[i0];
				Vertex& v1 = vertices[i1];
				Vertex& v2 = vertices[i2];

				vec3 e1 = v1.position - v0.position;	//���W�x�N�g��
				vec3 e2 = v2.position - v0.position;	//���W�x�N�g��

				vec2 uv1 = v1.texcoord - v0.texcoord;	//�e�N�X�`�����W�x�N�g��
				vec2 uv2 = v2.texcoord - v0.texcoord;	//�e�N�X�`�����W�x�N�g��

				//�e�N�X�`���̖ʐς��ق�0�̏ꍇ�͖���
				float uvArea = uv1.x * uv2.y - uv1.y * uv2.x;	//uv�x�N�g���ɂ�镽�s�l�ӌ`�̖ʐ�

				if (abs(uvArea) < 0x1p-20)//�\�Z�i���������_�����e����
				{
					continue;
				}

				//�e�N�X�`�����������ɒ����Ă���(�ʐς�����)�ꍇ�A�o�C�^���W�F���g�x�N�g���̌������t�ɂ���
				if (uvArea >= 0)
				{
					v0.tangent.w = 1;	//�\����
				}
				else
				{
					v0.tangent.w = -1;	//������
				}

				//�^���W�F���g���ʂƕ��s�ȃx�N�g���ɂ��邽�߁A�@�������̐���������
				e1 -= v0.normal * dot(e1, v0.normal);
				e2 -= v0.normal * dot(e2, v0.normal);

				//�^���W�F���g�x�N�g�����v�Z
				vec3 tangent = normalize((uv2.y * e1 - uv1.y * e2) * v0.tangent.w);

				//�Ӄx�N�g���̂Ȃ��p���d�v�x�Ƃ��ă^���W�F���g������
				float angle = acos(dot(e1, e2) / (length(e1) * length(e2)));
				v0.tangent += vec4(tangent * angle, 0);
			}

			//�^���W�F���g�x�N�g���𐳋K��
			for (auto& e : vertices)
			{
				vec3 t = normalize(vec3(e.tangent.x, e.tangent.y, e.tangent.z));
				e.tangent = vec4(t.x, t.y, t.z, e.tangent.w);
			}
		}
		//�ϊ������f�[�^���v���~�e�B�u�o�b�t�@�ɒǉ�
		const GLint baseVertex =
			primitiveBuffer.AddVertices(vertices.data(), vertices.size());

		const void* indexOffset =
			primitiveBuffer.AddIndices(indices.data(), indices.size());
		
		//���b�V�����쐬
		auto pMesh = std::make_shared<StaticMesh>();

		//�v���~�e�B�u���쐬
		size_t i = 0;

		if (usemtls.size() > 2)
		{
			i = 1;	//���f�[�^�Ɣԕ��ȊO�̃}�e���A��������ꍇ�A���f�[�^���΂�
		}

		for (;i < usemtls.size() - 1; ++i)
		{
			const UseMaterial& cur = usemtls[i];
			const UseMaterial& next = usemtls[i + 1];

			if (next.startOffset == cur.startOffset)
			{
				continue;	//�C���f�b�N�X�f�[�^���Ȃ��ꍇ�͔�΂�
			}


			Primitive prim;
			prim.mode = GL_TRIANGLES;
			prim.count = static_cast<GLsizei>(next.startOffset - cur.startOffset);
			prim.indices = indexOffset;
			prim.baseVertex = baseVertex;
			prim.materialNo = 0;//�f�t�H���g�l��ݒ�

			for (int i = 0; i < materials.size(); ++i)
			{
				if (materials[i]->name == cur.name)
				{
					prim.materialNo = i;//���O�̈�v����}�e���A����ݒ�
					break;
				}
			}

			pMesh->primitives.push_back(prim);

			//�C���f�b�N�X�I�t�Z�b�g��i�߂�
			indexOffset = reinterpret_cast<void*>(
				reinterpret_cast<size_t>(indexOffset) + sizeof(uint16_t) * prim.count);
		}

		//�}�e���A���z�񂪋�̏ꍇ�A�f�t�H���g�}�e���A����ǉ�
		if (materials.empty())
		{
			pMesh->materials.push_back(std::make_shared<Material>());
		}
		else
		{
			pMesh->materials.assign(materials.begin(), materials.end());
		}
		pMesh->name = filename;
		meshes.emplace(pMesh->name, pMesh);

		LOG("%s��ǂݍ��݂܂���(vertices/indices=%d/%d)", filename,
			vertices.size(),indices.size());

		//�쐬�������b�V����Ԃ�
		return pMesh;
	}

	//VAO���擾
	VertexArrayPtr MeshBuffer::GetVAO() const
	{
		return primitiveBuffer.GetVAO();
	}

	//���ׂẴ��b�V�����폜����
	void MeshBuffer::Clear()
	{
		meshes.clear();
		primitiveBuffer.Clear();
	}

	//���b�V�����쐬����		
	//@param name			���b�V����(�����Ȃǂŗ��p)
	//@param mode			�v���~�e�B�u�̎��
	//@param vertices		���_�f�[�^�̃A�h���X
	//@param vertexCount  ���_�f�[�^��
	//@param indices		�C���f�b�N�X�f�[�^�̃A�h���X
	//@param indexCount	�C���f�b�N�X�f�[�^��

	//@return �쐬�������b�V���ւ̃|�C���^

	StaticMeshPtr MeshBuffer::CreateMesh(
		const char* name, GLenum  mode,
		const Vertex* vertices, size_t vertexCount,
		const uint16_t* indices, size_t indexCount)
	{
		//�f�[�^���v���~�e�B�u�o�b�t�@�ɒǉ�
		const GLint baseVertex = primitiveBuffer.AddVertices(vertices, vertexCount);
		const void* indexOffset = primitiveBuffer.AddIndices(indices, indexCount);

		//���b�V�����쐬
		auto pMesh = std::make_shared<StaticMesh>();
		pMesh->name = name;

		//�v���~�e�B�u���쐬
		Primitive prim;
		prim.mode = mode;
		prim.count = static_cast<GLsizei>(indexCount);
		prim.indices = indexOffset;
		prim.baseVertex = baseVertex;
		prim.materialNo = 0;
		pMesh->primitives.push_back(prim);

		//�}�e���A����VAO��ݒ�
		pMesh->materials.push_back(defaultMaterial);
		//pMesh->vao = primitiveBuffer.GetVAO();
		meshes.emplace(pMesh->name, pMesh);

		LOG("%s���b�V�����쐬���܂���(vertices/indices=%d/%d)", name,
			vertexCount, indexCount);

		//�쐬�������b�V����Ԃ�
		return pMesh;
	}

	//�����̃��b�V�����쐬����
	//@param name  ���b�V����(�����ȂǂŎg�p)
	//@return �쐬�������b�V���ւ̃|�C���^

	StaticMeshPtr MeshBuffer::CreateBox(const char* name)
	{
		const vec3 positions[] = {
			{ 1,-1, 1},{ 1,-1,-1},{ 1, 1,-1},{ 1, 1, 1},	//+x�@�E
			{-1,-1,-1},{-1,-1, 1},{-1, 1, 1},{-1, 1,-1},	//-x�@��
			{-1, 1, 1},{ 1, 1, 1},{ 1, 1,-1},{-1, 1,-1},	//+y�@��
			{ 1,-1, 1},{-1,-1, 1},{-1,-1,-1},{ 1,-1,-1},	//-y  ��
			{-1,-1, 1},{ 1,-1, 1},{ 1, 1, 1},{-1, 1, 1},	//+z�@��
			{ 1,-1,-1},{-1,-1,-1},{-1, 1,-1},{ 1, 1,-1},	//-z�@��O
		};

		const vec3 normals[] = { {1,0,0},{0,1,0},{0,0,1} };				//�@���f�[�^
		const vec4 tangents[] = { {0,0,-1,1},{1,0,0,1},{-1,0,0,1} };	//�^���W�F���g�f�[�^

		//���_�f�[�^�𐶐�
		std::vector<Vertex> vertices(6 * 4);

		//��̃|�W�V��������
		for (int i = 0; i < 24; i += 4)
		{
			const int axis = i / 8;	//�ʂ��ǂ̕����������Ă��邩
			const float sign = (i & 0b100) ? -1.0f : 1.0f; //3�߂̃r�b�g���Q�Ƃ��Ă���
			const vec3 normal = normals[axis] * sign;
			const vec4 tangent = tangents[axis] * sign;

			vertices[i + 0] = { positions[i + 0],{0,0},normal,tangent };
			vertices[i + 1] = { positions[i + 1],{1,0},normal,tangent };
			vertices[i + 2] = { positions[i + 2],{1,1},normal,tangent };
			vertices[i + 3] = { positions[i + 3],{0,1},normal,tangent };
		}

		const uint16_t indices[] = {
			 0, 1, 2, 2, 3, 0,	//+x�@�E
			 4, 5, 6, 6, 7, 4,	//-x�@��
			 8, 9,10,10,11, 8,	//+y�@��
			12,13,14,14,15,12,	//-y  ��
			16,17,18,18,19,16,	//+z�@��
			20,21,22,22,23,20,	//-z�@��O
		};

		return CreateMesh(name, GL_TRIANGLES,
			vertices.data(), vertices.size(), indices, std::size(indices));
	}


	//���̃��b�V�����쐬����
	//@param name     ���b�V����(�����ȂǂŎg�p)
	//@param segment  �o�x�����̕�����
	//@param ring     �ܓx�����̕�����
	 
	//@return �쐬�������b�V���ւ̃|�C���^
	StaticMeshPtr MeshBuffer::CreateSphere(const char* name,
		int segment, int ring)
	{
		//�ܓx�o�x�����̊p�x�ω����擾
		const float angleLon = radians(360.0f / segment);	//Lon=Longitude
		const float angleLat = radians(180.0f / ring);		//Lat=Latitude
	
		//���_�f�[�^���擾
		std::vector<Vertex> vertices((segment + 1) * (ring + 1));
		auto v = vertices.begin();

		//XY���ʂŉ�]
		for (float lat = 0; lat < ring + 1; ++lat)
		{
			//���̂̉�������v�Z�����邽�߁A-pi*0.5
			const float x = std::cos(angleLat * lat - pi * 0.5f);
			const float y = std::sin(angleLat * lat - pi * 0.5f);

			//XZ���ʂŉ�]
			for (float lon = 0; lon < segment + 1; ++lon)
			{
				v->position.x = x * std::cos(angleLon * lon);
				v->position.y = y;
				v->position.z = x * -std::sin(angleLon * lon);

				v->normal = v->position;	//���a���P�̋��Ȃ̂ŁA�g���܂킷
				v->tangent = vec4(v->normal.z, v->normal.y, -v->normal.x, 1);//XZ���ʂ�90�x��]�����l
				v->texcoord.x = lon / segment;
				v->texcoord.y = lat / ring;

				++v;
			}
		}

		//�C���f�b�N�X�f�[�^�𐶐�
		std::vector<uint16_t> indices(segment * ring * 6);
		auto i = indices.begin();
		const uint16_t oneRingVertexCount = segment + 1;

		for (int lat = 0; lat < ring; ++lat)
		{
			for (int lon = 0; lon < segment; ++lon)
			{
				const uint16_t base = lat * oneRingVertexCount + lon;

				i[0] = base;
				i[1] = base + 1;
				i[2] = base + oneRingVertexCount + 1;
				i[3] = base + oneRingVertexCount + 1;
				i[4] = base + oneRingVertexCount;
				i[5] = base;

				i += 6;
			}
		}
		return CreateMesh(name, GL_TRIANGLES,
			vertices.data(), vertices.size(), indices.data(), indices.size());
	}

	//�J�v�Z�����b�V�����쐬
	//@param name
	//@param length
	//@param segment
	//@param ring
	
	//@return �쐬�������b�V���ւ̃|�C���^
	StaticMeshPtr MeshBuffer::CreateCapsule(const char* name, float length,
		int segment, int ring)
	{
		//�~�������̔��a���P�ɂȂ�悤�ɁA�����O���������ɂ���
		ring = ((ring + 1) / 2) * 2;

		const float angleLon = radians(360.0f / segment);	//�o�x
		const float angleLat = radians(180.0f / ring);		//�ܓx

		//���̂����ɂ��炵�����_�f�[�^���쐬
		std::vector<Vertex> vertices((segment + 1) * (ring + 2));
		auto v = vertices.begin();

		for (float lat = 0; lat < ring + 1; ++lat)
		{
			const float x = std::cos(angleLat * lat - pi * 0.5f);
			const float y = std::sin(angleLat * lat - pi * 0.5f);

			for (float lon = 0; lon < segment + 1; ++lon)
			{
				v->position.x = x * std::cos(angleLon * lon);
				v->position.y = y;
				v->position.z = x * -std::sin(angleLon * lon);

				v->normal = v->position;
				v->tangent = vec4(v->normal.z, v->normal.y, -v->normal.x, 1);
				v->texcoord.x = lon / segment;
				v->texcoord.y = lat / (ring + ring * length);	//�~���̒������l�����ăe�N�X�`����ݒ�
				v->position.y -= length * 0.5f;	//�J�v�Z���������Ɉړ�

				++v;
			}
		}

		//���̂̏㔼�����R�s�[���āA�������Length�������炷
		auto rend = vertices.begin() + (segment + 1) * (ring / 2);	//���o�[�X�G���h(�I�_)
		auto src = vertices.begin() + (segment + 1) * (ring + 1);	//�\�[�X(���f�[�^)
		auto dest = vertices.end();		//Destination(�s��)

		const float texcoordOffsetY = (ring * length) / (ring + ring * length);

		//�I�_�܂ők��
		while (src != rend)
		{
			//����f�[�^�������̂ڂ��Ă���
			*(--dest) = *(--src);
			dest->position.y += length;
			dest->texcoord.y += texcoordOffsetY;
		}

		//�C���f�b�N�X�f�[�^�𐶐�
		std::vector<uint16_t> indices(segment * (ring + 1) * 6);
		auto i = indices.begin();

		const uint16_t oneRingVertexCount = segment + 1;

		for (int lat = 0; lat < ring + 1; ++lat)
		{
			for (int lon = 0; lon < segment; ++lon)
			{
				const uint16_t base = lat * oneRingVertexCount + lon;

				//���_����
				i[0] = base;
				i[1] = base + 1;
				i[2] = base + oneRingVertexCount + 1;
				i[3] = base + oneRingVertexCount + 1;
				i[4] = base + oneRingVertexCount;
				i[5] = base;

				i += 6;
			}
		}

		return CreateMesh(name, GL_TRIANGLES,
			vertices.data(), vertices.size(), indices.data(), indices.size());
	}
}











