//@file DamageSource.h

#ifndef COMPONENT_DAMAGESOURCE_H_INCLUDED
#define COMPONENT_DAMAGESOURCE_H_INCLUDED
#include "../Engine/Component.h"
#include "../Application/GameObject.h"
#include "../Engine/VecMath.h"
#include "../Engine/Debug.h"

//�_���[�W���R���|�[�l���g
class DamageSource :public Component
{
public: 
	DamageSource() = default;//�R���X�g���N�^
	virtual ~DamageSource() = default;//�f�X�g���N�^

	virtual void OnCollision(GameObject& gameObject, GameObject& other)override
	{
		//�^�[�Q�b�g���Ɠ������O�̃Q�[���I�u�W�F�N�g�Ƀ_���[�W��^����
		if (other.name == targetName)
		{
			//���G���Ԃ�菬�����Ȃ�
			if (other.GetImmortalTime() <= 0)
			{
  				other.TakeDamage(gameObject, { amount,0 });//amount��HP�����炷
				
				//�O�Ԗڂ͕K�����b�V�������_���[
				//auto& ra = static_cast<MeshRenderer&>(*other.componentList[0]);
				//for (int i = 0; i < ra.materials.size(); i++)
				//{
				//	//�ԐF�ɕύX����
				//	ra.materials[i]->baseColor.x += 5.5;
				//}
			}

			//�v���C���[�Ȃ�A���G���Ԃ�݂���
			//if (other.name == "player")
			//{

			//	//���G���Ԃ�����������
			//	if (other.GetImmortalTime() <= 0)
			//	{
			//		//���G���Ԃ�ݒ�
			//		other.SetImmortalTime(100);

			//		//�v���C���[�݂̂̐F�ς�
			//		//�O�Ԗڂ͕K�����b�V�������_���[
			//		auto& ra = static_cast<MeshRenderer&>(*other.componentList[0]);
			//		for (int i = 0; i < ra.mesh->materials.size(); i++)
			//		{
			//			//�ԐF�ɕύX����
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

	
	std::string targetName;	//�_���[�W��^������Q�[���I�u�W�F�N�g��
	
	float amount = 1;		//�_���[�W��
	bool isOnce = false;		//��x�_���[�W��^�����玀�ʂ̂�
};




#endif  COMPONENT_DAMAGESOURCE_H_INCLUDED