//@file Particle.c

#include "Particle.h"
#include "../Engine/Debug.h"
#include <algorithm>

using namespace VecMath;

//頂点データ構造体
struct ParticleVertex
{
	float x, y;	//座標(x,y)
	float u, v;	//テクスチャ座標
};

//画像表示用データっを格納する構造体(float*16個=64バイト)
struct ParticleData
{
	vec4 Position;		//座標(wは未使用)
	vec2 Scale;			//拡大縮小
	float Cos;			//z軸回転のCos
	float Sin;			//z軸回転のSin
	vec4 Color;			//色と透明度
	Texcoord texcoord;	//テクスチャ座標
};

//コンストラクタ
//@param ep　　　　エミッターの初期化パラメータ
//@param pp　　　　パーティクルの初期化パラメータ
//@param pManager 管理オブジェクトのアドレス

ParticleEmitter::ParticleEmitter(const ParticleEmitterParameter& ep,
	const ParticleParameter& pp, ParticleManager* pManager)
	:ep(ep), pp(pp), pManager(pManager), Interval(1.0f / ep.EmissionsPerSecond)
{
	//テクスチャの作成
	texture = Texture::Create(ep.ImagePath.c_str(), GL_LINEAR,
		Texture::Usage::for2D);
}

//エミッターの管理下にあるパーティクルの状態を更新する
//@param matView   描画に使用するビュー行列
//@param deltaTime　前回の更新からの経過時間（秒）

void ParticleEmitter::Update(const mat4& matView, float deltaTime)
{
	//管理オブジェクトが未設定なら何もしない
    if (!pManager)
	{
		return;
	}

	//時間を経過させる
	Timer += deltaTime;

	//放出時間を過ぎたら
	if (Timer >= ep.Duration)
	{
		//ループをさせる
		if (ep.Loop)
		{
			//時間をリセット
			Timer -= ep.Duration;
			EmissionTimer -= ep.Duration;
		}
		else
		{
			//終り
			Timer = ep.Duration;
		}
	}

	//ソートに使う「視点からの深度地」を計算
	ViewDepth = (matView * vec4(ep.position, 1)).z;

	//パーティクルの放出
	for (; Timer - EmissionTimer >= Interval; EmissionTimer += Interval)
	{
		//パーティクルを作成
		AddParticle();
	}

	//パーティクルの更新
	for (auto e : Particles)
	{
		//死んでいたら次の人へ
		if (e->LifeTime <= 0)
		{
			continue;
		}

		//生存時間を経過させる
		e->LifeTime -= deltaTime;

		//速度を計算
		const float t = e->GetLifeTimeRatio();		//生存時間の割合
		vec3 velocity = e->velocity.Interporate(t);	//イージングで入力

		//半径方向の速度を計算
		velocity += e->RadialDirection * e->pp->radial.Interporate(t);

		//重力による加速度と速度を更新
		e->GravityAccel += ep.Gravity * deltaTime;		//加速度
		e->GravitySpeed += e->GravityAccel * deltaTime;	//速度
		velocity.y += e->GravitySpeed;

		//座標を更新
		e->position += velocity * deltaTime;

		//ソートに使う「視点からの深度地」を計算
		e->ViewDepth = (matView * vec4(e->position, 1)).z;
	}

	//死んでいるパーティクルを削除
	//先頭に生きている人を集める
	auto itr = std::partition(Particles.begin(), Particles.end(),
		[](const Particle* p) {return !p->IsDead(); });

	for (auto i = itr; i != Particles.end(); ++i)
	{
		//パーティクルを返却
		pManager->DeallocateParticle(*i);
	}
	Particles.erase(itr, Particles.end());

	//パーティクルをソート
	std::sort(Particles.begin(), Particles.end(),
		[](const Particle* a, const Particle* b) {return a->ViewDepth< b->ViewDepth; });
}

