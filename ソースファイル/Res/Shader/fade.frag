//@file fade.frag

#version 450

//シェーダ―への入力
layout(location = 1) in vec2 inTexcoord;

//テクスチャ
layout(binding = 0) uniform sampler2D texColor;	//カラー画像
layout(binding = 1) uniform sampler2D texRule;	//ルール画像

//ピクセルを破棄する閾値
layout(location = 100) uniform float Threshold;

//出力する色データ
out vec4 outColor;

void main()
{
	//ルール画像の値が閾値未満だったら、描画しない
	float value = texture(texRule,inTexcoord).r;

	if(value < Threshold)
	{
		discard;
	}

	//指定された画像を描画
	outColor = texture(texColor, inTexcoord);

	//境界部分のぎざぎざ感をなくすため、透明度をなめらかに変化させる
	const float SmoothRange = 0.02;
	outColor.a *= smoothstep(Threshold - SmoothRange, Threshold ,value);
}