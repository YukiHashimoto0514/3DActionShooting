//@file GameOverScene.h
#ifndef GAMEOVERSCENE_H_INCLUDED
#define GAMEOVERSCENE_H_INCLUDED

#include "../Engine/VecMath.h"
#include "../Engine/Debug.h"
#include "../Application/Scene.h"
#include "../Engine/Engine.h"

using namespace VecMath;

//�Q�[���I�[�o�[�V�[���R���|�[�l���g
class GameOverScene :public Scene
{
public:
	GameOverScene() = default;				//�R���X�g���N�^
	virtual ~GameOverScene() = default;		//�f�X�g���N�^

	virtual bool Initialize(Engine& engine)override;				//������
	virtual void Update(Engine& engine, float deltaTime)override;	//�X�V����


private:
	float Timer = 0;

	int Action = 0;
	bool ClickEnter = false;//�G���^�[�����������ǂ���
	GameObjectPtr uiPressEnter;
	GameObjectPtr uiGameover;

};




#endif //GameOverScene_H_INCLUDED