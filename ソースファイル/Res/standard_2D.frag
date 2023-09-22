
//@file standard_2D.frag

#version 450

//シェーダーへの入力
layout(location=0) in vec4 inColor;
layout(location=1) in vec2 inTexcoord;

//テクスチャサンプラ
layout(binding=0) uniform sampler2D texColor;

//出力する色データ
out vec4 outColor;	//変数宣言（out=修飾子＜ー書き換える時につける）

void main()
{
	vec4 c=texture(texColor,inTexcoord);
	outColor=c*inColor;
}