//パーティクルを追加
void ParticleEmitter::AddParticle()
{
	//パーティクルを確保できなければ何もしない
	Particle* particle = pManager->AllocateParticle();

	if (!particle)
	{
		return;
	}

	//ローカル放出座標を計算
	vec4 pos = { 0,0,0,1 };

	switch (ep.Shape)
	{
	case ParticleEmitterShape::sphere:
	{
		//cos　sin　πを求める
		const float cosTheta = pManager->RandomFloat(-1, 1);
		const float sinTheta = std::sqrt(1 - cosTheta * cosTheta);
		const float phi = pManager->RandomFloat(0, 2 * pi);

		//球の体積の公式4πr^3/3から、半径ごとの放出頻度は半径の3乗根で求められる
		//thicknessを3乗することで3乗根を取った時最小値がThickと一致する
		const float t = ep.sphere.Thickness;
		const float r0 = pManager->RandomFloat(1 - t * t * t, 1);
		const float r = std::cbrt(r0) * ep.sphere.Radius;//半径の3乗

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

	//Z->X->Yの順で回転を適用し、放出方向へ向ける行列を計算
	mat4 matRot = mat4::RotateY(ep.rotation.y) *
		mat4::RotateX(ep.rotation.x) * mat4::RotateZ(ep.rotation.z);

	//パーティクルのメンバ変数を初期化
	particle->pp = &pp;
	particle->LifeTime = pp.LifeTime;
	particle->position = ep.position + vec3(matRot * pos);
	particle->RadialDirection = normalize(particle->position - ep.position);
	particle->GravityAccel = 0;
	particle->GravitySpeed = 0;
	particle->ViewDepth = 0;

	//方向がランダム係数が０でなければ、移動方向にランダムを加える
	if (ep.RandomizeDirection)
	{
		//移動方向をランダム化する行列を計算
		const float RandmRamge = pi * ep.RandomizeDirection;

		const mat4 matRandom =
			mat4::RotateY(pManager->RandomFloat(-RandmRamge, RandmRamge)) *
			mat4::RotateX(pManager->RandomFloat(-RandmRamge, RandmRamge)) *
			mat4::RotateZ(pManager->RandomFloat(-RandmRamge, RandmRamge));

		//回転とランダムを合成
		matRot *= matRandom;

		//半径方向をランダム化
		particle->RadialDirection = vec3(matRandom * vec4(particle->RadialDirection, 1));
	}

	//速度ベクトルを回転
	particle->velocity.start = vec3(matRot * vec4(pp.velocity.start, 1));
	particle->velocity.end = vec3(matRot * vec4(pp.velocity.end, 1));

	//回転角度のランダム係数が０でなければ、回転角度にランダムを加える
	particle->rotation = pp.rotation;

	if (ep.RandomizeRotation)
	{
		const float randomRange = pi * ep.RandomizeRotation;
		const float a = pManager->RandomFloat(-randomRange, randomRange);
		particle->rotation.start += a;
		particle->rotation.end += a;
	}

	//拡縮のランダム係数が０でなければ、大きさにランダムを加える
	particle->scale = pp.scale;

	if (ep.RandomizeSize)
	{
		const float randomSize = pi * ep.RandomizeSize;
		const float a = pManager->RandomFloat(-randomSize, randomSize);
		particle->scale.start += a;
		particle->scale.end += a;
	}


	//タイル数が１より多い場合、ランダムにタイルを選択する
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

	//エミッターの管理リストにパーティクルを追加
	Particles.push_back(particle);
}


//パーティクル管理オブジェクトを作成する
//@param maxParticleCount 表示可能なパーティクルの最大数
ParticleManagerPtr ParticleManager::Create(size_t maxParticleCount)
{
	return std::make_shared<ParticleManager>(maxParticleCount);
}

//コンストラクタ
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

	//vboの作成
	vbo = BufferObject::Create(sizeof(vertexData), vertexData);

	//iboの作成
	const GLushort indexData[] = { 0,1,2,2,3,0 };
	ibo = BufferObject::Create(sizeof(indexData), indexData);

	//vboの作成
	vao = VertexArray::Create();

	glBindVertexArray(*vao);
	glBindBuffer(GL_ARRAY_BUFFER, *vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ibo);

	//vertに変数を渡す
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), 0);

	//vertに変数を渡す
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex),
		(const void*)offsetof(ParticleVertex, u));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	program = ProgramPipeline::Create("Res/particle.vert", "Res/particle.frag");
	ssbo = MappedBufferObject::Create(maxParticleCount * sizeof(ParticleData),
		GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);

	//必要なメモリ領域を確保
	Emitters.reserve(100);
	Particles.resize(maxParticleCount);

	//未使用パーティクルリストを作成
	FreeParticles.resize(maxParticleCount);
	auto itr = Particles.end();

	for (auto& e : FreeParticles)
	{
		--itr;
		e = &*itr;
	}

	//乱数を初期化
	std::random_device rd;
	RandomEngine.seed(rd());
}

//デストラクタ
ParticleManager::~ParticleManager()
{
	//すべてのエミッターからマネージャのアドレスを消去する
	for (auto& e : Emitters)
	{
		e->pManager = nullptr;
	}
}

//パーティクルの状態を更新する
//@param   matView  描画に使用するビュー行列 
//@param deltaTime  前回の更新からの経過時間(秒)
void ParticleManager::Update(const mat4& matView, float deltaTime)
{
	if (Emitters.empty())
	{
		return;
	}

	//エミッターの更新
	for (auto& e : Emitters)
	{
		e->Update(matView, deltaTime);
	}

	//ここから下は大体パーティクルと一緒

	//死んでいるエミッターを削除　
	auto itr = std::stable_partition(Emitters.begin(), Emitters.end(),
		[](const ParticleEmitterPtr& e) {return !e->IsDead(); });

	for (auto i = itr; i != Emitters.end(); ++i)
	{
		(*i)->pManager = nullptr;
	}
	Emitters.erase(itr, Emitters.end());

	//エミッターをソート
	std::stable_sort(Emitters.begin(), Emitters.end(),
		[](const ParticleEmitterPtr& a, const ParticleEmitterPtr& b)
		{return a->ViewDepth < b->ViewDepth; });
}

