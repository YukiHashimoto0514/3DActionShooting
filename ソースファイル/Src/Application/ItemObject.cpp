#include "ItemObject.h"
#include "../Engine/AudioSettings.h"
#include "EasyAudio.h"

void ItemObject::OnCollision(GameObject& object)
{
	
	//プレイヤーだったら
	if (object.name == player->name)
	{
		switch (PowerSpot)
		{
			//赤色
		case 0:

			//射撃間隔を下げる
			player->SetShotInterval(player->GetShotInterval() - 0.01f);
			break;

			//緑色
		case 1:

			//攻撃力の上昇
			player->SetShotDamage(player->GetShotDamage() + 0.1f);
			break;

			//青色
		case 2:

			//移動速度を上昇させる
			player->SetMoveSpeed(player->GetMoveSpeed() + 0.5f);
			break;
		default:
			break;
		}
		Audio::PlayOneShot(SE::powerup);
		this->SetDeadFlg(true);
	}

}
