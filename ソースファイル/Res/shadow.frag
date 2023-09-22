//@file shadow.frag

#version 450

//シェーダーへの入力
layout(location = 1) in vec2 inTexcoord;

//テクスチャ
layout(binding = 0) uniform sampler2D texColor;

//マテリアル
struct Material
{
	vec4 baseColor;	//基本色＋アルファ

	//x:鏡面反射指数
	//y:正規化係数
	vec2 specularFactor;
};
layout(location = 102) uniform Material material;

void main()
{
	float alpha = texture(texColor, inTexcoord).a * material.baseColor.a;

	if(alpha < 0.5)
	{
		discard;	//無視
	}
}
