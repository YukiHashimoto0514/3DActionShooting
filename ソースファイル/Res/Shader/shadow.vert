//@file shadow.vert

#version 450

//シェーダーへの入力
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexcoord;

//シェーダーからの出力
layout(location = 1) out vec2 outTexcoord;

//プログラムからの出力
layout(location = 0) uniform mat4 matModel;
layout(location = 1) uniform mat4 matShadow;

void main()
{
	outTexcoord = inTexcoord;
	gl_Position = matShadow * (matModel * vec4(inPosition, 1));
}