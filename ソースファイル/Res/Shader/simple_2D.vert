//@file simple_2D.vert

#version 450

//シェーダーへの入力
layout(location=0) in vec3 inPosition;
layout(location=1) in vec2 inTexcoord;

//シェーダーからの出力
layout(location=0) out vec3 outPosition;
layout(location=1) out vec2 outTexcoord;

//プログラムからの入力
layout(location=0) uniform mat4 matModel;
layout(location=1) uniform mat4 matVewProjection;

//エントリーポイント
void main()
{
	outTexcoord = inTexcoord;
	outPosition = vec3(matModel * vec4(inPosition, 1));
	gl_Position = matVewProjection * vec4(outPosition, 1);
}