//@file MainGameScene.h

#ifndef MAINGAMESCENE_H_INCLUDED
#define MAINGAMESCENE_H_INCLUDED

#include "Scene.h"
#include <vector>
#include "../Engine/Engine.h"
#include "../Component/TimeLimit.h"

//��s�錾
class Player;

using PlayerPtr = std::shared_ptr<Player>;

class MainGameScene :public Scene
{
public:
	GLFWwindow* window = nullptr;

	//�R���X�g���N�^
	MainGameScene() = default;

	//�f�X�g���N�^
	virtual ~MainGameScene() = default;

	virtual bool Initialize(Engine& engine)override;	//�������֐�
	virtual void Update(Engine& engine, float deltaTime)override;		//�X�V�֐�

	void CameraRot(GLFWwindow* window, GameObjectPtr camera, float deltaTime, double MouceX, double MouceY);
	void PlayerUpdate(Engine& engine, PlayerPtr player, float deltaTime);

	size_t mouce;			//�Ə��摜
	GameObjectPtr uimouce;	//�}�E�X�I�u�W�F

private:

	void PlayerInit(Engine& engine);	//�v���C���[�X�V
	void UIUpdate();	//UI�X�V

	GameObjectPtr BossObj;
	PlayerPtr PlayerObj;

	GameObjectPtr cameraobj;	//�J����
	GameObjectPtr Select;		//�I���摜�̃I�u�W�F�N�g
	GameObjectPtr Half[3];		//�w�i�̔������̃I�u�W�F�N�g
	GameObjectPtr Pistole;		//�s�X�g���摜�̃I�u�W�F�N�g��
	GameObjectPtr Assault;		//�A�T���g�摜�̃I�u�W�F�N�g��
	GameObjectPtr ShotGun;		//�V���b�g�K���摜�̃I�u�W�F�N�g��

	//����
	std::random_device rd;

	TimeLimitPtr TimeManager;	//�������Ԃ𑪂�}�l�[�W���[
	GameObjectPtr HPBarImg;		//HP�o�[�̉摜

	double MouceX = 0;	//�}�E�X�̃|�W�V����
	double MouceY = 0;	//�}�E�X�̃|�W�V����
	double oldX = 0;	//�}�E�X�̐̂̈ʒu
	double oldY = 0;	//�}�E�X�̐̂̈ʒu

	float EnemyCreate = 0;	//�G�l�~�[�𐶐�����N�[���^�C��
	float LimitTime = 1;	//�c�莞��

	const float UIMARGINE = 150.0f;	//UI�摜�̕�

	bool BossShow = false;//�{�X���o������(true=�o���@false=���o��) 

};

#endif //MAINGAMESCENE_H_INCLUDED