//SSBOの内容を更新する
void ParticleManager::UpdateSSBO()
{
	ssbo->WaitSync();

	//視点座標系で奥にあるエミッターからデータを設定
	auto* begin = reinterpret_cast<ParticleData*>(ssbo->GetMappedAddress());
	auto* pData = begin;

	for (auto& e : Emitters)
	{
		//エミッターの描画用のデータ変数を更新
		e->DataOffset = (pData - begin) * sizeof(ParticleData);		//データの位置
		e->DataCount = static_cast<GLsizei>(e->Particles.size());	//データの数
	
		//SSBOにデータを設定
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

		//次のオフセットが256バイト境界になるように調整
		pData += e->DataCount % 4;//64バイトなので４個で256バイト
	}
}

//パーティクルを描画する
//@param matProj 描画に使用するプロジェクション行列
//@param matView 描画に使用するビュー行列
void ParticleManager::Draw(const mat4& matProj, const mat4& matView, const VecMath::mat3& matBillboard)
{
	//空なら無視
	if (Emitters.empty())
	{
		return;
	}

	//深度テスト有効、深度バッファへの書き込みは行わないように設定
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	glBindVertexArray(*vao);
	glUseProgram(*program);

	//シェーダーにビュープロジェクション行列を設定
	const GLint locMatVP = 1;
	const mat4 matVP = matProj * matView;

	glProgramUniformMatrix4fv(*program,
		locMatVP, 1, GL_FALSE, &matVP[0][0]);

	//シェーダーにビルボード秒列を設定(常にカメラの方向を向くプリミティブのこと)
	const GLint locMatBilboard = 2;
	//const mat3 matBillboard2 = inverse(transpose(inverse(mat3(matView))));

	glProgramUniformMatrix3fv(*program,
		locMatBilboard, 1, GL_FALSE, &matBillboard[0][0]);

	//パーティクルを描画
	for(auto & e: Emitters)
	{
		//描画をしないなら無視
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

	//ブレンド設定、深度バッファへの書き込みをデフォルトに戻す
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_TRUE);	//深度バッファへの書き込みを許可
}

//エミッターを追加する
//@param ep エミッターの初期化パラメータ
//@param pp パーティクルの初期化パラメータ

//@return 追加したエミッター
ParticleEmitterPtr ParticleManager::AddEmitter(const ParticleEmitterParameter& ep,
	const ParticleParameter& pp)
{
	//ポインターを作成し、エミッターに追加する
	ParticleEmitterPtr p = std::make_shared<ParticleEmitter>(ep, pp, this);
	Emitters.push_back(p);

	return p;
}

//指定された名前を持つエミッターを検索する
//@param name 検索する名前

//@return nameと一致する名前を持つエミッターへのポインタ
ParticleEmitterPtr ParticleManager::FindEmitter(const std::string& name)const
{
	//エミッターの検索
	auto itr = std::find_if(Emitters.begin(), Emitters.end(),
		[name](const ParticleEmitterPtr& p)
		{return p->ep.name == name; });

	if (itr != Emitters.end())
	{
		return *itr;
	}

	return nullptr;
}

//指定されたエミッターを削除する
void ParticleManager::RemoveEmitter(const ParticleEmitterPtr& p)
{
	auto itr = std::find(Emitters.begin(), Emitters.end(), p);

	if (itr != Emitters.end())
	{
		(*itr)->pManager = nullptr;			//管理オブジェクトへのポインタを削除
		std::swap(*itr, Emitters.back());	//けつと入れ替える
		Emitters.pop_back();				//けつを消す
	}
}

//すべてのエミッターを削除する
void ParticleManager::RemoveEmitterAll()
{
	for (auto& e : Emitters)
	{
		e->pManager = nullptr;//管理オブジェクトへのポインタを削除
	}

	//全消しだドン
	Emitters.clear();
}

//パーティクルを確保する
//@retval nullptr      パーティクルの確保に失敗
//@retval nullptr以外  確保したパーティクルのアドレス

Particle* ParticleManager::AllocateParticle()
{
	//空っぽなら
	if (FreeParticles.empty())
	{
		return nullptr;
	}

	auto p = FreeParticles.back();	//けつを代入
	FreeParticles.pop_back();		//けつを消す
	return p;
}

//パーティクルを解放する
//@param p 解放するパーティクルのアドレス
void ParticleManager::DeallocateParticle(Particle* p)
{
	//ｐが範囲外ならエラー
	if (p<&Particles.front() || p>&Particles.back())
	{
		LOG_ERROR("管理範囲外のアドレスです(p=%x,範囲%x-%x)",
			p, &Particles.front(), &Particles.back());

		return;
	}

	FreeParticles.push_back(p);
}