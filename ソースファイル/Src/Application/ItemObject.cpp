#include "ItemObject.h"
#include "../Engine/AudioSettings.h"
#include "EasyAudio.h"
#include "../Component/FloatText.h"
void ItemObject::OnCollision(GameObject& object)
{
	//�t�H���g�摜
	const size_t textLayer = engine->AddUILayer("Res/font_04_2.tga", GL_NEAREST, 10);

	//�v���C���[��������
	if (object.name == player->name)
	{
		switch (PowerSpot)
		{
		//�ԐF
		case 0:
		{
			//�ˌ��Ԋu��������
			player->SetShotInterval(player->GetShotInterval() - 0.01f);


			//�e�L�X�g�̐���
			auto uitext = engine->CreateUI<GameObject>(textLayer, "text", 940, 600);
			auto floattext = uitext->AddComponent<FloatText>();
			auto text = uitext->AddComponent<Text>();
			text->SetText("CoolTime Down");

			break;
		}
		//�ΐF
		case 1:
		{
			//�U���͂̏㏸
			player->SetShotDamage(player->GetShotDamage() + 0.4f);


			//�e�L�X�g�̐���
			auto uitext = engine->CreateUI<GameObject>(textLayer, "text", 940, 600);
			auto floattext = uitext->AddComponent<FloatText>();
			auto text = uitext->AddComponent<Text>();
			text->SetText("AttackPower Up");

			break;
		}
		//�F
		case 2:
		{
			//�ړ����x���㏸������
			player->SetMoveSpeed(player->GetMoveSpeed() + 0.5f);

			//�e�L�X�g�̐���
			auto uitext = engine->CreateUI<GameObject>(textLayer, "text", 940, 600);
			auto floattext = uitext->AddComponent<FloatText>();
			auto text = uitext->AddComponent<Text>();
			text->SetText("MoveSpeed UP");


			break;
		}
		default:
			break;
		}

		//���̍Đ�
		Audio::PlayOneShot(SE::powerup);

		//�I�u�W�F�̔j��
		this->SetDeadFlg(true);
	}

}
