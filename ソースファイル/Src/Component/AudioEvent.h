//@file AudioEvent.h

#ifndef COMPONENT_AUDIOEVENT_H_INCLUDED
#define COMPONENT_AUDIOEVENT_H_INCLUDED
#include "../Engine/Component.h"
#include "../Application/EasyAudio.h"
#include <string>

//音声再生コンポーネント
class AudioEvent :public Component
{
public:
	AudioEvent() = default;
	virtual ~AudioEvent() = default;

	virtual void OnCollision(GameObject&, GameObject&)override
	{
		if (!files.onCollision.empty())
		{
			Audio::PlayOneShot(files.onCollision.c_str());
		}
	}

	virtual void OnDestroy(GameObject& gameObject)override
	{
		//画面外で破壊されたら音は鳴らさない
		if (gameObject.Point == 0)
		{
			return;
		}

		if (!files.onDestory.empty())
		{
			Audio::PlayOneShot(files.onDestory.c_str());
		}
	}

	struct {
		std::string onCollision;	//衝突時の音声ファイル名
		std::string onTakeDamage;	//ダメージを受けた時の音声ファイル名
		std::string onDestory;		//破壊時の音声ファイル名
	}files;
};

#endif // !COMPONENT_AUDIOEVENT_H_INCLUDED
