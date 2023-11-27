//@file standard_3D.frag

#version 450

//シェーダーへの入力
layout(location=0) in vec3 inposition;
layout(location=1) in vec2 inTexcoord;
layout(location=2) in vec3 inNormal;
layout(location=3) in vec4 inTangent;
layout(location=4) in vec3 inShadowPosition;

//テクスチャ
layout(binding=0) uniform sampler2D texColor;
layout(binding=1) uniform sampler2D texNormal;
layout(binding=2) uniform sampler2DShadow texDepth;

//出力する色データ
out vec4 outColor;

layout(location=4) uniform vec3 cameraPosition;	//カメラ座標

//平行光源パラメータ
struct DirectionalLight
{
	vec3 color;		//ライトの明るさ
	vec3 direction;	//ライトの向き
};

layout(location=100) uniform DirectionalLight directionalLight;

//マテリアル
struct Material
{
	vec4 baseColor;//基本色＋アルファ

	//x:鏡面反射指数
	//y:正規化係数
	vec3 specularFactorAndReceiveShadows;
};
layout(location=102) uniform Material material;

layout(location=104) uniform vec3 ambientLight;	//環境光パラメータ

//法線を計算する
vec3 ComputeWorldNormal()
{
	vec3 normal = texture(texNormal, inTexcoord).rgb;

	//値が小さすぎる場合、法線テクスチャが設定されていないと判断し、頂点法線を返す
	if (dot(normal, normal) <= 0.0001) 
	{
		return normalize(inNormal);
	}

	//8bit値であることを考慮しつつ0~1を-1~1に変換（128を0とみなす）
	normal = normal * (255.0 / 127.0) - (128.0 / 127.0);

	//今のままだと、タンジェント座標系にあるので、
	//法線テクスチャの値をワールド空間に変換
	vec3 bitangent = inTangent.w * cross(inNormal, inTangent.xyz);

	return normalize(
	inTangent.xyz * normal.x + 
	bitangent * normal.y +
	inNormal * normal.z);
}

void main()
{

	outColor = texture(texColor,inTexcoord) * material.baseColor;

	//透明と判断されるフラグメントは描画しない
	if(outColor.a < 0.5)	//閾値は適宜変更予定
	{
		discard;
	}

	float invPi = 1 / acos(-1);	//πの逆数

	//法線を計算
	vec3 normal = ComputeWorldNormal();

	//ランベルトの余弦則を使って明るさを計算
	float theta = max(dot(-directionalLight.direction, normal), 0);

	//拡散光の明るさを計算
	vec3 diffuse = directionalLight.color * theta * invPi;

	//鏡面反射パラメータを取得
	float specularPower = material.specularFactorAndReceiveShadows.x;
	float normalizeFactor = material.specularFactorAndReceiveShadows.y;
	float receiveShadows = material.specularFactorAndReceiveShadows.z;

	//周りからの影の影響を受けないようにする(ついでにライティングも)
	if (receiveShadows <= 0)
	{
	  return;
	}

	//ライトとカメラの中間の向きを求める
	vec3 cameraVector = normalize(cameraPosition - inposition);
	vec3 halfVector = normalize(-directionalLight.direction + cameraVector);

	//正規化Blinn-Phong法によって鏡面反射の明るさを計算
	float dotNH = max(dot(normal , halfVector) , 0);
	vec3 specular = directionalLight.color;
	specular *= normalizeFactor * pow(dotNH , specularPower) * theta;

	//環境光を計算
	vec3 ambient = outColor.rgb * ambientLight;

	//光が届いている比率を計算								影を半透明にする
	float shadow = texture(texDepth, inShadowPosition)* 0.75 + 0.25;

	//拡散光と鏡面反射を合成する
	float specularRatio = 0.04f;	//鏡面反射の比率
	diffuse *= outColor.rgb * (1 - specularRatio);
	specular *= specularRatio;
	outColor.rgb = (diffuse + specular) * shadow + ambient;

	//影の色を変えれる
    //outColor.rgb += (1 - shadow) * vec3(0.5, 0, 0);
}



