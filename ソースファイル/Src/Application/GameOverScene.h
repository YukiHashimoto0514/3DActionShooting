//@file GameOverScene.h
#ifndef GAMEOVERSCENE_H_INCLUDED
#define GAMEOVERSCENE_H_INCLUDED

#include "../Engine/VecMath.h"
#include "../Engine/Debug.h"
#include "../Application/Scene.h"
#include "../Engine/Engine.h"

using namespace VecMath;

//�ǂ�����I�����Ă��邩
enum class Choose
{
	Game,	//�Q�[��
	Title,	//�^�C�g��
};

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
	GameObjectPtr uiPressSpace;
	GameObjectPtr uiGameover;
	GameObjectPtr uiTitle;		//�����Ƃ�e�L�X�g
	GameObjectPtr uiGameStart;	//���C���Q�[���e�L�X�g

	Choose select = Choose::Game;
};




#endif //GameOverScene_H_INCLUDED