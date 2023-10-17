#include "ItemObject.h"
#include "../Engine/AudioSettings.h"
#include "EasyAudio.h"
#include "../Component/FloatText.h"
void ItemObject::OnCollision(GameObject& object)
{
	//フォント画像
	const size_t textLayer = engine->AddUILayer("Res/font_04_2.tga", GL_NEAREST, 10);

	//プレイヤーだったら
	if (object.name == player->name)
	{
		switch (PowerSpot)
		{
		//赤色
		case 0:
		{
			//射撃間隔を下げる
			player->SetShotInterval(player->GetShotInterval() - 0.01f);


			//テキストの生成
			auto uitext = engine->CreateUI<GameObject>(textLayer, "text", 940, 600);
			auto floattext = uitext->AddComponent<FloatText>();
			auto text = uitext->AddComponent<Text>();
			text->SetText("CoolTime Down");

			break;
		}
		//緑色
		case 1:
		{
			//攻撃力の上昇
			player->SetShotDamage(player->GetShotDamage() + 0.4f);


			//テキストの生成
			auto uitext = engine->CreateUI<GameObject>(textLayer, "text", 940, 600);
			auto floattext = uitext->AddComponent<FloatText>();
			auto text = uitext->AddComponent<Text>();
			text->SetText("AttackPower Up");

			break;
		}
		//青色
		case 2:
		{
			//移動速度を上昇させる
			player->SetMoveSpeed(player->GetMoveSpeed() + 0.5f);

			//テキストの生成
			auto uitext = engine->CreateUI<GameObject>(textLayer, "text", 940, 600);
			auto floattext = uitext->AddComponent<FloatText>();
			auto text = uitext->AddComponent<Text>();
			text->SetText("MoveSpeed UP");


			break;
		}
		default:
			break;
		}

		//音の再生
		Audio::PlayOneShot(SE::powerup);

		//オブジェの破棄
		this->SetDeadFlg(true);
	}

}
