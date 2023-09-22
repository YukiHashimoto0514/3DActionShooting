//@file Camera.h

#ifndef COMPONENT_CAMERA_H_INCLUDED
#define COMPONENT_CAMERA_H_INCLUDED

#include "../Engine/Component.h"
#include <math.h>

//ビューポート
struct Viewport
{
	int x;		//左下ｘ座標
	int y;		//左下ｙ座標
	int width;	//幅
	int height;	//高さ
};

//カメラコンポーネント
class Camera : public Component
{
public:
	Camera() = default;				//コンストラクタ
	virtual ~Camera() = default;	//デストラクタ


	virtual void Update(GameObject& gameObject, float deltaTime)override
	{
		//遠近法パラメーターA,Bを計算
		A = -2 * far * near / (far - near);
		B = (far + near) / (far - near);

		//視野角による拡大率を計算
		const float rad = fovY * acos(-1.0f) / 180;
		fovScaleY = 1 / tan(rad / 2);
		fovScaleX = aspect * fovScaleY;
	}

	float near = 1;			//近平面
	float far = 2000;		//遠平面
	float fovY = 60;		//視野角
	float aspect = 9.0f / 16.0f;	//アスペクト比
	Viewport viewport = { 0,0,1280,720 };

	//ユニフォーム変数に設定するパラメーター
	float fovScaleX = 1;	//視野角による拡大率
	float fovScaleY = 1;	//視野角による拡大率
	float A = 0.01f;		//遠近法パラメーターA
	float B = 0.99f;		//遠近法パラメーターB

};

#endif //COMPONENT_CAMERA_H_INCLUDED




