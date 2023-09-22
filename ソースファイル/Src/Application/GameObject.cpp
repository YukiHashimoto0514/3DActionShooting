//@file GameObject.cpp

#include "GameObject.h"
#include "../Engine/ProgramPipeline.h"
#include "../Component/MeshRenderer.h"
#include "../Engine/VecMath.h"

#include <algorithm>

using namespace VecMath;


//スタートイベント
void GameObject::Start()
{
	if (isStarted)
	{
		return;
	}

	for (auto& e : componentList)
	{
		e->Start(*this);
	}
	isStarted = true;
}

//更新イベント
void GameObject::Update(float deltaTime)
{
	for (auto& e : componentList)
	{
		e->Update(*this, deltaTime);
	}

	if (this->GetAir())
	{
		//if (GetMovable())
			//this->AddPosition(vec3(0, -deltaTime * 10, 0));
	}
}

//コリジョンイベント
void GameObject::OnCollision(GameObject& other)
{
	for (auto& e : componentList)
	{
		e->OnCollision(*this, other);
	}
}

//削除イベント
void GameObject::OnDestroy()
{
	for (auto& e : componentList)
	{
		e->OnDestroy(*this);
	}
}

//ダメージイベント
void GameObject::TakeDamage(GameObject& other, const Damage& damage)
{
	for (auto& e : componentList)
	{
		e->OnTakeDamage(*this, other, damage);
	}
}

//スプライトデータを追加する
void GameObject::AddSprite(const Texcoord& tc,
	float x, float y, float scale, float rotation)
{
	spriteList.push_back({ x,y,scale,rotation,tc });
}

//コリジョンデータを追加する
void GameObject::AddCollision(const Rect& r)
{
	collisionList.push_back(r);

	//境界ボックスの更新
	aabb.min[0] = std::min(aabb.min[0], r.min[0]);
	aabb.min[1] = std::min(aabb.min[1], r.min[1]);
	aabb.max[0] = std::max(aabb.max[0], r.max[0]);
	aabb.max[1] = std::max(aabb.max[1], r.max[1]);

}

//ゲームオブジェクトを描画する
//@param 描画に使用するシェーダープログラミング

void GameObject::DrawStaticMesh(const ProgramPipeline& program) const
{
	//メッシュがなかったらスルー
	if (meshRendererList.empty())
	{
		return;
	}

	//メッシュレンダラの移動＋回転
	mat4 mt = mat4::Translate(Position);
	mat4 mrx = mat4::RotateX(Rotation[0]);
	mat4 mry = mat4::RotateY(Rotation[1]);
	mat4 mrz = mat4::RotateZ(Rotation[2]);
	mat4 ms  = mat4::Scale(Scale);
	const mat4 matGameObject = mt * mry * mrx * mrz * ms;

	for (const auto& e : meshRendererList)
	{
		if (!e->mesh)
		{
			continue;
		}

		const vec3 t = e->translate + vec3(Position);
		const vec3 r = e->rotation + vec3(Rotation[0], Rotation[1], Rotation[2]);
		const vec3 s = e->scale;

		mat4 mt = mat4::Translate(e->translate);
		mat4 mrx = mat4::RotateX(e->rotation.x);
		mat4 mry = mat4::RotateY(e->rotation.y);
		mat4 mrz = mat4::RotateZ(e->rotation.z);
		mat4 ms = mat4::Scale(e->scale);

		mat4 matMesh = mt * mry * mrx * mrz * ms;
		mat4 matModel = matGameObject * matMesh;

		glProgramUniformMatrix4fv(program, 0, 1, GL_FALSE, &matModel[0][0]);//管理番号、ローケーション番号、こぴーする個数、正規化の有無、データのアドレス

		if (e->materials.empty())
		{
			Mesh::Draw(*e->mesh, e->mesh->materials, program);
		}
		else
		{
			Mesh::Draw(*e->mesh, e->materials, program);
		}
	}
}

