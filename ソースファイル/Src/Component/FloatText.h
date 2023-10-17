//@file FloatText.h

#ifndef COMPONENT_FLOATTEXT_H_INCLUDED
#define COMPONENT_FLOATTEXT_H_INCLUDED
#include "../Engine/Component.h"
#include "Text.h"
#include <stdio.h>
#include "../Component/Lerp.h"

//残り時間を管理するコンポーネント
class FloatText :public Component
{
public:
	FloatText() = default;			//コンストラクタ
	virtual ~FloatText() = default;	//デストラクタ

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{
		//経過時間を加算
		SpendTime += deltaTime / LifeTime;	//生存時間に合わせる

		//イージングを利用して位置を変更する
		gameObject.SetPos(
			vec3(BeforePos.x + Drop(SpendTime) * (TargetPos.x - BeforePos.x),
				BeforePos.y + Drop(SpendTime) * (TargetPos.y - BeforePos.y), 0));

		//透明度に変換
		const float alpha = 1 - SpendTime;
		gameObject.SetAlpha(alpha);	//透明度を反映

		//透明になったら
		if (alpha <= 0)
		{
			//滅ゲームオブジェクト
			gameObject.SetDeadFlg(true);
		}
	}

	void SetBeforePos(vec3 _pos)
	{
		BeforePos = _pos;
	}

private:

	const float LifeTime = 1.5f;	//消えるまでの時間

	float MoveSpeed = 20;	//移動速度
	float Alpha = 1;		//透明度
	float SpendTime = 0;	//経過時間
		
	vec3 TargetPos = { 10,500,0 };	//到着地点
	vec3 BeforePos = { 10,400,0 };	//開始地点
};

#endif // !COMPONENT_FLOATTEXT_H_INCLUDED
