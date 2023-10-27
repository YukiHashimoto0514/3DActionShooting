//@file particle.vert

#version 450

//シェーダ―への入力
layout(location=0) in vec2 inPosition;
layout(location=1) in vec2 inTexcoord;

//シェーダ―からの出力
layout(location=0) out vec4 outColor;
layout(location=1) out vec2 outTexcoord;

//プログラムからの入力
layout(location=1) uniform mat4 matViewProj;
layout(location=2) uniform mat3 matBillboard;

//テクスチャ
layout(binding=0) uniform sampler2D texColor;

//テクスチャ座標構造体
struct Texcoord
{
	vec2 uv;	//texture座標
	vec2 Size;	//大きさ
};

//パーティクルデータ構造体
struct ParticleData		//.cppの引数と合わせる必要がある
{
	vec4 Position;		//座標(wは未使用)
	vec2 Scale;			//拡大縮小
	float cos;			//z軸回転のcos
	float sin;			//z軸回転のsin
	vec4 Color;			//色と不透明度
	Texcoord texcoord;	//テクスチャ座標
};

//パーティクル用SSBO
layout(std430,binding=0) buffer ParticleDataBlock
{
	ParticleData ParticleList[];
};

//エントリーポイント
void main()
{
	ParticleData particle = ParticleList[gl_InstanceID];

	//プリミティブの色を設定
	outColor = particle.Color;

	//透明度を0~1にする
	outColor.a = clamp(outColor.a,0,1);

	//プリミティブのtexture座標を計算
	outTexcoord = particle.texcoord.uv + (inTexcoord * particle.texcoord.Size);

	//プリミティブの大きさを画像の大きさと等しくする
	vec2 v = inPosition * abs(particle.texcoord.Size) * textureSize(texColor, 0);

	//プリミティブの拡大縮小
	v *= particle.Scale;

	//プリミティブを回転
	gl_Position.x = v.x * particle.cos - v.y * particle.sin;
	gl_Position.y = v.x * particle.sin + v.y * particle.cos;

	//プリミティブをカメラ方向に向ける（ビルボード化）
	gl_Position.z = 0;
	gl_Position.xyz = matBillboard * gl_Position.xyz;

	//プリミティブをワールド座標系に平行移動
	gl_Position.xyz += particle.Position.xyz;

	//プリミティブをワールド座標系からクリップ座標系へ変換
	gl_Position.w=1;
	gl_Position=matViewProj * gl_Position;



	//gl_Position = vec4(inPosition, 0, 1);
}