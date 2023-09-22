
//@file standard_2D.vert

#version 450

//シェーダーへの入力
layout(location=0) in vec3 inPosition;//頂点座標(in＜ー相手からもらうときにつける)
layout(location=1) in vec2 inTexcoord;

//プログラムからの入力
layout(location=0) uniform float timer;
layout(location=1) uniform vec3 position;

//シェーダーからの出力
layout(location=0) out vec4 outColor;//頂点色
layout(location=1) out vec2 outTexcoord;

//画面座標系をクリップ座標系に変換するためのパラメータ
layout(location=2) uniform vec4 screenToClip;

//テクスチャ
layout(binding=0) uniform sampler2D texColor;	//画像の大きさ

//テクスチャ座標構造体
struct Texcoord
{
vec2 uv;
vec2 size;
};


//画像表示用データ（スプライト）を格納する構造体
struct Sprite
{
	vec4 position;
	Texcoord texcoord;
	vec4 variar;
};

//スプライト用SSBO(シェーダーストレージバッファオブジェクト)
layout(std430,binding=0) buffer spriteDataBlock
{
	Sprite spriteList[];
};


void main()
{


	outColor=vec4(spriteList[gl_InstanceID].variar.y,spriteList[gl_InstanceID].variar.z,spriteList[gl_InstanceID].variar.w,spriteList[gl_InstanceID].position.w);

	//テクスチャ座標を計算
	Texcoord texcoord=spriteList[gl_InstanceID].texcoord;
	outTexcoord=texcoord.uv+(inTexcoord*texcoord.size);

	//角度
	float rad=spriteList[gl_InstanceID].position.z*3.14/180;

//	cos(rad);
//	sin(rad);

	//頂点座標を計算
	gl_Position.x = inPosition.x*cos(rad)-inPosition.y*sin(rad);	//回転処理
	gl_Position.y = inPosition.x*sin(rad)+inPosition.y*cos(rad);	//回転処理
	gl_Position.z = inPosition.z;
	gl_Position.w = 1;

	//図形の大きさを画像の大きさと等しくする
	gl_Position.xy *= abs(texcoord.size) * textureSize(texColor, 0) * spriteList[gl_InstanceID].variar.x;	//ピクセルに変換

	//図形をSSBOの座標に移動
	gl_Position.x += spriteList[gl_InstanceID].position.x;
	gl_Position.y += spriteList[gl_InstanceID].position.y;

	//画面座標系からクリップ座標系へ変換
	gl_Position.xy=(gl_Position.xy*screenToClip.xy)+screenToClip.zw;
}