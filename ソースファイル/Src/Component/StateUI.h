//@file StateUI.h

#ifndef COMPONENT_STATEUI_H_INCLUDED
#define COMPONENT_STATEUI_H_INCLUDED
#include "../Engine/Component.h"
#include "Text.h"
#include "../Component/Player.h"
#include <stdio.h>

//������Ԃ������R���|�[�l���g
class StateUI :public Component
{
public:
	StateUI() = default;			//�R���X�g���N�^
	virtual ~StateUI() = default;	//�f�X�g���N�^

	virtual void Update(GameObject& gameObject, float deltaTime)override
	{

		if (TextComponent)
		{
			//switch (State)
			//{
			//case 0:
			//	break;

			//case 1:
			//	char str2[18];	//�\�����镶����

			//	snprintf(str2, std::size(str2), "Spd:%.1f", Lv);	//�����\��
			//	TextComponent->SetText(str2, 1.0f);					//�e�L�X�g�ɓ����

			//	break;
			//}
			char str[10];	//�\�����镶����

			snprintf(str, std::size(str), "%.1f", Lv);	//�����\��
			TextComponent->SetText(str, 2.0f);					//�e�L�X�g�ɓ����

		}
	}

	//�e�L�X�g�R���|�[�l���g���擾
	void SetTextComponent(std::shared_ptr<Text> _tex)
	{
		TextComponent = _tex;
	}

	//���̃��x����ݒ肷�邩
	void SetLv(float _lv)
	{
		Lv = _lv;
	}

	void SetState(int _state)
	{
		State = _state;
	}

private:
	std::shared_ptr<Text> TextComponent;//�e�L�X�g�R���|�[�l���g

	//�\�����郌�x��
	float Lv = 1;

	int State = 0;	//�ǂ����̏�Ԃ�
};

using StateUIPtr = std::shared_ptr<StateUI>;

#endif // !COMPONENT_TIMELIMIT_H_INCLUDED
