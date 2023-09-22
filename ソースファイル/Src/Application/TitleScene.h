//@file TitleScene.h
#ifndef TITLESCENE_H_INCLUDED
#define TITLESCENE_H_INCLUDED

#include "../Engine/VecMath.h"
#include "../Engine/Debug.h"
#include "../Application/Scene.h"
#include "../Engine/Engine.h"

using namespace VecMath;

//�^�C�g���R���|�[�l���g
class TitleScene :public Scene
{
public:
	TitleScene() = default;				//�R���X�g���N�^
	virtual ~TitleScene() = default;		//�f�X�g���N�^

	virtual bool Initialize(Engine& engine)override;				//������
	virtual void Update(Engine& engine, float deltaTime)override;	//�X�V����


private:

	bool Click = false;//�{�^�������������ǂ���
	GameObjectPtr uiPressSpace;
	GameObjectPtr uiTitle;

};




#endif //TitleScene_H_INCLUDED