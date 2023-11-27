//@file standard_3D.frag

#version 450

//�V�F�[�_�[�ւ̓���
layout(location=0) in vec3 inposition;
layout(location=1) in vec2 inTexcoord;
layout(location=2) in vec3 inNormal;
layout(location=3) in vec4 inTangent;
layout(location=4) in vec3 inShadowPosition;

//�e�N�X�`��
layout(binding=0) uniform sampler2D texColor;
layout(binding=1) uniform sampler2D texNormal;
layout(binding=2) uniform sampler2DShadow texDepth;

//�o�͂���F�f�[�^
out vec4 outColor;

layout(location=4) uniform vec3 cameraPosition;	//�J�������W

//���s�����p�����[�^
struct DirectionalLight
{
	vec3 color;		//���C�g�̖��邳
	vec3 direction;	//���C�g�̌���
};

layout(location=100) uniform DirectionalLight directionalLight;

//�}�e���A��
struct Material
{
	vec4 baseColor;//��{�F�{�A���t�@

	//x:���ʔ��ˎw��
	//y:���K���W��
	vec3 specularFactorAndReceiveShadows;
};
layout(location=102) uniform Material material;

layout(location=104) uniform vec3 ambientLight;	//�����p�����[�^

//�@�����v�Z����
vec3 ComputeWorldNormal()
{
	vec3 normal = texture(texNormal, inTexcoord).rgb;

	//�l������������ꍇ�A�@���e�N�X�`�����ݒ肳��Ă��Ȃ��Ɣ��f���A���_�@����Ԃ�
	if (dot(normal, normal) <= 0.0001) 
	{
		return normalize(inNormal);
	}

	//8bit�l�ł��邱�Ƃ��l������0~1��-1~1�ɕϊ��i128��0�Ƃ݂Ȃ��j
	normal = normal * (255.0 / 127.0) - (128.0 / 127.0);

	//���̂܂܂��ƁA�^���W�F���g���W�n�ɂ���̂ŁA
	//�@���e�N�X�`���̒l�����[���h��Ԃɕϊ�
	vec3 bitangent = inTangent.w * cross(inNormal, inTangent.xyz);

	return normalize(
	inTangent.xyz * normal.x + 
	bitangent * normal.y +
	inNormal * normal.z);
}

void main()
{

	outColor = texture(texColor,inTexcoord) * material.baseColor;

	//�����Ɣ��f�����t���O�����g�͕`�悵�Ȃ�
	if(outColor.a < 0.5)	//臒l�͓K�X�ύX�\��
	{
		discard;
	}

	float invPi = 1 / acos(-1);	//�΂̋t��

	//�@�����v�Z
	vec3 normal = ComputeWorldNormal();

	//�����x���g�̗]�������g���Ė��邳���v�Z
	float theta = max(dot(-directionalLight.direction, normal), 0);

	//�g�U���̖��邳���v�Z
	vec3 diffuse = directionalLight.color * theta * invPi;

	//���ʔ��˃p�����[�^���擾
	float specularPower = material.specularFactorAndReceiveShadows.x;
	float normalizeFactor = material.specularFactorAndReceiveShadows.y;
	float receiveShadows = material.specularFactorAndReceiveShadows.z;

	//���肩��̉e�̉e�����󂯂Ȃ��悤�ɂ���(���łɃ��C�e�B���O��)
	if (receiveShadows <= 0)
	{
	  return;
	}

	//���C�g�ƃJ�����̒��Ԃ̌��������߂�
	vec3 cameraVector = normalize(cameraPosition - inposition);
	vec3 halfVector = normalize(-directionalLight.direction + cameraVector);

	//���K��Blinn-Phong�@�ɂ���ċ��ʔ��˂̖��邳���v�Z
	float dotNH = max(dot(normal , halfVector) , 0);
	vec3 specular = directionalLight.color;
	specular *= normalizeFactor * pow(dotNH , specularPower) * theta;

	//�������v�Z
	vec3 ambient = outColor.rgb * ambientLight;

	//�����͂��Ă���䗦���v�Z								�e�𔼓����ɂ���
	float shadow = texture(texDepth, inShadowPosition)* 0.75 + 0.25;

	//�g�U���Ƌ��ʔ��˂���������
	float specularRatio = 0.04f;	//���ʔ��˂̔䗦
	diffuse *= outColor.rgb * (1 - specularRatio);
	specular *= specularRatio;
	outColor.rgb = (diffuse + specular) * shadow + ambient;

	//�e�̐F��ς����
    //outColor.rgb += (1 - shadow) * vec3(0.5, 0, 0);
}



