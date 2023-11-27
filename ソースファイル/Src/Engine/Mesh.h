//@file Mesh.h

#ifndef MESH_H_INCLUDED
#define MESH_H_INCLUDED

#include "glad/glad.h"
#include "VecMath.h"
#include "Texture.h"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

//先行宣言
class VertexArray;
using VertexArrayPtr = std::shared_ptr<VertexArray>;

//3Dモデル関係の機能を格納する名前空間

namespace Mesh
{
	//先行宣言
	class PrimitiveBuffer;
	using PrimitiveBufferPtr = std::shared_ptr<PrimitiveBuffer>;
	class MeshBuffer;
	using MeshBufferPtr = std::shared_ptr<MeshBuffer>;

	//頂点データ形式
	struct Vertex
	{
		VecMath::vec3 position; 	//頂点座標
		VecMath::vec2 texcoord;		//テクスチャ座標
		VecMath::vec3 normal;		//法線マップ
		VecMath::vec4 tangent;		//タンジェントベクトル
	};

	//マテリアル
	struct Material
	{
		std::string name = "<Default>";				//マテリアル名
		VecMath::vec4 baseColor = VecMath::vec4(1);	//基本色＋アルファ
		float specularPower = 16;					//鏡面反射指数
		float normalizeFactor = 24.0f / 25.13274f;	//正規化係数
		bool receiveShadows = true;
		TexturePtr texBaseColor;					//基本色テクスチャ
		TexturePtr texNormal;						//法線テクスチャ
	};
	using MaterialPtr = std::shared_ptr<Material>;

	//プリミティブ
	struct Primitive
	{
		GLenum mode = GL_TRIANGLES;	//描画モード
		GLsizei count = 0;			//描画するインデクス数
		const void* indices = 0;	//描画開始インデックスのバイトオフセット
		GLint baseVertex = 0;		//インデックス０番とみなされる頂点配列の位置
		int materialNo = -1;		//マテリアル
	};

	void Draw(const Primitive& prim);


	//プリミティブを管理するクラス
	class PrimitiveBuffer
	{
	public:
		//プリミティブバッファを作成する
		static PrimitiveBufferPtr Create(size_t vboCapacity, size_t iboCapacity)
		{
			return std::make_shared<PrimitiveBuffer>(vboCapacity, iboCapacity);
		}

		//コンストラクタ
		PrimitiveBuffer(size_t vboCapacity, size_t iboCapacity);

		//デストラクタ
		~PrimitiveBuffer();

		//コピーと代入の禁止
		PrimitiveBuffer(const PrimitiveBuffer&) = delete;
		PrimitiveBuffer& operator=(const PrimitiveBuffer&) = delete;

		//プリミティブの追加
		void AddPrimitive(const Vertex* vertices, size_t vertexCount,
			const uint16_t* indices, size_t indexCount, GLenum mode = GL_TRIANGLES);

		//プリミティブの取得
		const Primitive& GetPrimitive(size_t index)const { return primitives[index]; }

		//VAOの取得
		VertexArrayPtr GetVAO() const { return vao; }

		//頂点データ、インデックスデータの追加
		GLint AddVertices(const Vertex* vertices, size_t count);
		const void* AddIndices(const uint16_t* indices, size_t count);

		//すべてのプリミティブを削除
		void Clear();

	private:
		std::vector<Primitive> primitives;
		VertexArrayPtr vao;

		GLuint vbo = 0;				//vbo管理番号
		uint8_t* pVbo = nullptr;	//vboの書き込み先アドレス
		size_t vboCapacity = 0;		//vboの全容量（バイト）
		size_t vboSize = 0;			//VBOの使用済み容量（バイト）

		GLuint ibo = 0;				//ibo管理番号
		uint8_t* pIbo = nullptr;	//iboの書き込み先アドレス
		size_t iboCapacity = 0;		//iboの全容量（バイト）
		size_t iboSize = 0;			//iBOの使用済み容量（バイト）
	};


	//3Dモデル
	struct StaticMesh
	{
		std::string name;	//メッシュ名
		std::vector<Primitive> primitives;
		std::vector<MaterialPtr> materials;
		VertexArrayPtr vao;
	};

	using StaticMeshPtr = std::shared_ptr<StaticMesh>;
	void Draw(const StaticMesh& mesh,
		const std::vector<MaterialPtr>& materials, GLuint program = 0);

	//メッシュを管理するクラス
	class MeshBuffer
	{
	public:
		//メッシュバッファを作成する
		static MeshBufferPtr Create(size_t vboCapacity, size_t iboCapacity)
		{
			return std::make_shared<MeshBuffer>(vboCapacity, iboCapacity);
		}

		//コンストラクタ
		MeshBuffer(size_t vboCapacity, size_t iboCapacity);

		//デストラクタ
		~MeshBuffer() = default;

		//ファイルを読み込んでStaticMeshを返す
		StaticMeshPtr LoadOBJ(const char* filename);

		//
		VertexArrayPtr GetVAO() const;

		//すべてのStaticMeshを削除
		void Clear();

		//頂点データとインデックスデータを受け取ってメッシュを作成
		StaticMeshPtr CreateMesh(
			const char* name, GLenum mode,
			const Vertex* vertices, size_t vetexCount,
			const uint16_t* indices, size_t indexCount);

		//直方体の頂点データとインデックスデータを作成し、CreateMeshに渡す
		StaticMeshPtr CreateBox(const char* name);

		//球体の頂点データとインデックスデータを作成し、CreateMeshに渡す
		StaticMeshPtr CreateSphere(const char* name,
			int segment = 12, int ring = 6);

		//カプセルの頂点データとインデックスデータを作成し、CreateMeshに渡す
		StaticMeshPtr CreateCapsule(const char* name, float length,
			int segment = 12, int ring = 6);
	private:
		std::vector<MaterialPtr> LoadMTL(
			const std::string& foldername, const char* filename);

		PrimitiveBuffer primitiveBuffer;
		std::unordered_map<std::string, StaticMeshPtr> meshes;
		std::shared_ptr<class  MikkTSpace> mikkTSpace;
		MaterialPtr defaultMaterial;	//マテリアル未設定時に使用する仮マテリアル
	};
}



#endif //MESH_H_INCLUDED