//@file particle.frag

#version 450

//シェーダ―への入力
layout(location=0) in vec4 inColor;
layout(location=1) in vec2 inTexcoord;

//テクスチャ
layout(binding=0) uniform sampler2D texColor;

//出力する色データ
out vec4 outColor;

void main()
{
	outColor = inColor * texture(texColor,inTexcoord);
}
