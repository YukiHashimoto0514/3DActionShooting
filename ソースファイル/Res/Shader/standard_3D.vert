//@file standard_3D.vert

#version 450

//シェーダーへの入力
layout(location=0) in vec3 inPosition;
layout(location=1) in vec2 inTexcoord;
layout(location=2) in vec3 inNormal;
layout(location=3) in vec4 inTangent;


//シェーダーからの出力
layout(location=0) out vec3 outPosition;
layout(location=1) out vec2 outTexcoord;
layout(location=2) out vec3 outNormal;
layout(location=3) out vec4 outTangent;
layout(location=4) out vec3 outShadowPosition;

//テクスチャ
layout(binding=0) uniform sampler2D texColor;
//番号をそろえるため１を省き
layout(binding=2) uniform sampler2DShadow texDepth;

//プログラムからの入力
layout(location=0) uniform mat4 matModel;
layout(location=1) uniform mat4 matShadowTexture;

//x:視野角による水平拡大率
//y:視野角による垂直拡大率
//z:遠近法パラメーターA
//w:遠近法パラメーターB
layout(location=3) uniform vec4 cameraData;
layout(location=4) uniform vec3 cameraPosition;	//カメラの座標
layout(location=5) uniform vec2 cameraSinCosX;	//カメラのX軸回転
layout(location=6) uniform vec2 cameraSinCosY;	//カメラのY軸回転




void main()
{

	//拡大縮小と回転と平行移動
	outPosition= vec3(matModel * vec4(inPosition, 1));

	//ワールド法線を計算
	mat3 matNormal = transpose(inverse(mat3(matModel)));
	outNormal = normalize(matNormal * inNormal);

	//ワールドタンジェントを計算
	outTangent.xyz = normalize(matNormal * inTangent.xyz);	//向きベクトル計算
	outTangent.w = inTangent.w;

	//影判定用の座標を計算
	float worldShadowRange = 750 * 2;	//深度マップの描画範囲(CreateDepthMap関数を参照)
	float shadowTexelSize = 1 / float(textureSize(texDepth, 0).x);	//1テクセルサイズ
	float worldShadowTexelSize =	
		worldShadowRange * shadowTexelSize;	//１テクセルのサイズ（ワールド座標）

	outShadowPosition = vec3(matShadowTexture *
		vec4(outPosition + outNormal * worldShadowTexelSize, 1));

	//カメラ座標が原点となるように平行移動
	vec3 pos = outPosition - cameraPosition;

	//カメラの右方向がx軸、正面がz軸となるように回転
	float sx = -cameraSinCosX.x;	//回転方向を反時計回りから時計回りに変換
	float cx =  cameraSinCosX.y;

	float sy = -cameraSinCosY.x;
	float cy =  cameraSinCosY.y;

	//y軸回転
	pos = vec3( pos.x * cy + pos.z * sy,
				pos.y,
				pos.x * -sy + pos.z * cy);
	//x軸回転
	pos = vec3( pos.x,
				pos.z * -sx + pos.y * cx,
				pos.z * cx + pos.y * sx);



	outTexcoord = inTexcoord;

	gl_Position.x = pos.x * cameraData.x;
	gl_Position.y = pos.y * cameraData.y;
	gl_Position.z = -pos.z * cameraData.w + cameraData.z;

	gl_Position.w = -pos.z;

}

