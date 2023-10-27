//@file skybox.frag

#version 450

//シェーダ―への入力
layout(location=0) in vec3 inTexcoord;//キューブの中心から外に向かうベクトル

//テクスチャ
layout(binding=0) uniform samplerCube texSkyBox;

//出力する色データ
out vec4 outColor;

//エントリーポイント
void main()
{
	outColor = texture(texSkyBox, inTexcoord);
}