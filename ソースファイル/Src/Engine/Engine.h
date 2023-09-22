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


//先行宣言
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

//ゲームエンジン

class Engine
{
public:
	int Run();			//メインを動かす

	float SlowSpeed = 1;//ゲームオブジェクトからdeltaTimeをいじる

	//ゲッター軍団
	GLFWwindow* GetWindow() { return window; }
	std::mt19937& GetRandomGenerator() { return rg; }
	int GetMapLevel() { return MapLevel; }
	float GetblurStrength() { return blurStrength; }


	void SetMapLevel(int _MapLevel) { MapLevel = _MapLevel; }
	void SetblurStrength(float _strength) { blurStrength = _strength; }



	//3Dモデル配置データの読み込み
	GameObjectList LoadGameObjectMap(const char* filename,
		const VecMath::vec3& translate = VecMath::vec3(0),
		const VecMath::vec3& scale = VecMath::vec3(1));

	//次のシーンを設定する
	template<typename T>
	void SetNextScene() { nextScene = std::make_shared<T>(); }

	//シーン遷移エフェクト状態
	enum class FadeState
	{
		Fading,	//フェードイン・アウトを実行中
		Open,	//フェードイン完了
		Closed,	//フェードアウト完了
	};
	//シーン遷移エフェクトの状態を取得
	FadeState GetFadeState() const { return fadeState; }

	static constexpr float DefaultFadeTime = 1;

	//フェードイン・アウトの開始
	void StartFadeIn(float time = DefaultFadeTime);
	void StartFadeOut(float time = DefaultFadeTime);

	//フェードの仕方の画像を設定
	void SetFadeRule(std::string _rule)
	{
		texFadeRule = Texture::Create(_rule,
			GL_LINEAR, Texture::Usage::for3DLinear);
	}
	
	//フェードの色を変更
	void SetFadeColor(std::string _rule)
	{
		texFadeColor = Texture::Create(_rule,
			GL_LINEAR, Texture::Usage::for3DLinear);
	}

	//ゲームオブジェクトを作成する
	template<typename T>
	std::shared_ptr<T> Create(const std::string& name,
		const VecMath::vec3& position = { 0,0,0 })//ゲームオブジェクトリストに生成して、追加する
	{
		std::shared_ptr<T> p = std::make_shared<T>();
		p->engine = this;
		p->name = name;
		p->SetPos(position);
		gameObjectList.push_back(p);
		return p;
	}

	//ゲームオブジェクトを削除する
	void ClearGameObjectList() { gameObjectList.clear(); }

	//描画解像度
	VecMath::vec2 GetViewSize() const { return { 1280,720 }; }

	//ウィンドウサイズ
	VecMath::vec2 GetWindowSize() const { return { 1280,720 }; }

	//キーが押されていたらtrue,押されていなければfalse
	bool GetKey(int key)const
	{
		return glfwGetKey(window, key) == GLFW_PRESS;//ウィンドウアドレス、キー番号
	}

	//配置データの読み込み
	TileMap LoadTileMap(const char* filename);	//ファイルの読み込み

	size_t AddUILayer(const char* image, GLenum filterMode, size_t reserve);
	void ClearUILayers();         

	//UI用ゲームオブジェクトを作成する

	//@param layerNo 追加先のレイヤー番号
	//@param		 nameゲームオブジェクトの名前
	//@param		 xゲームオブジェクトを配置するX座標
	//@param		 yゲームオブジェクトを配置するY座標
	//@return 作成したUI用ゲームオブジェクト

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

	//メインカメラを取得
	GameObjectPtr GetMainCameraObject() const { return cameraObject; }

	std::shared_ptr<Camera> GetMainCamera() const {
		return std::static_pointer_cast<Camera>(cameraObject->componentList[0]);}

	//プルームの閾値の取得・設定
	float GetBloomThreshold() const { return bloomThreshold; }
	void SetBloomThreshold(float _t) { bloomThreshold = _t; }

	//プルームの強さの取得・設定
	float GetBloomStrength() const { return bloomStrength; }
	void SetBloomStrength(float _s) { bloomStrength = _s; }

	//パーティクルエミッタの操作
	ParticleEmitterPtr AddParticleEmitter(
		const ParticleEmitterParameter& ep, const ParticleParameter& pp);

	//パーティクルの検索
	ParticleEmitterPtr FindParticleEmitter(const std::string& name) const;

	//パーティクルの削除
	void RemoveParticleEmitter(const ParticleEmitterPtr&);
	void RemoveParticleEmitterAll();

