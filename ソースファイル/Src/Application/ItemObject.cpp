#include "ItemObject.h"
#include "../Engine/AudioSettings.h"
#include "EasyAudio.h"
#include "../Component/FloatText.h"


void ItemObject::OnCollision(GameObject& object)
{
	//�t�H���g�摜
	const size_t textLayer = engine->AddUILayer("Res/UI/font_04_2.tga", GL_NEAREST, 10);

	//�v���C���[��������
	if (object.name == player->name)
	{
		switch (PowerSpot)
		{
		//�ԐF
		case 0:
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
		//�ΐF
		case 1:
		{
			float maxHP = player->GetMaxHP();	//�ő�HP���擾
			float NowHP = player->GetHP() + 0.5f;//�񕜌��HP
		
			//�񕜌��HP���ő�l�𒴂��Ă�����
			if (NowHP > maxHP)
			{
				//�ő�HP�ɂ���
				NowHP = maxHP;
			}
			player->SetHP(NowHP);
			//�e�L�X�g�̐���
			auto uitext = engine->CreateUI<GameObject>(textLayer, "text", 940, 600);
			auto floattext = uitext->AddComponent<FloatText>();
			auto text = uitext->AddComponent<Text>();
			text->SetText("Heal");

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

		case 7://KIng
		{
			//�ړ����x���㏸������
			player->SetMoveSpeed(player->GetMoveSpeed() + 15.5f);

			//�U���͂̏㏸
			player->SetShotDamage(player->GetShotDamage() + 5.0f);

			//��
			float maxHP = player->GetMaxHP();		//�ő�HP���擾
			float NowHP = player->GetHP() + 10.0f;	//�񕜌��HP

			//�񕜌��HP���ő�l�𒴂��Ă�����
			if (NowHP > maxHP)
			{
				//�ő�HP�ɂ���
				NowHP = maxHP;
			}
			player->SetHP(NowHP);

			//�e�L�X�g�̐���
			auto uitext = engine->CreateUI<GameObject>(textLayer, "text", 940, 600);
			auto floattext = uitext->AddComponent<FloatText>();
			auto text = uitext->AddComponent<Text>();
			text->SetText("All Status UP!!");

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
