//@file TitleScene.h
#ifndef TITLESCENE_H_INCLUDED
#define TITLESCENE_H_INCLUDED

#include "../Engine/VecMath.h"
#include "../Engine/Debug.h"
#include "../Application/Scene.h"
#include "../Engine/Engine.h"

using namespace VecMath;

//�ǂ�����I�����Ă��邩
enum class Select
{
	Tutorial,	//�`���[�g���A��
	Game,		//���C���Q�[��
};

//�^�C�g���R���|�[�l���g
class TitleScene :public Scene
{
public:
	TitleScene() = default;				//�R���X�g���N�^
	virtual ~TitleScene() = default;		//�f�X�g���N�^

	virtual bool Initialize(Engine& engine)override;				//������
	virtual void Update(Engine& engine, float deltaTime)override;	//�X�V����


private:
	float Timer = 0;
	bool Click = false;	//�{�^�������������ǂ���
	GameObjectPtr uiPressSpace;	//�v���X�X�y�[�X�e�L�X�g
	GameObjectPtr uiTutorial;	//�`���[�g���A���e�L�X�g
	GameObjectPtr uiGameStart;	//���C���Q�[���e�L�X�g

	GameObjectPtr uiMonster;	//�^�C�g���摜
	GameObjectPtr uiPanic;		//�^�C�g���摜

	//�ŏ��Ƀ`���[�g���A����I�����Ă���
	Select select = Select::Tutorial;
};




#endif //TitleScene_H_INCLUDED