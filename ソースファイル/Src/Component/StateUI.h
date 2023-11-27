//@file StateUI.h

#ifndef COMPONENT_STATEUI_H_INCLUDED
#define COMPONENT_STATEUI_H_INCLUDED
#include "../Engine/Component.h"
#include "Text.h"
#include "../Component/Player.h"
#include <stdio.h>

//強化状態を示すコンポーネント
class StateUI :public Component
{
public:
	StateUI() = default;			//コンストラクタ
	virtual ~StateUI() = default;	//デストラクタ

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{

		if (TextComponent)
		{
			//switch (State)
			//{
			//case 0:
			//	break;

			//case 1:
			//	char str2[18];	//表示する文字列

			//	snprintf(str2, std::size(str2), "Spd:%.1f", Lv);	//文字表示
			//	TextComponent->SetText(str2, 1.0f);					//テキストに入れる

			//	break;
			//}
			char str[10];	//表示する文字列

			snprintf(str, std::size(str), "%.1f", Lv);	//文字表示
			TextComponent->SetText(str, 2.0f);					//テキストに入れる

		}
	}

	//テキストコンポーネントを取得
	void SetTextComponent(std::shared_ptr<Text> _tex)
	{
		TextComponent = _tex;
	}

	//何のレベルを設定するか
	void SetLv(float _lv)
	{
		Lv = _lv;
	}

	void SetState(int _state)
	{
		State = _state;
	}

private:
	std::shared_ptr<Text> TextComponent;//テキストコンポーネント

	//表示するレベル
	float Lv = 1;

	int State = 0;	//どっちの状態か
};

using StateUIPtr = std::shared_ptr<StateUI>;

#endif // !COMPONENT_TIMELIMIT_H_INCLUDED
