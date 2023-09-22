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

	size_t mouce;
	GameObjectPtr uimouce;

private:

	void PlayerInit(Engine& engine);

	GameObjectPtr BossObj;
	PlayerPtr PlayerObj;

	GameObjectPtr cameraobj;


	//����
	std::random_device rd;

	TimeLimitPtr TimeManager;	//�������Ԃ𑪂�}�l�[�W���[
	GameObjectPtr HPBarImg;		//HP�o�[�̉摜

	double MouceX, MouceY;		//�}�E�X�̃|�W�V����
	double oldX, oldY;			//�}�E�X�̐̂̈ʒu

	float enemycreate = 0;	//�G�l�~�[�𐶐�����N�[���^�C��
	float LimitTime = 180;	//�c�莞��

	bool BossShow = false;//�{�X���o������(true=�o���@false=���o��)

};

#endif //MAINGAMESCENE_H_INCLUDED