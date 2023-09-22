//@file DamageSource.h

#ifndef COMPONENT_DAMAGESOURCE_H_INCLUDED
#define COMPONENT_DAMAGESOURCE_H_INCLUDED
#include "../Engine/Component.h"
#include "../Application/GameObject.h"
#include "../Engine/VecMath.h"
#include "../Engine/Debug.h"

//ダメージ源コンポーネント
class DamageSource :public Component
{
public: 
	DamageSource() = default;//コンストラクタ
	virtual ~DamageSource() = default;//デストラクタ

	virtual void OnCollision(GameObject& gameObject, GameObject& other)override
	{
		//ターゲット名と同じ名前のゲームオブジェクトにダメージを与える
		if (other.name == targetName)
		{
			//無敵時間より小さいなら
			if (other.GetImmortalTime() <= 0)
			{
  				other.TakeDamage(gameObject, { amount,0 });//amount分HPを減らす
				
				//０番目は必ずメッシュレンダラー
				//auto& ra = static_cast<MeshRenderer&>(*other.componentList[0]);
				//for (int i = 0; i < ra.materials.size(); i++)
				//{
				//	//赤色に変更する
				//	ra.materials[i]->baseColor.x += 5.5;
				//}
			}

			//プレイヤーなら、無敵時間を設ける
			//if (other.name == "player")
			//{

			//	//無敵時間が無かったら
			//	if (other.GetImmortalTime() <= 0)
			//	{
			//		//無敵時間を設定
			//		other.SetImmortalTime(100);

			//		//プレイヤーのみの色変え
			//		//０番目は必ずメッシュレンダラー
			//		auto& ra = static_cast<MeshRenderer&>(*other.componentList[0]);
			//		for (int i = 0; i < ra.mesh->materials.size(); i++)
			//		{
			//			//赤色に変更する
			//			ra.mesh->materials[i]->baseColor.x += 1.5;
			//		}
			//	}
			//}

			if (isOnce)
			{
				gameObject.SetDeadFlg(true);
			}
		}
	}

	
	std::string targetName;	//ダメージを与えられるゲームオブジェクト名
	
	float amount = 1;		//ダメージ量
	bool isOnce = false;		//一度ダメージを与えたら死ぬのか
};




#endif  COMPONENT_DAMAGESOURCE_H_INCLUDED