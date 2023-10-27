//@file gaussian_blur.frag

#version 450

//シェーダーへの入力
layout(location = 1) in vec2 inTexcoord;

//テクスチャ
layout(binding = 0) uniform sampler2D texColor;

//プログラムからの入力
layout(location = 100) uniform float blurRatio;//ぼかし効果（0 ~ 1）

//出力する色データ
out vec4 outColor;

//エントリーポイント
void main()
{
	float offset[] = { -5.176, -3.294, -1.412, 0, 1.412, 3.294, 5.176 };
	float weight[] = { 0.010, 0.094, 0.297 , 0.196, 0.297, 0.094, 0.010 };

	vec2 invTexelSize = 1 / vec2(textureSize(texColor, 0));

	vec3 blurColor = vec3(0);
	for(int y = 0;y < 7;y++)
	{
		for(int x = 0;x < 7;x++)
		{
			vec2 o = vec2(offset[x], offset[y]) * invTexelSize;
			float w = weight[x] * weight[y];
			blurColor.rgb += texture(texColor, inTexcoord + o).rgb * w;
		}
	}

	vec3 baseColor =texture(texColor, inTexcoord).rgb;
	outColor = vec4(mix(baseColor, blurColor, blurRatio), 1);


}