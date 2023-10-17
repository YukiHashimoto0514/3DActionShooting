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

//先行宣言
class Particle;
class ParticleEmitter;
class ParticleManager;

using ParticleEmitterPtr = std::shared_ptr<ParticleEmitter>;
using ParticleManagerPtr = std::shared_ptr<ParticleManager>;


//値の補間方法
enum class InterporationType
{
	Linear,		//線形補間
	WaseIn,		//徐々に加速
	EaseOut,	//徐々に減速
	EaseInOut,	//徐々に加速し、徐々に減速する
	None,		//補間しない
};

//値を補間する機能を持つクラス
template<typename T>
class Interporator
{
public:
	Interporator() = default;	//コンストラクタ
	~Interporator() = default;	//デストラクタ

	explicit Interporator(const T& start) : start(start), end(start) {}
	Interporator(const T& start, const T& end) : start(start), end(end) {}

	//値を設定する
	void Set(const T& start, const T& end)
	{
		this->start = start;
		this->end = end;
	}

	//引数t(0.0~1.0)で補間した値を返す(後で追加予定)
	T Interporate(float t)const
	{
		switch (type)
		{
		default:
		case InterporationType::Linear: return start + t * (end - start);
		case InterporationType::None:  return start;
		}
	}

	T start;	//補間開始時の値
	T end;		//補間終了時の値

	InterporationType type = InterporationType::Linear;//補間方法

};

using Interporator1f = Interporator<float>;
using Interporator2f = Interporator<VecMath::vec2>;
using Interporator3f = Interporator<VecMath::vec3>;
using Interporator4f = Interporator<VecMath::vec4>;


//パーティクルの動作を定義するパラメータ
struct ParticleParameter
{
	float LifeTime = 1;//生存時間
	Texcoord tc = { 0,0,1,1 };//テクスチャ座標
	Interporator1f radial = Interporator1f(10);				//半径方向の速度(エミッターの中心から放射状に放出したい場合に設定)
	Interporator3f velocity = Interporator3f({ 0,0,0 });	//速度(特定の向きに放出したい場合に設定)
	Interporator2f scale = Interporator2f({ 1,1 });			//大きさ
	Interporator1f rotation = Interporator1f(0);			//回転
	Interporator4f color = Interporator4f({ 1,1,1,1 });		//色と不透明度
};

//エミッターの形状
enum class ParticleEmitterShape
{
	sphere,	//球
	box,	//直方体
};

//パーティクル・エミッターのパラメーター
struct ParticleEmitterParameter
{

	//コンストラクタ
	ParticleEmitterParameter() : sphere({ 1,1 }){}

	//デストラクタ
	~ParticleEmitterParameter() = default;

	//放出範囲に球を指定する
	void SetSphereShape(float radius, float thickness)
	{
		Shape = ParticleEmitterShape::sphere;
		sphere.Radius = radius;
		sphere.Thickness = thickness;
	}

	//放出範囲に直方体を指定する
	void SetBoxShape(const VecMath::vec3& size)
	{
		Shape = ParticleEmitterShape::box;
		box.size = size;
	}

	
	std::string name;		//エミッター名
	float Gravity = -9.8f;	//重力

	//画像とブレンド係数
	std::string ImagePath;						//テクスチャ・ファイル名
	VecMath::vec2 tiles = { 1,1 };				//テクスチャの縦と横の分割数
	GLenum srcFactor = GL_SRC_ALPHA;			//ブレンド係数
	GLenum dstFactor = GL_ONE_MINUS_SRC_ALPHA;	//ブレンド係数

	//放出時間と放出頻度
	float Duration = 1.0f;			//パーティクルを放出する秒数
	bool Loop = false;				//Duration経過後の動作(true=ループ　false=停止)
	float EmissionsPerSecond = 10;	//秒あたりのパーティクル放出数

	//放出の形状
	VecMath::vec3 position = { 0,0,0 };			//座標
	VecMath::vec3 rotation = { 0,0,0 };			//回転

	ParticleEmitterShape Shape = ParticleEmitterShape::sphere;	//形状
	float RandomizeDirection = 0;		//放出方向の乱数性(0=なし　1=中心まできっしり)
	float RandomizeRotation = 0;		//回転角度の乱数性(0=なし　1=中心まできっしり)
	float RandomizeSize = 0;		    //拡縮の乱数性(0=なし　1=中心まできっしり)

	union 
	{
		struct Sphere
		{
			float Radius = 1;//球の半径
			float Thickness = 1;//球殻の厚み(0=最薄　1=中心までぎっしり)
		}sphere;

		struct Box
		{
			VecMath::vec3 size = { 1,1,1 };//直方体のサイズ
		};
		Box box = { VecMath::vec3(1) };
	};
}; 

//パーティクル
class Particle
{
public:
	friend ParticleManager;
	friend ParticleEmitter;

	Particle() = default;	//コンストラクタ
	~Particle() = default;	//デストラクタ

