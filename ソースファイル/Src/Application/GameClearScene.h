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
	float Timer = 0;

	int Action = 0;

	GameObjectPtr uiPressSpace;
	GameObjectPtr uiGameClear;

};




#endif //GameClearScene_H_INCLUDED