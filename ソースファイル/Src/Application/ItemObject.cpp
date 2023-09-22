#include "ItemObject.h"
#include "../Engine/AudioSettings.h"
#include "EasyAudio.h"

void ItemObject::OnCollision(GameObject& object)
{
	
	//�v���C���[��������
	if (object.name == player->name)
	{
		switch (PowerSpot)
		{
			//�ԐF
		case 0:

			//�ˌ��Ԋu��������
			player->SetShotInterval(player->GetShotInterval() - 0.01f);
			break;

			//�ΐF
		case 1:

			//�U���͂̏㏸
			player->SetShotDamage(player->GetShotDamage() + 0.1f);
			break;

			//�F
		case 2:

			//�ړ����x���㏸������
			player->SetMoveSpeed(player->GetMoveSpeed() + 0.5f);
			break;
		default:
			break;
		}
		Audio::PlayOneShot(SE::powerup);
		this->SetDeadFlg(true);
	}

}
