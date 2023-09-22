//@file TimeLimit.h

#ifndef COMPONENT_TIMELIMIT_H_INCLUDED
#define COMPONENT_TIMELIMIT_H_INCLUDED
#include "../Engine/Component.h"
#include "Text.h"
#include <stdio.h>

//�c�莞�Ԃ��Ǘ�����R���|�[�l���g
class TimeLimit :public Component
{
public:
	TimeLimit() = default;			//�R���X�g���N�^
	virtual ~TimeLimit() = default;	//�f�X�g���N�^

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{
		//�������Ԃ�������A�ޏ�
		if (LimitTime <= 0)
		{
			gameObject.SetDeadFlg(true);
			return;
		}

		if (TextComponent)
		{
			char str[7];	//�\�����镶����

			const int Minute = static_cast<int>(LimitTime) / 60;			//�����v�Z
			const int Second = static_cast<int>(LimitTime) - (60 * Minute);	//�b���v�Z

			snprintf(str, std::size(str), "%d : %02d", Minute,Second);	//�����\��
			
			TextComponent->SetText(str, 2.0f);					//�e�L�X�g�ɓ����
		}
	}

	//�e�L�X�g�R���|�[�l���g���擾
	void SetTextComponent(std::shared_ptr<Text> _tex)
	{
		TextComponent = _tex;
	}

	//�c�莞�Ԃ���
	void SetLimitTime(float _time)
	{
		LimitTime = _time;
	}

private:
	float LimitTime = 100;//�c�莞��
	std::shared_ptr<Text> TextComponent;//�e�L�X�g�R���|�[�l���g
};

using TimeLimitPtr = std::shared_ptr<TimeLimit>;

#endif // !COMPONENT_TIMELIMIT_H_INCLUDED
