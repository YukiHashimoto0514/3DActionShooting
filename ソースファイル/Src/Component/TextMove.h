//@file TextMove.h

#ifndef COMPONENT_TEXTMOVE_H_INCLUDED
#define COMPONENT_TEXTMOVE_H_INCLUDED

#include "../Engine/Component.h"
#include "../Application/GameObject.h"
#include "../Engine/Sprite.h"
#include "../Component/Lerp.h"


enum class State
{
	EaseBefore,
	EaseNow,
	EaseAfter
};

//文字の動き

class TextMove :public Component
{
public:
	TextMove() = default;			//コンストラクタ
	virtual ~TextMove() = default;	//デストラクタ


	virtual void Start(GameObject& gameObject)override
	{
		//乱数の初期化
		RandomEngine.seed(rd());
		WaitTime = std::uniform_real_distribution<float>(3.0f, 5.0f)(rd);//2~5

	}

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{

		//時間が来ていないなら無視
		if (WaitTime > 0)
		{
			WaitTime -= deltaTime;
			return;
		}

		//イージング前
		if (state == State::EaseBefore)
		{
			//行動を選択する
			Action = std::uniform_int_distribution<>(0, 4)(rd);//0~4
			state = State::EaseNow;//ステートをイージング中へ
		}
			 		
		//どんな行動を取るか
		switch (Action)
		{
		case 0://少し揺れる

			//イージングが完了していなかったら
			if (TakeTime < 1)
			{
				//初めの一度だけ入る
				if (!isOnce)
				{
					//イージングに必要な情報を入力
					Before = gameObject.GetRotation();
					Target = vec3(30);
					isOnce = true;
				}

				//イージングを利用
				gameObject.SetRotation(
					vec3(Before.x + Smooth(TakeTime) * (Target.x - Before.x),
						0, 0));
			}
			else
			{
				//イージング後へ
				state = State::EaseAfter;
			}

			break;

		case 1://大きくなる

			//イージングが完了していなかったら
			if (TakeTime < 1)
			{
				//初めの一度だけ入る
				if (!isOnce)
				{
					//イージングに必要な情報を入力
					Before = gameObject.GetScale();
					Target = vec3(1.5f);
					isOnce = true;
				}

				//イージング
				gameObject.SetScale(
					vec3(Before.x + Smooth(TakeTime) * (Target.x - Before.x),
						0, 0));
			}
			else
			{
				//イージング後へ
				state = State::EaseAfter;
			}

			break;

		case 2://移動する（左）

			//イージングが完了していなかったら
			if (TakeTime < 1)
			{
				//初めの一度だけ入る
				if (!isOnce)
				{
					//イージングに必要な情報を入力
					Before = gameObject.GetPos();
					Target = vec3(gameObject.GetPos() - 100);
					isOnce = true;
				}

				//イージング
				gameObject.SetPos(
					vec3(Before.x + Smooth(TakeTime) * (Target.x - Before.x),
						Before.y, 0));
			}
			else
			{
				//イージング後へ
				state = State::EaseAfter;
			}


			break;

		case 3://傾く（右）

			//イージングが完了していなかったら
			if (TakeTime < 1)
			{
				//初めの一度だけ入る
				if (!isOnce)
				{
					//イージングに必要な情報を入力
					Before = gameObject.GetRotation();
					Target = vec3(-30);
					isOnce = true;
				}

				//イージング
				gameObject.SetRotation(
					vec3(Before.x + Smooth(TakeTime) * (Target.x - Before.x),
						0, 0));
			}
			else
			{
				//イージング後へ
				state = State::EaseAfter;
			}

			break;

		case 4://移動する（右）

			//イージングが完了していなかったら
			if (TakeTime < 1)
			{
				//初めの一度だけ入る
				if (!isOnce)
				{
					//イージングに必要な情報を入力
					Before = gameObject.GetPos();
					Target = vec3(Before + 100);
					isOnce = true;
				}

				//イージングを利用して位置を変更する
				gameObject.SetPos(
					vec3(Before.x + Smooth(TakeTime) * (Target.x - Before.x),
						Before.y, 0));
			}
			else
			{
				//イージング後へ
				state = State::EaseAfter;
			}

			break;
		default:
			break;
		}

		//イージングが終ったら
		if (state == State::EaseAfter)
		{
			TakeTime = 0;	//時間をリセット
			isOnce = false;
			WaitTime = std::uniform_real_distribution<float>(2.0f, 5.0f)(rd);//2~5
			state = State::EaseBefore;
		}

		//時間を経過
		TakeTime += deltaTime;
		

	}


private:


	float TakeTime = 0;		//イージングに使う変数
	float WaitTime = 5.0f;		//動くまでの待機時間
	bool isOnce = false;	//一度だけ入る

	int Action = 0;

	vec3 Before = vec3(1);	//移動前のもの
	vec3 Target = vec3(1);	//移動するもの

	State state = State::EaseBefore;
		
	//乱数
	std::random_device rd;
	std::mt19937 RandomEngine;

};


#endif // !COMPONENT_TEXTMOVE_H_INCLUDED
