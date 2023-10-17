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

//ウィンドウズの定義を未定義に
#undef far
#undef near

//minwinderのmin関数を使いたくない
#undef min
#undef max
#include <fstream>
#include <filesystem>

//#include "../ImGui/imgui.h"
//#include "../ImGui/imgui_impl_glfw.h"
//#include "../ImGui/imgui_impl_opengl3.h"


using namespace VecMath;

//ワールド座標系の衝突判定を表す構造体
struct WorldCollider
{
	//ゲームオブジェクトと衝突判定の座標を変更する
	void AddPosition(const vec3& v)
	{
		//ゲームオブジェクトの位置を変更
		gameObject->AddPosition(v);

		for (auto& e : colliderList)
		{
			//コライダー関数のAddPositinを呼ぶ
			e->AddPosition(v);
		}
	}

	//他のワールドコライダーとの衝突判定を行う
	std::vector<Collision::ContactPoint> DetectCollision(WorldCollider& other)
	{
		std::vector<Collision::ContactPoint> cpList;

		for (auto& ca : colliderList)
		{
			for (auto& cb : other.colliderList)
			{
				Collision::ContactPoint cp;

				//動かない物体同士は判定しない
				if (!this->gameObject->GetMovable() && !other.gameObject->GetMovable())
				{
					continue;
				}

				//衝突していない
				if (!Intersect(*ca, *cb, cp))
				{
					//次の処理へ
					continue;
				}


				//衝突した
				cpList.push_back(cp);

				//座標を戻す
				const vec3 v = cp.Normal * cp.Penetration * 0.5f;

				//動かない物体と動く物体の判定
				if (other.gameObject->GetMovable() && !this->gameObject->GetMovable())
				{
					//動かない物体となので２倍跳ね返す
					other.AddPosition(v * 2);
				}
				//動かない物体と動く物体の判定
				else if (!other.gameObject->GetMovable() && this->gameObject->GetMovable())
				{
					//動かない物体となので２倍跳ね返す
					AddPosition(-v * 2);
				}
				//動く物体と動く物体の判定
				else if (other.gameObject->GetMovable() && this->gameObject->GetMovable())
				{
					other.AddPosition(v);
					AddPosition(-v);
				}
				
				//法線で衝突した角度を計算
				float theta = dot(cp.Normal, vec3(0, 1, 0));
				
				//大体垂直だったら
				if (theta <= -0.9f)
				{
					
					gameObject->only = 4;		//地面判定にするフレーム数
					other.gameObject->only = 4;	//地面判定にするフレーム数
					other.gameObject->SetJumpFlg(false);//ジャンプできるようにする
					gameObject->SetJumpFlg(false);		//ジャンプできるようにする
				}

			}
		}


		return cpList;
	}

	GameObject* gameObject;	//コライダーを所有するゲームオブジェクト
	std::vector<ColliderPtr> colliderList;	//コライダー配列
};

//OpenGL空のメッセージを処理するコールバック関数
// 
// @param source			メッセージの配信者(OpenGL,Windows,シェーダーなど)
// @param type				メッセージの種類(エラー警告など)
// @param id				メッセージを一位に識別する値
// @param severity			メッセージの重要度(高、中、低、最低)
// @param length			メッセージの文字数. 負数ならメッセージは０終端されている
// @param message			メッセージ本体
// @param userParam			コールバック設定時に指定したポインタ

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
	OutputDebugString(s.c_str());//(出力する文字列)
}

//SSBOのサイズを計算する
size_t CalcSsboSize(size_t n)//ｎ以上の２５６の倍数の値
{
	const size_t alignment = 256;//アライメント（データの境界）
	return ((n + alignment - 1) / alignment) * alignment;
};



//エンジンを実行する
// 	   @retval 0 正常に実行が完了した(retval=戻り値)
// 	   @retval 0以外　エラーが発生した

int Engine::Run()
{
	int ret = Initialize();//初期化
	if (ret == 0)
	{
		ret = MainLoop();//メインループ
	}
	Finalize();//終了
	return ret;
}

//配置データを読み込む

TileMap Engine::LoadTileMap(const char* filename)
{
	//ファイルを開く
	std::ifstream file;
	file.open(filename);
	if (!file)
	{
		return { };	//失敗（ファイル名が違う？）
	}

	//読み込み先となる変数を準備
	TileMap tileMap;
	tileMap.data.reserve(bgSizeX * bgSizeY);	//適当な容量を予約

	//ファイルを読み込む
	while (!file.eof())
	{
		//一行読み込み
		std::string line;
		std::getline(file, line);	//ファイルから読み込み

		if (line.size() <= 0)
		{
			continue;	//データの無い行は無視
		}
		line.push_back(',');	//終端を追加

		//カンマ区切り文字列を数値に変換
		tileMap.sizeX = 0;
		const char* start = line.data();
		const char* end = start + line.size();

		for (const char* p = start; p != end; ++p)
		{
			if (*p == ',')
			{
				const int n = atoi(start);	//文字列を数字に
				tileMap.data.push_back(n);
				start = p + 1;				//次のカンマ区切りを目指すように更新
				++tileMap.sizeX;			//横のタイル数を更新
			}
		}
		++tileMap.sizeY;//縦のタイルを更新
	}

	//読み込んだ配置データを返す
	return tileMap;
}


//3Dモデル配置ファイルを読み込む

//@param filename   3Dモデル配置ファイル
//@param translate	すべての3Dモデル座標に加算する平行移動量
//@param scale		すべての3Dモデルに掛ける拡大率
//@return ファイルから読み込んだゲームオブジェクトの配列

