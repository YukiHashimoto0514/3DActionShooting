//@fiel Animator2D.h

#ifndef COMPONENT_ANIMATOR2D_H_INCLUDED
#define COMPONENT_ANIMATOR2D_H_INCLUDED
#include "../Engine/Component.h"
#include "../Application/GameObject.h"
#include "../Engine/Sprite.h"
#include <vector>
#include <algorithm>


//2Dアニメーションコンポーネント

class Animator2D :public Component
{
public:
	Animator2D() = default;				//コンストラクタ
	virtual ~Animator2D() = default;	//デストラクタ

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{
		//データがない場合は何もしない
		if (clip.empty())
		{
			return;
		}

		//経過時間を更新
		timer += deltaTime;

		//再生位置を計算
		const int size = static_cast<int>(clip.size());
		int i = static_cast<int>(timer / interval);

		if (loopTime)
		{
			i %= size;//ループ再生
		}
		else
		{
			//アニメーション終了時killWhenEndフラグがtrueならゲームオブジェクトを削除
			if (i >= size && killWhenEnd)
			{
				gameObject.SetDeadFlg(true);
			}
			i = std::clamp(i, 0, size - 1);
		}

		//スプライトデータを再生位置のデータに更新
		gameObject.spriteList = clip[i];

	}

	//フレームにスプライトを追加
	void AddSprite(size_t frameNo, const Texcoord& tc,
		float x = 0, float y = 0, float scale = 1, float rotation = 0)
	{
		if (frameNo >= clip.size())
		{
			clip.resize(frameNo + 1);
		}
		clip[frameNo].push_back({ x,y,0,0,tc });
	}

	bool loopTime = true;			//ループ再生の有無
	bool killWhenEnd = false;		//アニメーション終了時にゲームオブジェクトを削除
	float timer = 0;				//経過時間（秒）
	float interval = 0.1f;			//フレームを進める間隔
	std::vector<SpriteList> clip;	//アニメーションクリップ
};


#endif // !COMPONENT_ANIMATOR2D_H_INCLUDED

