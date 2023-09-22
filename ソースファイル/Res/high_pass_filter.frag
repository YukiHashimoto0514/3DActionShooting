//@file high_pass_filter.frag

#version 450

//シェーダーへの入力
layout(location=1) in vec2 inTexcoord;

//テクスチャ
layout(binding=0) uniform sampler2D texColor;

//プログラムからの入力
//x : 高輝度とみなす明るさ（閾値）
//y : ブルームの強さ
layout(location=100) uniform vec2 bloomParam;

//出力する色データ
out vec4 outColor;

//高輝度ピクセルの色成分を取得する
vec3 GetBrightnessPart(vec3 color)
{
	//RGBのうち、もっとも明るい要素の値をピクセルの輝度とする
	float brightness = max(color.r, max(color.g, color.b));

	//高輝度成分の比率を計算
	float threshold = bloomParam.x;
	float ratio = max(brightness - threshold, 0) / max(brightness, 0.00001);

	//高輝度成分を計算
	return color * ratio;
}

//エントリーポイント
void main()
{
	//テクセルサイズを計算
	vec2 oneTexel = 1 / vec2(textureSize(texColor, 0));

	//明るい成分を計算し、4x4ピクセルの縮小ぼかし処理を行う
	outColor.rgb = GetBrightnessPart(texture(texColor, inTexcoord).rgb) * 4;

	outColor.rgb += GetBrightnessPart(
		texture(texColor, inTexcoord + vec2(-oneTexel.x,  oneTexel.y)).rgb);

	outColor.rgb += GetBrightnessPart(
		texture(texColor, inTexcoord + vec2( oneTexel.x,  oneTexel.y)).rgb);

	outColor.rgb += GetBrightnessPart(
		texture(texColor, inTexcoord + vec2(-oneTexel.x, -oneTexel.y)).rgb);

	outColor.rgb += GetBrightnessPart(
		texture(texColor, inTexcoord + vec2( oneTexel.x, -oneTexel.y)).rgb);

	//ブルームのつよさを乗算
	float strength = bloomParam.y;
	outColor.rgb *= (1.0 / 8.0) * strength;
	outColor.a = 1;

}