	//生存期間の比率
	float GetLifeTimeRatio()const
	{
		const float t = 1 - LifeTime / pp->LifeTime;

		//0~1の値に収める
		return std::clamp(t, 0.0f, 1.0f);
	}

	//true=死亡　false=生存
	bool IsDead()const 
	{
		return LifeTime <= 0;
	}


private:
	//変化しないパラメータ
	const ParticleParameter* pp = 0;	//基本パラメータ
	VecMath::vec3 RadialDirection = { 1,0,0 };	//中心からの向き
	Interporator3f velocity = Interporator3f({ 0,0,0 });	//速度
	Interporator1f rotation = Interporator1f(0, 0);			//回転
	Interporator2f scale = Interporator2f({ 1,1 });			//大きさ

	Texcoord tc = { 0,0,1,1 };	//テクスチャ座標

	//変化するパラメータ
	float LifeTime = 0;		//生存時間
	VecMath::vec3 position = { 0,0,0 };	//座標
	float GravityAccel = 0;	//重力による加速度の合計
	float GravitySpeed = 0;	//重力による速度の合計
	float ViewDepth = 0;	//視点からの深度
};

//パーティクル放出クラス
class ParticleEmitter
{
public:
	friend ParticleManager;

	//コンストラクタ
	ParticleEmitter(const ParticleEmitterParameter& ep,
		const ParticleParameter& pp, ParticleManager* pManager);

	//デストラクタ
	~ParticleEmitter() = default;

	//コピーと代入の禁止
	ParticleEmitter(const ParticleEmitter&) = delete;
	ParticleEmitter& operator=(const ParticleEmitter&) = delete;

	//更新
	void Update(const VecMath::mat4& matView, float deltaTime);

	//パーティクルを消す
	void Die() { ep.Loop = false; ep.Duration = 0; }

	//生死判定
	bool IsDead()const { return !ep.Loop && Timer >= ep.Duration && Particles.empty(); }

	//パラメーターの取得・設定
	ParticleEmitterParameter& GetEmitterParameter() { return ep; }
	const ParticleEmitterParameter& GetEmitterParameter()const { return ep; }

	ParticleParameter& GetParticleParameter() { return pp; }
	const ParticleParameter& GetParticleParameter()const  { return pp; }


private:
	void AddParticle();

	ParticleParameter pp;				//パーティクルのパラメーター
	ParticleEmitterParameter ep;		//エミッターのパラメーター

	//sharedにすると、循環参照になってしまうから
	ParticleManager* pManager = nullptr;//管理クラスのアドレス

	TexturePtr texture;			//パーティクル用のテクスチャ
	float Interval = 0;			//パーティクルの発生間隔(秒)
	float Timer = 0;			//経過時間(秒)
	float EmissionTimer = 0;	//パーティクル発生タイマー(秒)
	float ViewDepth = 0;		//視点からの深度値

	std::vector<Particle*> Particles;	//パーティクルリスト
	GLsizei DataCount = 0;				//描画するパーティクル数
	size_t DataOffset = 0;				//描画の基準となる頂点のオフセット

};

//パーティクル管理クラス
class ParticleManager
{
public:
	static ParticleManagerPtr Create(size_t maxParticleCount);

	//コンストラクタ
	explicit ParticleManager(size_t maxParticleCount);

	//デストラクタ
	~ParticleManager();

	//コピー代入の禁止
	ParticleManager(const ParticleManager&) = delete;
	ParticleManager& operator=(const ParticleManager&) = delete;

	void Update(const VecMath::mat4& matView, float deltatime);
	void UpdateSSBO();
	void Draw(const VecMath::mat4& matProj, const VecMath::mat4& matView, const VecMath::mat3& matBillboard);

	//エミッターの管理
	ParticleEmitterPtr AddEmitter(
		const ParticleEmitterParameter& ep, const ParticleParameter& pp);
	
	ParticleEmitterPtr FindEmitter(const std::string& name)const;	//エミッターの検索
	void RemoveEmitter(const ParticleEmitterPtr&);					//エミッターの削除
	void RemoveEmitterAll();	//エミッターの全削除

	//パーティクルの管理
	Particle* AllocateParticle();
	void DeallocateParticle(Particle* p);

	//乱数の生成
	int RandomInt(int min, int max)
	{
		return std::uniform_int_distribution<>(min, max)(RandomEngine);
	}

	float RandomFloat(float min, float max)
	{
		return std::uniform_real_distribution<float>(min, max)(RandomEngine);
	}


private:
	std::vector<ParticleEmitterPtr> Emitters;	//エミッターのリスト
	std::vector<Particle> Particles;			//パーティクルのリスト
	std::vector<Particle*> FreeParticles;		//未使用パーティクルのリスト

	size_t MaxParticleCount = 0; //最大パーティクル数
	BufferObjectPtr vbo;
	BufferObjectPtr ibo;
	VertexArrayPtr vao;

	ProgramPipelinePtr program;
	MappedBufferObjectPtr ssbo;
	std::mt19937 RandomEngine;
};

#endif//PARTICLE_H_INCLUDED