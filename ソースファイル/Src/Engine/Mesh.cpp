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

	//MikkTSpace用のユーザーデータ
	struct UserData
	{
		std::vector<Mesh::Vertex>& vertices;
		std::vector<uint16_t>& indices;
	};

	//MikkTSpaceのインターフェイス実装
	class MikkTSpace
	{
	public:
		//コンストラクタ
		MikkTSpace()
		{
			//モデルの総ポリゴン数を返す
			interface.m_getNumFaces = [](const SMikkTSpaceContext* pContext)
			{
				UserData* p = static_cast<UserData*>(pContext->m_pUserData);
				return static_cast<int>(p->indices.size() / 3);
			};

			//ポリゴンの頂点数を返す
			interface.m_getNumVerticesOfFace = [](const SMikkTSpaceContext* pContext,
				int iFace)
			{
				//三角形なので３を返す
				return 3;
			};

			//頂点の座標を返す
			interface.m_getPosition = [](const SMikkTSpaceContext* pContext,
				float fvPosOut[], int iFace, int iVert)
			{
				UserData* p = static_cast<UserData*>(pContext->m_pUserData);

				//iFace番目のポリゴンのIvert番目の頂点を順番に代入
				const int index = p->indices[iFace * 3 + iVert];
				fvPosOut[0] = p->vertices[index].position.x;
				fvPosOut[1] = p->vertices[index].position.y;
				fvPosOut[2] = p->vertices[index].position.z;
			};

			//頂点の法線を返す
			interface.m_getNormal = [](const SMikkTSpaceContext* pContext,
				float fvNormaOut[], int iFace, int iVert)
			{
				UserData* p = static_cast<UserData*>(pContext->m_pUserData);

				const int index = p->indices[iFace * 3 + iVert];
				fvNormaOut[0] = p->vertices[index].normal.x;
				fvNormaOut[1] = p->vertices[index].normal.y;
				fvNormaOut[2] = p->vertices[index].normal.z;
			};

			//頂点のテクスチャ座標を返す
			interface.m_getTexCoord = [](const SMikkTSpaceContext* pContext,
				float fvTexcOut[], int iFace, int iVert)
			{
				UserData* p = static_cast<UserData*>(pContext->m_pUserData);

				const int index = p->indices[iFace * 3 + iVert];
				fvTexcOut[0] = p->vertices[index].normal.x;
				fvTexcOut[1] = p->vertices[index].normal.y;
			};

			//タンジェントベクトルとバイタンジェントベクトルの向きを受け取る
			interface.m_setTSpaceBasic = [](const SMikkTSpaceContext* pContext,
				const float fvTangent[], float fSign,int iFace, int iVert)//ベクトル、向き、ポリゴン番号、頂点番号
			{
				UserData* p = static_cast<UserData*>(pContext->m_pUserData);

				const int index = p->indices[iFace * 3 + iVert];
				p->vertices[index].tangent =
					vec4(fvTangent[0], fvTangent[1], fvTangent[2], fSign);
			};

			//使わないのでnullptrを設定
			interface.m_setTSpace = nullptr;

		}

		//デストラクタ
		~MikkTSpace() = default;

		//メンバ変数
		SMikkTSpaceInterface interface;
	};

	//primitiveを描画する
	void Draw(const Primitive& prim)
	{
		glDrawElementsInstancedBaseVertex(prim.mode, prim.count,	//基本図形の種類、データ数
			GL_UNSIGNED_SHORT, prim.indices, 1, prim.baseVertex);	//インデックスデータの型、データの開始位置、描画のする図形の数、頂点データの開始位置
	}

	//メッシュを描画する
	void Draw(const StaticMesh& mesh,const std::vector<MaterialPtr>& materials ,GLuint program)
	{
		for (const auto& prim : mesh.primitives)
		{
			//マテリアルを設定
			if (prim.materialNo >= 0 && prim.materialNo < materials.size())
			{
				const Material& material = *materials[prim.materialNo];

				if (program)
				{
					glProgramUniform4fv(program, 102, 1, &material.baseColor.x);
					glProgramUniform3f(program, 103,
						material.specularPower, material.normalizeFactor, material.receiveShadows);
				}

				//GLコンテキストにテクスチャを割り当てる
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

	//MTLファイルを読み込む（マテリアルテンプレートライブラリ）
	//@param foldername OBJファイルのあるフォルダ名
	//@param filename   MTLファイル名

	//return MTLファイルに含まれるマテリアルの配列

	std::vector<MaterialPtr> MeshBuffer::LoadMTL(
		const std::string& foldername, const char* filename)
	{
		//MTLファイルを開く
		const std::string fullpath = foldername + filename;
		std::ifstream file(fullpath);

		if (!file)
		{
			LOG_ERROR("%sを開けません", fullpath.c_str());
			return{};
		}

		//MTLファイルを解析する
		std::vector<MaterialPtr> materials;
		MaterialPtr pMaterial;

		while (!file.eof())
		{
			std::string line;
			std::getline(file, line);

			//マテリアル定義の読み取りを試みる
			char name[1000] = { 0 };

			if (sscanf(line.data(), " newmtl %999s", name) == 1)
			{
				pMaterial = std::make_shared<Material>();
				pMaterial->name = name;
				pMaterial->texBaseColor = defaultMaterial->texBaseColor;
				materials.push_back(pMaterial);

				continue;
			}


			//マテリアルが定義されていない場合は行を無視する
			if (!pMaterial)
			{
				continue;
			}

			//基本色の読み取りを試みる
			if (sscanf(line.data(), " Kd %f %f %f",
				&pMaterial->baseColor.x, &pMaterial->baseColor.y,
				&pMaterial->baseColor.z) == 3)
			{
				continue;
			}

			//不透明度の読み取りを試みる
			if (sscanf(line.data(), " d %f", &pMaterial->baseColor.w) == 1)
			{
				continue;
			}

			//鏡面反射指数の読み取りを試みる
			if (sscanf(line.data(), " Ns %f", &pMaterial->specularPower) == 1)
			{
				//正規化係数を求める
				pMaterial->normalizeFactor =
					(pMaterial->specularPower + 8) / (8 * pi);
				
				continue;
			}

			//基本色テクスチャ名の読み取りを試みる
			char textureName[1000] = { 0 };

			if (sscanf(line.data(), " map_Kd %999[^\n]", &textureName) == 1)
			{
				pMaterial->texBaseColor =
					Texture::Create(foldername + textureName, GL_LINEAR,
						Texture::Usage::for3DLinear);

				continue;
			}

			//法線テクスチャ名の読み取りを試みる
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
		//読み込んだマテリアル配列を返す
		return materials;
	}

	//コンストラクタ
	//@param vboCapacity　VBOの最大容量（バイト数）
	//@param iboCapacity　IBOの最大容量（バイト数）

	PrimitiveBuffer::PrimitiveBuffer(size_t vboCapacity, size_t iboCapacity) :
		vboCapacity(vboCapacity), iboCapacity(iboCapacity)
	{
		const GLenum flags = GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT;

		//VBOを作成
		glCreateBuffers(1, &vbo);
		glNamedBufferStorage(vbo, vboCapacity, nullptr, flags);
		pVbo = static_cast<uint8_t*>(glMapNamedBufferRange(vbo, 0, vboCapacity, flags));

		//IBOを作成
		glCreateBuffers(1, &ibo);
		glNamedBufferStorage(ibo, iboCapacity, nullptr, flags);
		pIbo = static_cast<uint8_t*>(glMapNamedBufferRange(ibo, 0, iboCapacity, flags));
	
		//VAOを作成し、OpenGLコンテキストに割り当てる
		vao = VertexArray::Create();
		glBindVertexArray(*vao);

		//VAOに、VBOとIBOを割り当てる
		//頂点んアトリビュート設定前に行う必要がある
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

		//頂点アトリビュートを設定
		const auto setAttribute = [](GLuint index, GLint size, size_t offset)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, sizeof(Vertex),
				reinterpret_cast<void*>(offset));
		};

		//シェーダ―に情報を送る
		setAttribute(0, 3, offsetof(Vertex, position));
		setAttribute(1, 2, offsetof(Vertex, texcoord));
		setAttribute(2, 3, offsetof(Vertex, normal));
		setAttribute(3, 4, offsetof(Vertex, tangent));

		//OpenGLコンテキストへの割り当てを解除
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	//デストラクタ
	PrimitiveBuffer::~PrimitiveBuffer()
	{
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ibo);
	}

	//プリミティブを追加する
	//@param vertices　　GPUメモリにコピーする頂点データ配列
	//@param vertexCount verticesの要素数
	//@param indices　	 GPUメモリにコピーするインデックスデータ配列
	//@param indexCount　indicesの要素数
	//@param mode　		 プリミティブの種類

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


	//頂点データをGPUメモリにコピー
	//@param vertices    GPUメモリにコピーする頂点データ配列
	//@param vertexCount verticesの要素数

	GLint PrimitiveBuffer::AddVertices(const Vertex* vertices, size_t count)
	{
		const GLint offset = static_cast<GLint>(vboSize / sizeof(Vertex));

		const size_t verticesSize = count * sizeof(Vertex);
		memcpy(pVbo + vboSize, vertices, verticesSize);
		vboSize += verticesSize;

		return offset;
	}

	//頂点インデックスをGPUメモリーにコピー
	//@param indices    GPUメモリにコピーするインデックスデータ配列
	//@param indexCount indicesの要素数

	const void* PrimitiveBuffer::AddIndices(const uint16_t* indices, size_t count)
	{
		const void* offset = reinterpret_cast<void*>(iboSize);

		const size_t indexSize = count * sizeof(uint16_t);
		memcpy(pIbo + iboSize, indices, indexSize);
		iboSize += indexSize;

		return offset;
	}

	//すべてのプリミティブを削除する
	void PrimitiveBuffer::Clear()
	{
		primitives.clear();

		//先頭がコピーできるように
		vboSize = 0;
		iboSize = 0;
	}

	//コンストラクタ
	//@param vboCapacity　VBOの最大容量（バイト数）
	//@param iboCapacity　IBOの最大容量（バイト数）
	MeshBuffer::MeshBuffer(size_t vboCapacity, size_t iboCapacity) :
		primitiveBuffer(vboCapacity, iboCapacity)
	{
		//タンジェント空間計算用のオブジェクトを作成する
		mikkTSpace = std::make_shared<MikkTSpace>();
		if (!mikkTSpace)
		{
			LOG_ERROR("MikkTSpaceの作成にしっぱい");
		}

		//マテリアル未設定時に使用するマテリアルを作成
		defaultMaterial = std::make_shared<Material>();
		defaultMaterial->texBaseColor = Texture::Create("defaultTexture", 4, 4,
			GL_SRGB8_ALPHA8,GL_LINEAR);
		
		std::vector<uint32_t> img(4 * 4, 0xff'cc'cc'cc);
		glTextureSubImage2D(*defaultMaterial->texBaseColor, 0, 0, 0,
			4, 4, GL_BGRA, GL_UNSIGNED_BYTE, img.data());
	}

	//OBJファイルを読み込む
	//@param filename OBJファイル名
	//@return filenameから作成したメッシュ

	StaticMeshPtr MeshBuffer::LoadOBJ(const char* filename)
	{
		//以前に読み込んだファイルなら、作成済みのメッシュを返す
		{
			auto itr = meshes.find(filename);

			if (itr != meshes.end())
			{
				return itr->second;
			}
		}

		//OBJファイルを開く
		std::ifstream file(filename);

		if (!file)
		{
			LOG_ERROR("%sを開けません", filename);
			return nullptr;
		}

		//フォルダ名を取得する
		std::string foldername(filename);
		{
			const size_t p = foldername.find_last_of("\\/");

			if (p != std::string::npos)
			{
				foldername.resize(p + 1);
			}
		}

		//OBJファイルを解析して、頂点データとインデックスデータを読み込む
		struct IndexSet { int v, vt, vn; };		//インデックスデータの組

		struct UseMaterial
		{
			std::string name;	//マテリアル名
			size_t startOffset;	//割り当て範囲の先頭位置
		};

		std::vector<vec3> positions;
		std::vector<vec2> texcoords;
		std::vector<vec3> normals;
		std::vector<IndexSet> faceIndexSet;
		std::vector<MaterialPtr> materials;
		std::vector<UseMaterial> usemtls;

		positions.reserve(20'000);
		normals.reserve(20'000);
		faceIndexSet.reserve(20'000 * 3);	//この数値はPs4なら余裕、Switchならちょうど
		materials.reserve(100);
		usemtls.reserve(100);

		//仮データを追加（マテリアル指定がないファイル対策）
		usemtls.push_back({ std::string(),0 });

		while (!file.eof())
		{
			std::string line;
			std::getline(file, line);
			const char* p = line.c_str();

			//頂点座標の読み取りを試みる
			vec3 v;

			if (sscanf(p, " v %f %f %f", &v.x, &v.y, &v.z) == 3)
			{
				positions.push_back(v);
				continue;
			}

			//テクスチャ座標の読み取りを試みる
			vec2 vt;

			if (sscanf(p, " vt %f %f", &vt.x, &vt.y) == 2)
			{
				texcoords.push_back(vt);
				continue;
			}

			//法線の読み取りを試みる
			vec3 vn;

			if (sscanf(p, " vn %f %f %f", &vn.x, &vn.y, &vn.z) == 3)
			{
				normals.push_back(vn);
				continue;
			}

			//インデックスデータの読み取りを試みる
			IndexSet f0, f1, f2;
			int readByte;

			//頂点座標＋テクスチャ座標＋法線
			if (sscanf(p, " f %u/%u/%u %u/%u/%u%n",
				&f0.v, &f0.vt,&f0.vn, &f1.v, &f1.vt,&f1.vn, &readByte) == 6)
			{
				p += readByte;	//読み取り位置を更新

				for (;;)
				{
					if (sscanf(p, " %u/%u/%u%n", &f2.v, &f2.vt,&f2.vn, &readByte) != 3)
					{
						break;
					}
					p += readByte;	//読み取り位置を更新
					faceIndexSet.push_back(f0);
					faceIndexSet.push_back(f1);
					faceIndexSet.push_back(f2);

					f1 = f2;	//次の三角形のためにデータを移行
				}
				continue;
			}

			//頂点座標＋テクスチャ座標
			if (sscanf(p, " f %u/%u %u/%u%n",
				&f0.v, &f0.vt, &f1.v, &f1.vt, &readByte) == 4)
			{
				f0.vn = f1.vn = 0;	//法線無し
				p += readByte;		//読み取り位置を更新

				for (;;)
				{
					if (sscanf(p, " %u/%u%n", &f2.v, &f2.vt, &readByte) != 2)
					{
						break;
					}
					f2.vn = 0;		//法線なし
					p += readByte;	//読み取り位置を更新
					faceIndexSet.push_back(f0);
					faceIndexSet.push_back(f1);
					faceIndexSet.push_back(f2);

					f1 = f2;	//次の三角形のためにデータを移行
				}
				continue;
			}

			//MTLファイルの読み取りを試みる
			char mtlFilename[1000];

			if (sscanf(line.data(), " mtllib %999s", mtlFilename) == 1)
			{
				const auto tmp = LoadMTL(foldername, mtlFilename);
				materials.insert(materials.end(), tmp.begin(), tmp.end());

				continue;
			}

			//使用マテリアル名の読み取りを試みる
			char mtlName[1000];

			if (sscanf(line.data(), " usemtl %999s", mtlName) == 1)
			{
				//前回と異なるマテリアルが指定された場合のみ、新しいマテリアル範囲
				if (usemtls.back().name != mtlName)
				{
					usemtls.push_back({ mtlName,faceIndexSet.size() });
				}

				continue;
			}
		}

		//末尾に番兵を追加
		usemtls.push_back({ std::string(),faceIndexSet.size() });

		//読み込んだデータを、OpenGLで使えるデータに変換
		std::vector<Vertex> vertices;
		vertices.reserve(faceIndexSet.size());

		std::vector<uint16_t> indices;
		indices.reserve(faceIndexSet.size());
		std::unordered_map<uint64_t, uint16_t> vertexMap;
		vertexMap.reserve(faceIndexSet.size());


		for (const auto& e : faceIndexSet)
		{

			//頂点IDを作成 (21ビットずつずらして加算)
			const uint64_t vertexId =
				(static_cast<uint64_t>(e.v) << 42) +
				(static_cast<uint64_t>(e.vt) << 21) +
				static_cast<uint64_t>(e.vn);

			//一致する頂点IDがあれば、そのペアのインデックスを使う
			const auto itr = vertexMap.find(vertexId);

			if (itr != vertexMap.end())
			{
				indices.push_back(itr->second);

				continue;
			}

			//一致する頂点IDがなければ、新しいインデックスと頂点データを作成する
			const uint16_t index = static_cast<uint16_t>(vertices.size());
			indices.push_back(index);
			vertexMap.emplace(vertexId, index);//頂点IDとインデックスのペアを追加

			//新しい頂点データを作成
			Vertex v;
			v.position = positions[e.v - 1];
			v.texcoord = texcoords[e.vt - 1];

			//法線がない場合は仮データを設定
			if (e.vn == 0)
			{
				v.normal = vec3(0, 0, 1);
			}
			else
			{
				v.normal = normals[e.vn - 1];
			}

			v.tangent = vec4(0);	//初期値を０にする
			vertices.push_back(v);
		}

		//タンジェントベクトルを計算
		if (mikkTSpace)
		{
			//MikkTSpaceライブラリを使ってタンジェントを計算
			UserData userData = { vertices,indices };
			SMikkTSpaceContext context = { &mikkTSpace->interface,&userData };
			genTangSpaceDefault(&context);
		}
		else
		{
			//手動でタンジェントを計算
			for (size_t i = 0; i < indices.size(); ++i)
			{
				//頂点座標とテクスチャ座標の辺ベクトルを計算
				const size_t offset = (i / 3) * 3;
				const int i0 = indices[(i + 0) % 3 + offset];
				const int i1 = indices[(i + 1) % 3 + offset];
				const int i2 = indices[(i + 2) % 3 + offset];

				Vertex& v0 = vertices[i0];
				Vertex& v1 = vertices[i1];
				Vertex& v2 = vertices[i2];

				vec3 e1 = v1.position - v0.position;	//座標ベクトル
				vec3 e2 = v2.position - v0.position;	//座標ベクトル

				vec2 uv1 = v1.texcoord - v0.texcoord;	//テクスチャ座標ベクトル
				vec2 uv2 = v2.texcoord - v0.texcoord;	//テクスチャ座標ベクトル

				//テクスチャの面積がほぼ0の場合は無視
				float uvArea = uv1.x * uv2.y - uv1.y * uv2.x;	//uvベクトルによる平行四辺形の面積

				if (abs(uvArea) < 0x1p-20)//十六進浮動少数点数リテラル
				{
					continue;
				}

				//テクスチャが裏向きに張られている(面積が負の)場合、バイタンジェントベクトルの向きを逆にする
				if (uvArea >= 0)
				{
					v0.tangent.w = 1;	//表向き
				}
				else
				{
					v0.tangent.w = -1;	//裏向き
				}

				//タンジェント平面と平行なベクトルにするため、法線方向の成分を除去
				e1 -= v0.normal * dot(e1, v0.normal);
				e2 -= v0.normal * dot(e2, v0.normal);

				//タンジェントベクトルを計算
				vec3 tangent = normalize((uv2.y * e1 - uv1.y * e2) * v0.tangent.w);

				//辺ベクトルのなす角を重要度としてタンジェントを合成
				float angle = acos(dot(e1, e2) / (length(e1) * length(e2)));
				v0.tangent += vec4(tangent * angle, 0);
			}

			//タンジェントベクトルを正規化
			for (auto& e : vertices)
			{
				vec3 t = normalize(vec3(e.tangent.x, e.tangent.y, e.tangent.z));
				e.tangent = vec4(t.x, t.y, t.z, e.tangent.w);
			}
		}
		//変換したデータをプリミティブバッファに追加
		const GLint baseVertex =
			primitiveBuffer.AddVertices(vertices.data(), vertices.size());

		const void* indexOffset =
			primitiveBuffer.AddIndices(indices.data(), indices.size());
		
		//メッシュを作成
		auto pMesh = std::make_shared<StaticMesh>();

		//プリミティブを作成
		size_t i = 0;

		if (usemtls.size() > 2)
		{
			i = 1;	//仮データと番兵以外のマテリアルがある場合、仮データを飛ばす
		}

		for (;i < usemtls.size() - 1; ++i)
		{
			const UseMaterial& cur = usemtls[i];
			const UseMaterial& next = usemtls[i + 1];

			if (next.startOffset == cur.startOffset)
			{
				continue;	//インデックスデータがない場合は飛ばす
			}


			Primitive prim;
			prim.mode = GL_TRIANGLES;
			prim.count = static_cast<GLsizei>(next.startOffset - cur.startOffset);
			prim.indices = indexOffset;
			prim.baseVertex = baseVertex;
			prim.materialNo = 0;//デフォルト値を設定

			for (int i = 0; i < materials.size(); ++i)
			{
				if (materials[i]->name == cur.name)
				{
					prim.materialNo = i;//名前の一致するマテリアルを設定
					break;
				}
			}

			pMesh->primitives.push_back(prim);

			//インデックスオフセットを進める
			indexOffset = reinterpret_cast<void*>(
				reinterpret_cast<size_t>(indexOffset) + sizeof(uint16_t) * prim.count);
		}

		//マテリアル配列が空の場合、デフォルトマテリアルを追加
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

		LOG("%sを読み込みました(vertices/indices=%d/%d)", filename,
			vertices.size(),indices.size());

		//作成したメッシュを返す
		return pMesh;
	}

	//VAOを取得
	VertexArrayPtr MeshBuffer::GetVAO() const
	{
		return primitiveBuffer.GetVAO();
	}

	//すべてのメッシュを削除する
	void MeshBuffer::Clear()
	{
		meshes.clear();
		primitiveBuffer.Clear();
	}

	//メッシュを作成する		
	//@param name			メッシュ名(検索などで利用)
	//@param mode			プリミティブの種類
	//@param vertices		頂点データのアドレス
	//@param vertexCount  頂点データ数
	//@param indices		インデックスデータのアドレス
	//@param indexCount	インデックスデータ数

	//@return 作成したメッシュへのポインタ

	StaticMeshPtr MeshBuffer::CreateMesh(
		const char* name, GLenum  mode,
		const Vertex* vertices, size_t vertexCount,
		const uint16_t* indices, size_t indexCount)
	{
		//データをプリミティブバッファに追加
		const GLint baseVertex = primitiveBuffer.AddVertices(vertices, vertexCount);
		const void* indexOffset = primitiveBuffer.AddIndices(indices, indexCount);

		//メッシュを作成
		auto pMesh = std::make_shared<StaticMesh>();
		pMesh->name = name;

		//プリミティブを作成
		Primitive prim;
		prim.mode = mode;
		prim.count = static_cast<GLsizei>(indexCount);
		prim.indices = indexOffset;
		prim.baseVertex = baseVertex;
		prim.materialNo = 0;
		pMesh->primitives.push_back(prim);

		//マテリアルとVAOを設定
		pMesh->materials.push_back(defaultMaterial);
		//pMesh->vao = primitiveBuffer.GetVAO();
		meshes.emplace(pMesh->name, pMesh);

		LOG("%sメッシュを作成しました(vertices/indices=%d/%d)", name,
			vertexCount, indexCount);

		//作成したメッシュを返す
		return pMesh;
	}

	//立方体メッシュを作成する
	//@param name  メッシュ名(検索などで使用)
	//@return 作成したメッシュへのポインタ

	StaticMeshPtr MeshBuffer::CreateBox(const char* name)
	{
		const vec3 positions[] = {
			{ 1,-1, 1},{ 1,-1,-1},{ 1, 1,-1},{ 1, 1, 1},	//+x　右
			{-1,-1,-1},{-1,-1, 1},{-1, 1, 1},{-1, 1,-1},	//-x　左
			{-1, 1, 1},{ 1, 1, 1},{ 1, 1,-1},{-1, 1,-1},	//+y　上
			{ 1,-1, 1},{-1,-1, 1},{-1,-1,-1},{ 1,-1,-1},	//-y  下
			{-1,-1, 1},{ 1,-1, 1},{ 1, 1, 1},{-1, 1, 1},	//+z　奥
			{ 1,-1,-1},{-1,-1,-1},{-1, 1,-1},{ 1, 1,-1},	//-z　手前
		};

		const vec3 normals[] = { {1,0,0},{0,1,0},{0,0,1} };				//法線データ
		const vec4 tangents[] = { {0,0,-1,1},{1,0,0,1},{-1,0,0,1} };	//タンジェントデータ

		//頂点データを生成
		std::vector<Vertex> vertices(6 * 4);

		//上のポジション分回す
		for (int i = 0; i < 24; i += 4)
		{
			const int axis = i / 8;	//面がどの方向を向いているか
			const float sign = (i & 0b100) ? -1.0f : 1.0f; //3つめのビットを参照している
			const vec3 normal = normals[axis] * sign;
			const vec4 tangent = tangents[axis] * sign;

			vertices[i + 0] = { positions[i + 0],{0,0},normal,tangent };
			vertices[i + 1] = { positions[i + 1],{1,0},normal,tangent };
			vertices[i + 2] = { positions[i + 2],{1,1},normal,tangent };
			vertices[i + 3] = { positions[i + 3],{0,1},normal,tangent };
		}

		const uint16_t indices[] = {
			 0, 1, 2, 2, 3, 0,	//+x　右
			 4, 5, 6, 6, 7, 4,	//-x　左
			 8, 9,10,10,11, 8,	//+y　上
			12,13,14,14,15,12,	//-y  下
			16,17,18,18,19,16,	//+z　奥
			20,21,22,22,23,20,	//-z　手前
		};

		return CreateMesh(name, GL_TRIANGLES,
			vertices.data(), vertices.size(), indices, std::size(indices));
	}


	//球体メッシュを作成する
	//@param name     メッシュ名(検索などで使用)
	//@param segment  経度方向の分割数
	//@param ring     緯度方向の分割数
	 
	//@return 作成したメッシュへのポインタ
	StaticMeshPtr MeshBuffer::CreateSphere(const char* name,
		int segment, int ring)
	{
		//緯度経度方向の角度変化を取得
		const float angleLon = radians(360.0f / segment);	//Lon=Longitude
		const float angleLat = radians(180.0f / ring);		//Lat=Latitude
	
		//頂点データを取得
		std::vector<Vertex> vertices((segment + 1) * (ring + 1));
		auto v = vertices.begin();

		//XY平面で回転
		for (float lat = 0; lat < ring + 1; ++lat)
		{
			//球体の下側から計算をするため、-pi*0.5
			const float x = std::cos(angleLat * lat - pi * 0.5f);
			const float y = std::sin(angleLat * lat - pi * 0.5f);

			//XZ平面で回転
			for (float lon = 0; lon < segment + 1; ++lon)
			{
				v->position.x = x * std::cos(angleLon * lon);
				v->position.y = y;
				v->position.z = x * -std::sin(angleLon * lon);

				v->normal = v->position;	//半径が１の球なので、使いまわす
				v->tangent = vec4(v->normal.z, v->normal.y, -v->normal.x, 1);//XZ平面で90度回転した値
				v->texcoord.x = lon / segment;
				v->texcoord.y = lat / ring;

				++v;
			}
		}

		//インデックスデータを生成
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

	//カプセルメッシュを作成
	//@param name
	//@param length
	//@param segment
	//@param ring
	
	//@return 作成したメッシュへのポインタ
	StaticMeshPtr MeshBuffer::CreateCapsule(const char* name, float length,
		int segment, int ring)
	{
		//円柱部分の半径が１になるように、リング数を偶数にする
		ring = ((ring + 1) / 2) * 2;

		const float angleLon = radians(360.0f / segment);	//経度
		const float angleLat = radians(180.0f / ring);		//緯度

		//球体を下にずらした頂点データを作成
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
				v->texcoord.y = lat / (ring + ring * length);	//円柱の長さを考慮してテクスチャを設定
				v->position.y -= length * 0.5f;	//カプセルを下側に移動

				++v;
			}
		}

		//球体の上半分をコピーして、上方向にLengthだけずらす
		auto rend = vertices.begin() + (segment + 1) * (ring / 2);	//リバースエンド(終点)
		auto src = vertices.begin() + (segment + 1) * (ring + 1);	//ソース(元データ)
		auto dest = vertices.end();		//Destination(行先)

		const float texcoordOffsetY = (ring * length) / (ring + ring * length);

		//終点まで遡る
		while (src != rend)
		{
			//一個ずつデータをさかのぼっていく
			*(--dest) = *(--src);
			dest->position.y += length;
			dest->texcoord.y += texcoordOffsetY;
		}

		//インデックスデータを生成
		std::vector<uint16_t> indices(segment * (ring + 1) * 6);
		auto i = indices.begin();

		const uint16_t oneRingVertexCount = segment + 1;

		for (int lat = 0; lat < ring + 1; ++lat)
		{
			for (int lon = 0; lon < segment; ++lon)
			{
				const uint16_t base = lat * oneRingVertexCount + lon;

				//頂点を代入
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











