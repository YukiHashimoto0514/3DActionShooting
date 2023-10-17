//@file TutorialScene.h

#ifndef TUTORIALSCENE_H_INCLUDED
#define TUTORIALSCENE_H_INCLUDED
#include "Scene.h"
#include "../Component/Text.h"
#include "../Engine/Engine.h"

//�`���[�g���A���V�[��

class TutorialScene :public Scene
{
public:

	GLFWwindow* window = nullptr;

	TutorialScene() = default;				//�R���X�g���N�^
	virtual ~TutorialScene() = default;		//�f�X�g���N�^

	virtual bool Initialize(Engine& engine)override;
	virtual void Update(Engine& engine, float deltaTime)override;

	//�J�����̉�]
	void CameraRot(GLFWwindow* window, GameObjectPtr camera, float deltaTime, double MouceX, double MouceY);

	//�v���C���[�A�b�v�f�[�g
	void PlayerUpdate(Engine& engine, PlayerPtr player, float deltaTime);

	//UI�X�V
	void UIUpdate();

	PlayerPtr PlayerObj;
	GameObjectPtr cameraobj;	//�J����
	GameObjectPtr Select;		//�I���摜�̃I�u�W�F�N�g
	GameObjectPtr Pistole;		//�s�X�g���摜�̃I�u�W�F�N�g��
	GameObjectPtr Assault;		//�A�T���g�摜�̃I�u�W�F�N�g��
	GameObjectPtr ShotGun;		//�V���b�g�K���摜�̃I�u�W�F�N�g��

	double MouceX, MouceY;		//�}�E�X�̃|�W�V����
	double oldX, oldY;			//�}�E�X�̐̂̈ʒu
	const float UIMARGINE = 150.0f;	//UI�摜�̕�


};

#endif //TUTORIALSCENE_H_INCLUDED