GameObjectList Engine::LoadGameObjectMap(const char* filename,
	const vec3& translate, const vec3& scale)
{
	//ファイルを開く
	std::ifstream ifs(filename);

	if (!ifs)
	{
		LOG_WARNING("%sを開けません", filename);
		return{};
	}

	GameObjectList gameObjectList;
	gameObjectList.reserve(400);//適当な数を予約

	//行単位で読み込む
	while (!ifs.eof())
	{
		std::string line;
		std::getline(ifs, line);

		const char* p = line.c_str();

		int readByte = 0;

		//データ行の開始判定
		char name[256];

					  //生文字リテラル
		if (sscanf(p, R"( { "name" : "%255[^"]" %n)", name, &readByte) != 1)
		{
			continue;	//データ行ではない
		}
		name[255] = '\0';
		p += readByte;	//読み取り位置を更新

		//名前以外の要素を読み込む
		char baseMeshName[256];

		if (sscanf(p, R"(, "mesh" : "%255[^"]" %n)", baseMeshName, &readByte) != 1)
		{
			continue;
		}
		baseMeshName[255] = '\0';
		p += readByte;	//読み取り位置を更新

		vec3 t(0);
		if (sscanf(p, R"(, "translate" : [ %f, %f, %f ] %n)",
			&t.x, &t.y, &t.z, &readByte) != 3)
		{
			continue;
		}
		p += readByte;	//読み取り位置を更新

		vec3 r(0);
		if (sscanf(p, R"(, "rotate" : [ %f, %f, %f ] %n)",
			&r.x, &r.y, &r.z, &readByte) != 3)
		{
			continue;
		}

		p += readByte;	//読み取り位置を更新

		vec3 s(1);
		if (sscanf(p, R"(, "scale" : [ %f, %f, %f ] %n)",
			&s.x, &s.y, &s.z, &readByte) != 3)
		{
			continue;
		}

		p += readByte;	//読み取り位置を更新
		
		char tag[256];
		if (sscanf(p, R"(, "tag" : "%255[^"]" %n } )", tag, &readByte) != 1)
		{
			continue;
		}
		tag[255] = '\0';
		p += readByte;	//読み取り位置を更新


		//メッシュを読み込む
		const std::string meshName = std::string("Res/Model/") + baseMeshName + ".obj";
		auto mesh = LoadOBJ(meshName.c_str());

		if (!mesh)
		{
			LOG_WARNING("メッシュファイル%sの読み込みに失敗", meshName.c_str());
			continue;
		}

		//座標の拡大縮小と平行移動
		t = t * scale + translate;
		s *= scale;

		//ゲームオブジェクトを作成
		auto gameObject = Create<GameObject>(meshName);
		gameObject->SetPos(t);

		//Unityは左手座標系なので、回転方向を右手座標系に変換する
		gameObject->SetRotation(vec3(radians(-r.x), radians(-r.y), radians(r.z)));


		auto renderer = gameObject->AddComponent<MeshRenderer>();
		renderer->mesh = mesh;
		renderer->scale = s * 0.5f;	//Unityで出力しているためサイズ合わせるため半分に

		//UnityからエクスポーとしたOBJはX軸が反転しているので、Y軸で180度回転させる
		renderer->rotation.y = radians(180);

		std::string Tag = tag;
		//地面だった場合
		if (Tag == "Floor")
		{
			//コライダーを割り当てる
			auto Gcollider = gameObject->AddComponent<BoxCollider>();
			Gcollider->box.Scale = renderer->scale;

			gameObject->SetMoveFlg(false);

		}
		else if (Tag == "Rock")//動かせる岩の場合
		{
			//コライダーを割り当てる
			auto Gcollider = gameObject->AddComponent<BoxCollider>();
			Gcollider->box.Scale = renderer->scale;

			gameObject->SetMoveFlg(true);
		}
		else if (Tag == "FallFloor")//落下床の場合
		{
			//コライダーを割り当てる
			auto Gcollider = gameObject->AddComponent<BoxCollider>();
			Gcollider->box.Scale = renderer->scale;

			gameObject->AddComponent<FallFloor>();
			gameObject->SetMoveFlg(false);
		}
		else if (Tag == "Tree")
		{
			auto capcollider = gameObject->AddComponent<CapsuleCollider>();

			//モデルごとに幹の太さが違うため、この方法で対処
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

			//コライダーを割り当てる
			auto Gcollider = gameObject->AddComponent<BoxCollider>();
			Gcollider->box.Scale = renderer->scale;

			gameObject->SetMoveFlg(false);

		}
		else if (Tag == "Goal")
		{
			std::string Name = name;
			gameObject->name = Name;

			//コライダーを割り当てる
			auto Gcollider = gameObject->AddComponent<BoxCollider>();
			Gcollider->box.Scale = renderer->scale;

			auto ParticleObject = Create<GameObject>(
				"particle explosion", gameObject->GetPos());
			

			auto ps = ParticleObject->AddComponent<ParticleSystem>();
			ps->Emitters.resize(1);
			auto& emitter = ps->Emitters[0];
			emitter.ep.ImagePath = "Res/Last.tga";

			emitter.ep.RandomizeRotation = 1;		//角度をつける
			emitter.ep.RandomizeDirection = 1;
			emitter.ep.RandomizeSize = 1;			//大きさをランダムに
			emitter.ep.Duration = 0.1f;				//放出時間
			emitter.ep.EmissionsPerSecond = 100;	//秒間放出数

			emitter.pp.LifeTime = 0.4f;				//生存時間
			emitter.pp.color.Set({ 5, 1, 5.5f, 1 }, { 1, 2, 1.5f, 0 });	//色付け
			emitter.pp.scale.Set({ 0.3f,0.1f }, { 0.05f,0.02f });	//サイズを徐々にへ変更させる

			emitter.ep.Loop = true;
			gameObject->SetMoveFlg(false);

		}
		else if (Tag == "WarpEnter")
		{
			std::string Name = name;
			gameObject->name = name;

			
			//コライダーを割り当てる
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

			//コライダーを設定
			auto collider = gameObject->AddComponent<SphereCollider>();
			collider->sphere.Center = renderer->translate;
			collider->sphere.Radius = 1.5f;

			auto hh = gameObject->AddComponent<Health>();		//HP
			auto ee = gameObject->AddComponent<Explosion>();	//爆発

		}
		//ゲームオブジェクトを追加
		gameObjectList.push_back(gameObject);
	}


	//作成したゲームオブジェクト配列を返す
	LOG("3Dモデル配置ファイル%sを読み込みました", filename);

	return gameObjectList;
}

//フェードインを開始する
//@param time フェードにかかる時間

void Engine::StartFadeIn(float time)
{
	//状態をフェード実行中にする
	fadeState = FadeState::Fading;

	//エフェクトにかかる時間を設定し、経過時間をリセットする
	FadeTotalTime = time;
	FadeTimer = 0;

	//閾値が０～１に変化するように設定
	FadeFrom = 0;
	FadeTo = 1;
}

//フェードアウトを開始する
//@param time フェードにかかる時間

void Engine::StartFadeOut(float time)
{
	//状態をフェード実行中にする
	fadeState = FadeState::Fading;

	//エフェクトにかかる時間を設定し、経過時間をリセットする
	FadeTotalTime = time;
	FadeTimer = 0;

	//閾値が１～０に変化するように設定
	FadeFrom = 1;
	FadeTo = 0;
}

//UIレイヤーを追加する

//@param image      レイヤーで使用するテクスチャ画像
//@param filterMode テクスチャに設定する拡大縮小フィルタ
//@param reserve    レイヤー用に予約するゲームオブジェクト数
//@return 追加したレイヤー番号

size_t Engine::AddUILayer(const char* image, GLenum filterMode, size_t reserve)
{
	TexturePtr tex = Texture::Create(image, filterMode,
		Texture::Usage::for2D);
	uiLayerList.push_back({ tex });
	uiLayerList.back().gameObjectList.reserve(reserve);
	uiLayerList.back().spriteList.reserve(reserve);

	return uiLayerList.size() - 1;
}

//すべてのレイヤーを削除する
void Engine::ClearUILayers()
{
	uiLayerList.clear();
}

//OBJファイルを読み込む
//@param filename 読み込むOBJファイル名
//@return作成したメッシュのアドレス

Mesh::StaticMeshPtr Engine::LoadOBJ(const char* filename)
{
	return meshBuffer->LoadOBJ(filename);
}

//エミッターを追加する
//@param ep	エミッターの初期化パラメータ
//@param pp パーティクルの初期化パラメータ

ParticleEmitterPtr Engine::AddParticleEmitter(
	const ParticleEmitterParameter& ep, const ParticleParameter& pp)
{
	return particleManager->AddEmitter(ep, pp);
}

//指定された名前を持つエミッターを検索する
//@param  name 検索するid
//@return 引数idと一致するidを持つエミッター

ParticleEmitterPtr Engine::FindParticleEmitter(const std::string& name)const
{
	return particleManager->FindEmitter(name);
}

//指定されたエミッターを削除する
void Engine::RemoveParticleEmitter(const ParticleEmitterPtr& p)
{
	particleManager->RemoveEmitter(p);
}

//すべてのエミッターを削除する
void Engine::RemoveParticleEmitterAll()
{
	particleManager->RemoveEmitterAll();
}

//ゲームエンジンを初期化する
//		@retval 0 正常に初期化された
//		@retval 0以外　エラーが発生した

int Engine::Initialize()
{
	//GLFWの初期化
	if (glfwInit() != GLFW_TRUE)
	{
		return 1;
	}


	//描画ウィンドウの作成
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);//	ヒントの種類、ヒントに設定する値

	//フレームバッファをsRGB対応にする
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
	window = glfwCreateWindow(1280, 720, title.c_str(), nullptr, nullptr);

	if (!window)
	{
		glfwTerminate();
		return 1;
	}

	//OpenGLコンテキストの作成
	glfwMakeContextCurrent(window);

	//OpenGL関数のアドレスを取得する
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		glfwTerminate();
		return 1;
	}

	//メッセージコールバックの設定
	glDebugMessageCallback(DebugCallback, nullptr);//(呼び出す関数のアドレス、userParam引数に渡される値)

	//シェーダーファイル(2D)の読み込み
	progSprite = ProgramPipeline::Create(
		"Res/standard_2D.vert", "Res/standard_2D.frag");

	//シェーダーファイル(3D)の読み込み
	progStandard3D = ProgramPipeline::Create(
		"Res/standard_3D.vert", "Res/standard_3D.frag");

	//影用オブジェクトを作成
	fboShadow = FramebufferObject::Create(1024, 1024, FboType::depth);
	progShadow = ProgramPipeline::Create("Res/shadow.vert", "Res/shadow.frag");

	//スカイボックス用オブジェクトを生成
	const std::string skyBoxPathList[6] = {
		"Res/SkyBox/px2.tga","Res/SkyBox/nx2.tga",
		"Res/SkyBox/py2.tga","Res/SkyBox/ny2.tga",
		"Res/SkyBox/pz2.tga","Res/SkyBox/nz2.tga",
	};

	texSkyBox = Texture::CreateCubeMap(skyBoxPathList);
	progSkyBox = ProgramPipeline::Create("Res/skybox.vert", "Res/skybox.frag");

	//ゲームウィンドウ用FBOを作成
	const vec2 viewSize = GetViewSize();
	fboGameWindow = FramebufferObject::Create(
		static_cast<int>(viewSize.x), static_cast<int>(viewSize.y), FboType::colorDepth);
	
	//ぼかしシェーダを読み込む
	progGaussianBlur = ProgramPipeline::Create(
		"Res/standard_2D.vert", "Res/gaussian_blur.frag");

	//ブルーム用シェーダ―を読み込む
	progHighPassFilter = ProgramPipeline::Create(
		"Res/simple_2D.vert", "Res/high_pass_filter.frag");
	progDownSampling = ProgramPipeline::Create(
		"Res/simple_2D.vert", "Res/down_sampling.frag");
	progUpSampling = ProgramPipeline::Create(
		"Res/simple_2D.vert", "Res/up_sampling.frag");

	//シーン遷移エフェクト用のファイルを読み込む
	progFade = ProgramPipeline::Create("Res/simple_2D.vert", "Res/fade.frag");
	texFadeColor = Texture::Create("Res/fade_color.tga",
		GL_LINEAR, Texture::Usage::for3D);
	texFadeRule = Texture::Create("Res/fade_rule.tga",
		GL_LINEAR, Texture::Usage::for3DLinear);

	//座標変換行列をユニフォーム変数にコピー
	//使用する頂点座標が-0.5~0.5のサイズなので2倍する
	//スプライトとFBOのテクスチャ座標は逆なのでY軸マイナスにする
	const mat4 matBloomM = {//モデル行列
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

	//メイン画面のブルームエフェクト用FBOを作成
	int bloomX = static_cast<int>(viewSize.x);
	int bloomY = static_cast<int>(viewSize.y);

	for (auto& e : fboBloom)
	{
		bloomX /= 2;
		bloomY /= 2;

		e = FramebufferObject::Create(bloomX, bloomY, FboType::color);
	}


	//頂点データをGPUメモリにコピー
	struct Vertex {
		float x, y, z;	//頂点座標
		float u, v;		//テクスチャ座票
	};

	//画像の位置座標を決める
	const Vertex vertexData[] = {
		{-0.5f ,-0.5f, 0 , 0 , 1},
		{ 0.5f , -0.5f, 0 , 1 , 1},
		{ 0.5f ,  0.5f, 0 , 1 , 0},
		{-0.5f ,  0.5f, 0 , 0, 0},
	};

	vbo = BufferObject::Create(sizeof(vertexData), vertexData);

	//インデックスデータをGPUメモリにコピー
	const GLshort indexData[] = {
		0,1,2,2,3,0,
	};

	ibo = BufferObject::Create(sizeof(indexData), indexData);

	//頂点データの構成情報を設定
	vao = VertexArray::Create();
	glBindVertexArray(*vao);
	glBindBuffer(GL_ARRAY_BUFFER, *vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ibo);
	glEnableVertexAttribArray(0);		//頂点属性を有効にする	(有効にする頂点属性配列のインデックス)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);//GPUに伝達する	(頂点属性配列のインデックス、データの要素数、データの型、正規化の有無、次のデータまでのバイト数、最初のデータ位置)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(const void*)offsetof(Vertex, u));




	//スプライト用のＳＳＢＯを作成
	ssboSprite = MappedBufferObject::Create(
		CalcSsboSize(maxSpriteCount * sizeof(Sprite)),
		GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);

	//3Dモデル用のバッファを作成
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

	//メインカメラを作成
	cameraObject = Create<GameObject>("Main Camera");
	cameraObject->AddComponent<Camera>();

	//乱数を初期化
	std::random_device rd;	//真の乱数を生成するオブジェクト（遅い）
	rg.seed(rd());			//疑似乱数を「真の乱数」で初期化

	//パーティクルマネージャーを作成
	particleManager = ParticleManager::Create(100'000);

	//ゲームオブジェクト配列の容量を予約
	gameObjectList.reserve(1000);	//1000分予約した。

	//音声ライブラリを初期化
	if (!Audio::Initialize())
	{
		return 1;	//初期化失敗
	}


	//IMGUI_CHECKVERSION();
	//ImGui::CreateContext();
	//ImGuiIO& io = ImGui::GetIO(); (void)io;

	//ImGui::StyleColorsDark();

	//ImGui_ImplGlfw_InitForOpenGL(window, true);
	//ImGui_ImplOpenGL3_Init("#verison 450");


	//最初のシーンを作成して初期化
	scene = std::make_shared<TitleScene>();
	if (scene)
	{
		scene->Initialize(*this);
	}


	return 0;//正常に初期化された
}

//メインループの実行

int Engine::MainLoop()
{
	//ImGui_ImplOpenGL3_NewFrame();
	//ImGui_ImplGlfw_NewFrame();
	//ImGui::NewFrame();

	//FPS計測用の変数
	double fpsTime = glfwGetTime();		//経過時間を取得
	double fpsCount = 0;				//計測用
	double prevTime = glfwGetTime();	//前回のループで計測した時間

	//メインループ
	while (!glfwWindowShouldClose(window))
	{
		//FPS計測
		const double curTime = glfwGetTime();	//経過時間を取得
		const double differTime = curTime - fpsTime;
		++fpsCount;

		//１秒ごとにFPSを表示
		if (differTime >= 1)
		{
			const std::string fps = std::to_string(fpsCount / differTime);		//出てきた値を文字列に変換
			const std::string newTitle = title + "-FPS:" + fps;						//タイトルの書き換え
			glfwSetWindowTitle(window, newTitle.c_str());							//タイトルの再表示

			//計測用変数をリセット
			fpsTime = curTime;
			fpsCount = 0;
		}

		//経過時間(デルタタイム)を計測
		float deltaTime = static_cast<float>(curTime - prevTime) * SlowSpeed;
		if (deltaTime > 1) {
			deltaTime = 1.0f / 60;
		}
		prevTime = curTime;

		//シーンの切り替え
		if (nextScene)
		{
			if (scene)
			{
				scene->Finalize(*this);
			}
			nextScene->Initialize(*this);
			scene = std::move(nextScene);
		}

		//シーンを更新
		if (scene)
		{
			scene->Update(*this, deltaTime);
		}

		//Escapeキーを押すと、マウスが返却される
		if (this->GetKey(GLFW_KEY_ESCAPE))
		{
			//マウスを返してもらう
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}


		//ゲームオブジェクトを更新
		UpdateGameObject(deltaTime);		//アップデート
		CollideGameObject();				//衝突判定
		RemoveGameObject(gameObjectList);	//ゲームオブジェクトを消去

		for (auto& layer : uiLayerList)
		{
			RemoveGameObject(layer.gameObjectList);
		}

		for (auto& layer : uiLayerList)
		{
			MakeSpriteList(layer.gameObjectList, layer.spriteList);
		}


		//GPUがバッファを使い終わるのを待つ
		ssboSprite->WaitSync();


		//スプライト用SSBOのアドレスを取得
		uint8_t* p = ssboSprite->GetMappedAddress();


		//ゲームウィンドウ用のスプライトデータをSSBOにコピー
		SpriteList spriteListPostProcess;

		if (!gameObjectList.empty())
		{
			//ポストプロセス用スプライトを追加
			const vec2 gameWindowPos = GetWindowSize() * 0.5f;
			spriteListPostProcess.push_back(
				Sprite{ gameWindowPos.x,gameWindowPos.y,0,1,
				Texcoord{0,1,1,-1}, 1, 1,1,1 });

			const size_t size = spriteListPostProcess.size() * sizeof(Sprite);
			memcpy(p, spriteListPostProcess.data(), size);
			p += CalcSsboSize(size);
		}

		//UIレイヤーのスプライトデータをSSBOにコピー
		for (const auto& layer : uiLayerList)
		{
			memcpy(p, layer.spriteList.data(), layer.spriteList.size() * sizeof(Sprite));
			p += CalcSsboSize(layer.spriteList.size() * sizeof(Sprite));
		}

		//パーティクルを更新
		//カメラz軸方向を向くようにビュー行列を作成
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

		//描画先をゲームウィンドウに変更
		glBindFramebuffer(GL_FRAMEBUFFER, *fboGameWindow);
		glDisable(GL_FRAMEBUFFER_SRGB);	//ガンマ補正を無効にする
		const vec2 viewSize = GetViewSize();

		glViewport(0, 0,
			static_cast<int>(viewSize.x), static_cast<int>(viewSize.y));

		glBindVertexArray(*vao);

		//バックバッファをクリア
		glClearColor(0.1f, 0.2f, 0.6f, 0.0f);//赤,緑,青,透明度(0~1の値だけ)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//スカイボックスを描画
		{
			//深度バッファへの書き込みを行わないことで、最も遠いと思わせる
			glDisable(GL_DEPTH_TEST);

			//立方体の裏側を描画したいので、カリング対象を表側にする
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);

			//アルファブレンドは使わないので描画の高速化のために無効化
			glDisable(GL_BLEND);

			//キューブマップのつなぎ目を目立たなくする機能を有効化
			glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

			//ビューポートを設定
			Camera& camera = static_cast<Camera&>(*cameraObject->componentList[0]);
			glViewport(
				camera.viewport.x, camera.viewport.y,
				camera.viewport.width, camera.viewport.height);

			//スカイボックス用のカメラ情報を設定
			glProgramUniform4f(*progSkyBox, 3,
				camera.fovScaleX, camera.fovScaleY, camera.A, camera.B);

			glProgramUniform4f(*progSkyBox, 5,
				sin(cameraObject->GetRotation().x), cos(cameraObject->GetRotation().x),
				sin(cameraObject->GetRotation().y), cos(cameraObject->GetRotation().y));
		
			//立方体にキューブマップを描画
			const auto& skybox = meshBuffer->LoadOBJ("Res/Model/box.obj");
			const auto& prim = skybox->primitives[0];
			glUseProgram(*progSkyBox);
			glBindVertexArray(*meshBuffer->GetVAO());
			glBindTextureUnit(0, *texSkyBox);

			glDrawElementsBaseVertex(prim.mode, prim.count,
				GL_UNSIGNED_SHORT, prim.indices, prim.baseVertex);

			glCullFace(GL_BACK);	//カリング対象を元に戻す
		}

		//描画に使うシェーダを指定
		glUseProgram(*progSprite);

		//深度テストを無効化
		glDisable(GL_DEPTH_TEST);

		//裏面カリングを無効化
		glDisable(GL_CULL_FACE);

		//アルファブレンドを有効にする
		glEnable(GL_BLEND);		//指定した機能を有効にする
		glBlendEquation(GL_FUNC_ADD);	//半透明合成のため	
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	//半透明合成のため

#if 0
		//ユニフォーム変数にデータをコピーする
		const float timer = static_cast<float>(glfwGetTime());//起動時からの時間経過を取得
		glProgramUniform1f(progSprite, 0, timer);//プログラムオブジェクトの管理番号、送り先のロケーション番号、送るデータ
#endif

		//スプライト描画用のVAOを設定
		glBindVertexArray(*vao);

		//2D背景を描画
		size_t spriteSsboOffset = 0;


		//3Dメッシュを描画
		DrawStaticMesh(gameObjectList);
		
		//パーティクルを描画
		{
			//描画範囲を設定
			const Camera& camera = static_cast<Camera&>(*cameraObject->componentList[0]);
			glViewport(camera.viewport.x, camera.viewport.y,
				camera.viewport.width, camera.viewport.height);

			//プロジェクション行列を作成
			const float w = static_cast<float>(camera.viewport.width);
			const float h = static_cast<float>(camera.viewport.height);

			const mat4 marParticleProj = mat4::Perspective(
				radians(camera.fovY), w / h, camera.near, camera.far);

			//パーティクルを描画
			particleManager->Draw(marParticleProj, matParticleView, matBillboard);
		}

		//ブルームを描画
		glBindVertexArray(*vao);
		DrawBloomEffect();

		//デフォルトフレームバッファに戻す
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_FRAMEBUFFER_SRGB);	//ガンマ補正を有効にする
		glViewport(0, 0, 1280, 720);
		glProgramUniform4f(*progSprite, 2, 2.0f / 1280, 2.0f / 720, -1, -1);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//ゲームウィンドウを描画
		glUseProgram(*progSprite);
		glProgramUniform4f(*progSprite, 2, 2.0f / 1280, 2.0f / 720, -1, -1);

		if (!gameObjectList.empty())
		{
			//ぼかし強度が０より大きければぼかしシェーダを使う
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

		//UIレイヤーのスプライト配列を描画
		for (const auto& layer : uiLayerList)
		{
			DrawSpriteList(layer.spriteList, *layer.tex, spriteSsboOffset);
		}

		//シーン遷移エフェクト

		//フェードが開けていなかったら
		if (fadeState != FadeState::Open)
		{
			glUseProgram(*progFade);
			glBindVertexArray(*vao);

			//テクスチャをOpenGlコンテキストに割り当てる
			const GLuint tex[] = { *texFadeColor,*texFadeRule };
			glBindTextures(0, 2, tex);
			
			//ロケーション番号
			const GLint locFadeThreshold = 100;

			//フェードの閾値
			const float FadeThreshold =
				FadeFrom + (FadeTo - FadeFrom) * (FadeTimer / FadeTotalTime);

			//シェーダーにつっこむ
			glProgramUniform1f(*progFade, locFadeThreshold, FadeThreshold);

			//エフェクトを描画
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

			glBindTextures(0, 1, nullptr);
			glBindVertexArray(0);
			glUseProgram(0);

			//タイマーの更新
			FadeTimer += deltaTime;

			//タイマーがそう動作時間以上になったら状態を更新
			if (FadeTimer >= FadeTotalTime)
			{
				FadeTimer = FadeTotalTime;

				//閾値の変化方向が0~1ならopen,それ以外はclosed状態にする
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

		//スプライト用SSBOの割り当てを解除する
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, 0, 0, 0);//解除はSSBOの管理番号を０にする

		//スプライト描画の直後にフェンスを作成
		ssboSprite->SwapBuffers();


//音声ライブラリを更新
		Audio::Update();
		
		//ImGui::Render();
		//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	return 0;
}

//ゲームエンジンを終了する
//		@retval 0　　　正常に終了した
//		@retval 0以外　エラーが発生した

int Engine::Finalize()
{
	//音声ライブラリを終了
	Audio::Finalize();
	//ImGui_ImplOpenGL3_Shutdown();
	//ImGui_ImplGlfw_Shutdown();
	//ImGui::DestroyContext();
	//GLFWの終了
	glfwTerminate();

	return 0;
}

//ゲームオブジェクトの状態を更新
void Engine::UpdateGameObject(float deltaTime)
{
	//処理中に配列が変化しないようにコピーを作成
	std::vector<GameObject*> list(gameObjectList.size());
	std::transform(gameObjectList.begin(), gameObjectList.end(), list.begin(),//変換範囲の先頭、終端、コピー先の先頭
		[](const GameObjectPtr& e) {return e.get(); });//変換関数


	//UIレイヤーのゲームオブジェクトのコピーを作成
	for (auto& layer : uiLayerList)
	{
		const size_t prevListSize = list.size();
		list.resize(prevListSize + layer.gameObjectList.size());
		std::transform(layer.gameObjectList.begin(), layer.gameObjectList.end(),
			list.begin() + prevListSize,
			[](const GameObjectPtr& e) {return e.get(); });


	}
	//カメラオブジェクトを追加
	if (cameraObject)
	{
		list.push_back(cameraObject.get());
	}

	//Startイベント
	for (auto& e : list)
	{
		e->Start();
	}

	//Updateイベント
	for (auto& e : list)
	{
		if (!e->GetDead())
		{
			e->Update(deltaTime);
		}
	}
}

//ゲームオブジェクトの衝突判定
void Engine::CollideGameObject()
{

	//ワールド座標のコライダーを計算
	std::vector<WorldCollider> tmp;
	tmp.reserve(gameObjectList.size());

	for (auto& e : gameObjectList)
	{
		//スタートをしていない、または死んでいるやつは判定しない
		if (!e->IsStarted() || e->GetDead())
		{
			continue;
		}

		//ワールド座標＝ローカル座標＋ゲームオブジェクトの座標
		const auto& local = e->colliderList;

		//もしもコライダーを持っているゲームオブジェクトがいなかったら
		if (local.empty())
		{
			//次を処理
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

	//コライダーの衝突判定
	for (auto pa = tmp.begin(); pa != tmp.end(); ++pa)
	{
		for (auto pb = pa + 1; pb != tmp.end(); ++pb)
		{

			if (pb->gameObject->GetDead())
			{
				continue;//衝突していない
			}

			auto cpList = pa->DetectCollision(*pb);

			if (!cpList.empty())
			{

				//衝突しているので、衝突イベント関数を実行
				pa->gameObject->OnCollision(*pb->gameObject);
				pb->gameObject->OnCollision(*pa->gameObject);
				
				if (pa->gameObject->GetDead())
				{
					break;//paが死んでいるならループを打ち切る
				}
			}
		}
		
	}

	//地面に着地していない物を空中判定とする
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

//死亡状態のゲームオブジェクトを削除
void Engine::RemoveGameObject(GameObjectList& gameObjectList)
{
	//生きているオブジェクトと死んでいるオブジェクトに分ける
	const auto i = std::stable_partition(gameObjectList.begin(), gameObjectList.end(),//範囲の先頭、終了、ふりわけ条件
		[](const GameObjectPtr& e) { return !e->GetDead(); }//関数をよびだすために、removeではなく、partition
	);

	//死んでいるゲームオブジェクトを別の配列に移動
	GameObjectList deadList(
		std::make_move_iterator(i),
		std::make_move_iterator(gameObjectList.end())
	);

	//配列から移動済みオブジェクトを消去
	gameObjectList.erase(i, gameObjectList.end());

	//死んでいるオブジェクトに対してDestoroyイベントを実行
	for (auto& e : deadList)
	{
		e->OnDestroy();
	}
}

//ゲームオブジェクトからスプライト配列を作成
void Engine::MakeSpriteList(GameObjectList& gameObjectList, SpriteList& spriteList)
{
	//既存の動く物体用のスプライト配列を空にする
	spriteList.clear();

	//優先順位が大きい順番に並び変える
	std::stable_sort(
		gameObjectList.begin(), gameObjectList.end(),//先頭、終端
		[](const GameObjectPtr& a, const GameObjectPtr& b)//並べ変え条件
		{return a->GetPriority() > b->GetPriority(); }
	);
	

	//ゲームオブジェクトのスプライトデータをスプライト配列に追加
	//同時にワールド座標へ変換する
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

//スプライト配列を描画する

//@param spriteList			描画するスプライト配列
//@param tex				描画に使用するテクスチャ
//@param spriteSsboOffset	SSBO内の素往来とデータ開始位置

void Engine::DrawSpriteList(const SpriteList& spriteList, GLuint tex, size_t& spriteSsboOffset)
{

	const size_t size = CalcSsboSize(spriteList.size() * sizeof(Sprite));

	if (size == 0)
	{
		return;	//描画データがなければスキップ
	}


	//テクスチャを指定する
	glBindTextures(0, 1, &tex);//割り当て開始インデックス、割り当てる個数、テクスチャ管理番号配列のアドレス

	//スプライト用SSBOを割り当てる
	ssboSprite->Bind(0, spriteSsboOffset, size);
	spriteSsboOffset += size;

	//図形を描画
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0,
		static_cast<GLsizei>(spriteList.size()));

}

//3Dメッシュを描画

void Engine::DrawStaticMesh(GameObjectList& gameObjectList)
{

	//平行光源
	struct DirectionalLight
	{
		vec3 color;			//色
		float intensity;	//明るさ
		vec3 direction;		//向き
	};
	
	DirectionalLight directionalLight = {
		vec3(1.0f,0.9f,1.0f),
		5, 
		vec3(0.57735f,-0.57735f,-0.57735f)
	};

	//深度マップ作成用の注視店座標
	const vec2 viewSize = GetViewSize();
	const vec3 shadowTarget =
		vec3(viewSize.x * 0.5f, viewSize.y * 0.5f, 0);

	//深度マップ作成用の視点座標
	const vec3 shadowEye =
		shadowTarget - directionalLight.direction * 1000;

	//深度マップを作成
	const mat4 matShadow = CreateDepthMap(
		gameObjectList, shadowEye, shadowTarget, 750, 100, 2000);

	//ゲームウィンドウ用FBOに描画
	glBindFramebuffer(GL_FRAMEBUFFER, *fboGameWindow);
	glUseProgram(*progStandard3D);
	glBindVertexArray(*meshBuffer->GetVAO());

	//深度テストを有効化
	glEnable(GL_DEPTH_TEST);

	//裏面カリングを有効化
	glEnable(GL_CULL_FACE);

	//アルファブレンドを有効化
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//影描画用の深度マップをテクスチャとして割り当てる
	const GLuint texDepth = *fboShadow->GetDepthTexture();
	glBindTextures(2, 1, &texDepth);

	//影座標からテクスチャ座標に変換する行列
	const mat4 matTexture = {
		vec4(0.5f, 0.0f, 0.0f, 0.0f),
		vec4(0.0f, 0.5f, 0.0f, 0.0f),
		vec4(0.0f, 0.0f, 0.5f, 0.0f),
		vec4(0.5f, 0.5f, 0.5f, 1.0f),
	};
	const mat4 matShadowTexture = matTexture * matShadow;
	glProgramUniformMatrix4fv(*progStandard3D, 1, 1, GL_FALSE, &matShadowTexture[0][0]);
	
	//平行光源の情報を設定
	const vec3 color = directionalLight.color * directionalLight.intensity;
	glProgramUniform3f(*progStandard3D, 100, color.x, color.y, color.z);

	glProgramUniform3f(*progStandard3D, 101,
		directionalLight.direction.x,
		directionalLight.direction.y,
		directionalLight.direction.z);

	//環境光の情報を設定
	const vec3 ambientLight = vec3(0.2f, 0.2f, 0.3f);
	glProgramUniform3f(*progStandard3D, 104,
		ambientLight.x, ambientLight.y, ambientLight.z);

	//3D表示用のカメラ情報を設定

	//視野角等の情報
	Camera& camera = static_cast<Camera&>(*cameraObject->componentList[0]);
	glProgramUniform4f(*progStandard3D, 3,
		camera.fovScaleX, camera.fovScaleY, camera.A, camera.B);

	//カメラの座標
	glProgramUniform3fv(*progStandard3D, 4,
		1,const_cast<GLfloat*>(&cameraObject->GetPos().x));

	//カメラの回転角(x)
	glProgramUniform2f(*progStandard3D, 5,
		sin(cameraObject->GetRotation().x), cos(cameraObject->GetRotation().x));

	//カメラの回転角(y)
	glProgramUniform2f(*progStandard3D, 6,
		sin(cameraObject->GetRotation().y), cos(cameraObject->GetRotation().y));

	//ビューポートを設定
	glViewport(
		camera.viewport.x, camera.viewport.y,
		camera.viewport.width, camera.viewport.height);

	//メッシュレンダラを持つすべてのゲームオブジェクトを描画
	for (const auto& gameObject : gameObjectList)
	{
		gameObject->DrawStaticMesh(*progStandard3D);
	}

	glBindTextures(0, 2, nullptr);
	glBindVertexArray(0);
	glUseProgram(0);

}

//深度マップを作成する

//@param gameObjectList 描画するゲームオブジェクトの配列
//@param eye            影描画用の視点座標
//@param target         影描画用の注視点の座標
//@param range          eyeから影描画範囲の上端、下端、左端、右端までの距離
//@param zNear          eyeから影描画範囲の手前端までの距離
//@param zFar           eyeから影描画範囲ｎ奥端までの距離

//@return 影描画用ビュープロジェクション行列

mat4 Engine::CreateDepthMap(GameObjectList& gameObjectList,
	const vec3& eye, const vec3& target,
	float range, float zNear, float zFar)
{
	//描画先を影用ＦＢＯに変更
	glBindFramebuffer(GL_FRAMEBUFFER, *fboShadow);
	glClear(GL_DEPTH_BUFFER_BIT);

	//ビューポートをＦＢＯサイズに合わせる
	const GLuint texDepth = *fboShadow->GetDepthTexture();
	GLint w, h;
	glGetTextureLevelParameteriv(texDepth, 0, GL_TEXTURE_WIDTH, &w);//影用FBOサイズ
	glGetTextureLevelParameteriv(texDepth, 0, GL_TEXTURE_WIDTH, &h);
	glViewport(0, 0, w, h);

	//描画パラメータを設定
	glBindVertexArray(*meshBuffer->GetVAO());	//3Dモデル用VAOをバインド
	glUseProgram(*progShadow);	//影用描画用シェーダを指定
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);		//アルファブレンドを無効化

	//影用ビュー行列を作成
	const mat4 matShadowView = mat4::LookAt(eye, target, vec3(0, 1, 0));

	//影用プロジェクション行列を作成
	const mat4 matShadowProj = mat4::Orthogonal(
		-range, range,
		-range, range,
		zNear, zFar);

	//影用ビュープロジェクション行列を作成
	const mat4 matShadow = matShadowProj * matShadowView;

	//matShadowをユニフォーム変数にコピー
	glProgramUniformMatrix4fv(*progShadow, 1, 1, GL_FALSE, &matShadow[0][0]);

	//メッシュレンダラを持つすべてのゲームオブジェクトを描画
	for (const auto& gameObject : gameObjectList)
	{
		gameObject->DrawStaticMesh(*progShadow);
	}

	return matShadow;
}

//カラーテクスチャをFBOに描画する
//@param fbo      描画先FBO
//@param texture  FBOに描画するテクスチャ
//@param wrapMode テクスチャに設定するラップモード
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
	//ブルームエフェクト用にGLコンテキストを設定
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	//ゲーム画面用FBOから高輝度成分を抽出
	glUseProgram(*progHighPassFilter);
	glDisable(GL_BLEND);//半透明合成しないから
	{
		//高輝度成分はFBOの枚数だけ加算されるので、その影響を打ち消すために枚数で割る
		const float strength = bloomStrength / static_cast<float>(std::size(fboBloom));
		glProgramUniform2f(*progHighPassFilter, 100, bloomThreshold, strength);

		DrawTextureToFbo(*fboBloom[0],
			*fboGameWindow->GetColorTexture(), GL_CLAMP_TO_BORDER);

	}

	//縮小ぼかしを行い、１段小さいFBOにコピー
	glUseProgram(*progDownSampling);

	for (size_t i = 1; i < std::size(fboBloom); ++i)
	{
		DrawTextureToFbo(*fboBloom[i],
			*fboBloom[i - 1]->GetColorTexture(), GL_CLAMP_TO_BORDER);
	}
	
	//拡大ぼかしを行い、１段大きいFBOに加算合成
	glUseProgram(*progUpSampling);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);//加算合成(シェーダ出力＋FBO)

	for (size_t i = std::size(fboBloom) - 1; i > 0;)
	{
		--i;
		DrawTextureToFbo(*fboBloom[i],
			*fboBloom[i + 1]->GetColorTexture(), GL_CLAMP_TO_EDGE);
	}

	//最後の拡大ぼかしを行い、ゲーム画用FBOに加算合成
	{
		DrawTextureToFbo(*fboGameWindow,
			*fboBloom[0]->GetColorTexture(), GL_CLAMP_TO_EDGE);
	}
}

