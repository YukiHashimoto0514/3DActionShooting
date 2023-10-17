//@file GameClearScene.h
#ifndef GAMECLEARSCENE_H_INCLUDED
#define GAMECLEARSCENE_H_INCLUDED

#include "../Engine/VecMath.h"
#include "../Engine/Debug.h"
#include "../Application/Scene.h"
#include "../Engine/Engine.h"

using namespace VecMath;

//�^�[�Q�b�g��ǂ�������R���|�[�l���g
class GameClearScene :public Scene
{
public:
	GameClearScene() = default;				//�R���X�g���N�^
	virtual ~GameClearScene() = default;		//�f�X�g���N�^

	virtual bool Initialize(Engine& engine)override;				//������
	virtual void Update(Engine& engine, float deltaTime)override;	//�X�V����


private:
	float Timer = 0;	//�C�[�W���O�Ŏg���^�C�}�[

	bool ClickEnter = false;	//��x�N���b�N�������ǂ���

	GameObjectPtr uiPressSpace;	//�e�L�X�g�I�u�W�F�N�g
	GameObjectPtr uiGameClear;	//�C���[�W�I�u�W�F�N�g

};




#endif //GameClearScene_H_INCLUDED