	//ゲームオブジェクトリストから名前検索
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
	int Initialize();	//初期化
	int MainLoop();		//メインループ
	int Finalize();		//終了

	void UpdateGameObject(float deltaTime);	//アップデート
	void CollideGameObject();				//衝突判定
	void RemoveGameObject(GameObjectList& gameObjectList);										//ゲームオブジェクトを消去
	void MakeSpriteList(GameObjectList& gameObjectList,SpriteList& spriteList);					//スプライトリストを作る
	void DrawSpriteList(const SpriteList& spriteList, GLuint tex, size_t& spriteSsboOffset);	//スプライトリストの描画
	void DrawStaticMesh(GameObjectList& gameObjectList);	//メッシュ描画
	
	VecMath::mat4 CreateDepthMap(GameObjectList& gameObjectList,
		const VecMath::vec3& eye, const VecMath::vec3& target,
		float range, float zNear, float zFar);

	void DrawBloomEffect();//ブルームエフェクト描画用関数

	GLFWwindow* window = nullptr;				//ウィンドウオブジェクト
	const std::string title = "Last Trigger";	//ウィンドウタイトル

	GLuint vs = 0;					//頂点シェーダの管理番号
	GLuint fs = 0;					//フラグメントシェーダ管理番号
	BufferObjectPtr vbo;			//頂点バッファの管理番号
	BufferObjectPtr ibo;			//インデックスバッファの管理番号
	VertexArrayPtr  vao;			//構成情報の管理番号
	ProgramPipelinePtr progSprite;	//プログラムオブジェクトの管理番号

	int MapLevel = 0;
	const size_t maxSpriteCount = 100'000;		//スプライトの最大数


	MappedBufferObjectPtr ssboSprite;//マップしないbufferオブジェクト


	std::mt19937 rg;			//疑似乱数を生成するオブジェクト（速い）

	const size_t bgSizeX = 17;	//横のタイル数
	const size_t bgSizeY = 110;	//縦のタイル数

	float bgBaseX = 16;				//背景の配置開始ｘ座標
	float bgBaseY = 16;				//背景の配置開始y座標

	//3Dモデル用の変数
	ProgramPipelinePtr progStandard3D;
	Mesh::PrimitiveBufferPtr primitiveBuffer;
	GameObjectPtr cameraObject;
	Mesh::MeshBufferPtr meshBuffer;

	FramebufferObjectPtr fboGameWindow;	//ゲームウィンドウ用FBO

	ScenePtr scene;		//実行中のシーン
	ScenePtr nextScene;	//次のシーン
	GameObjectList gameObjectList;	//ゲームオブジェクト配列

	//3Dモデルの影描画用の変数
	FramebufferObjectPtr fboShadow;
	ProgramPipelinePtr progShadow;

	//スカイボックス用の変数
	TexturePtr texSkyBox;
	ProgramPipelinePtr progSkyBox;

	//エフェクト用の変数
	ProgramPipelinePtr progGaussianBlur;	//ぼかしシェーダ
	float blurStrength = 0;					//ぼかし強度(0 ~ 1)

	ProgramPipelinePtr progHighPassFilter;	//高輝度を抽出するシェーダー
	ProgramPipelinePtr progDownSampling;	//縮小シェーダ―
	ProgramPipelinePtr progUpSampling;		//拡大シェーダ―

	FramebufferObjectPtr fboBloom[6];		//ぼかしFBO

	float bloomThreshold = 1.0f;	//高輝度とみなす明るさ(0~上限無し)
	float bloomStrength = 4;		//ブルームの強さ

	//シーン遷移エフェクト用の変数
	ProgramPipelinePtr progFade;	//フェード用シェーダー
	TexturePtr texFadeColor;		//フェードのカラー画像
	TexturePtr texFadeRule;			//フェードのルール画像
	float FadeFrom = 0;				//フェードの開始の閾値
	float FadeTo = 1;				//フェードの終了時の閾値
	float FadeTotalTime = 1;		//フェードにかかる時間
	float FadeTimer = 0;			//フェードの経過時間

	FadeState fadeState = FadeState::Open;//エフェクト状態

	//UI用ゲームオブジェクトのレイヤー構造
	struct UILayer {
		TexturePtr tex;
		GameObjectList gameObjectList;
		std::vector<Sprite> spriteList;
	};
	std::vector<UILayer> uiLayerList;	//UIレイヤー配列

	//パーティクル管理
	ParticleManagerPtr particleManager;
};
#endif //ENGINE_H_INCLUDED