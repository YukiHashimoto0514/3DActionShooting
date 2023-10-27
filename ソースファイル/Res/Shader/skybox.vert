//@file skybox.vert

#version 450

//シェーダ―への入力
layout(location=0) in vec3 inPosition;

//シェーダ―からの出力
layout(location=0) out vec3 outTexcoord;

//プログラムからの入力

//x:視野角による水平拡大率
//y:視野角による垂直拡大率
//z:遠近法パラメータA
//w:遠近法パラメータB
layout(location=3) uniform vec4 CameraData;

//x:カメラx軸回転のsin
//y:カメラx軸回転のcos
//z:カメラy軸回転のsin
//w:カメラy軸回転のcos
layout(location=5) uniform vec4 CameraSinCosXY;

//エントリーポイント
void main()
{
	//頂点座標をキューブマップのテクスチャ座標とする
	outTexcoord = inPosition;

	//カメラの回転をモデルに反映するには、カメラの回転と逆方向に回転させる
	//標準の回転方向は半時計回りなので、時計回りになるようにする
	vec3 pos = inPosition * 2;

	//Y軸回転(x,z平面)
	float sY = -CameraSinCosXY.z;	//時計回りに変換
	float cY = CameraSinCosXY.w;

	pos=vec3(pos.x * cY + pos.z * sY,
			 pos.y,
			 pos.x * -sY + pos.z * cY);

	//X軸回転(z,y平面)
	float sX = -CameraSinCosXY.x;	//時計回りに変換
	float cX = CameraSinCosXY.y;

	pos=vec3(pos.x,
			 pos.z * -sX + pos.y * cX,
			 pos.z * cX + pos.y * sX);

	//視野角を反映
	gl_Position.x = pos.x * CameraData.x;
	gl_Position.y = pos.y * CameraData.y;
	gl_Position.z = -pos.z * CameraData.w + CameraData.z;
	gl_Position.w = -pos.z;
}
