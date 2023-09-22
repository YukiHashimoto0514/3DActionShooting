//@file TimeLimit.h

#ifndef COMPONENT_TIMELIMIT_H_INCLUDED
#define COMPONENT_TIMELIMIT_H_INCLUDED
#include "../Engine/Component.h"
#include "Text.h"
#include <stdio.h>

//残り時間を管理するコンポーネント
class TimeLimit :public Component
{
public:
	TimeLimit() = default;			//コンストラクタ
	virtual ~TimeLimit() = default;	//デストラクタ

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{
		//制限時間が来たら、退場
		if (LimitTime <= 0)
		{
			gameObject.SetDeadFlg(true);
			return;
		}

		if (TextComponent)
		{
			char str[7];	//表示する文字列

			const int Minute = static_cast<int>(LimitTime) / 60;			//分を計算
			const int Second = static_cast<int>(LimitTime) - (60 * Minute);	//秒を計算

			snprintf(str, std::size(str), "%d : %02d", Minute,Second);	//文字表示
			
			TextComponent->SetText(str, 2.0f);					//テキストに入れる
		}
	}

	//テキストコンポーネントを取得
	void SetTextComponent(std::shared_ptr<Text> _tex)
	{
		TextComponent = _tex;
	}

	//残り時間を代入
	void SetLimitTime(float _time)
	{
		LimitTime = _time;
	}

private:
	float LimitTime = 100;//残り時間
	std::shared_ptr<Text> TextComponent;//テキストコンポーネント
};

using TimeLimitPtr = std::shared_ptr<TimeLimit>;

#endif // !COMPONENT_TIMELIMIT_H_INCLUDED
