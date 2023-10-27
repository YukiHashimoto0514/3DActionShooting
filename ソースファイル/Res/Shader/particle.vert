//@file particle.vert

#version 450

//�V�F�[�_�\�ւ̓���
layout(location=0) in vec2 inPosition;
layout(location=1) in vec2 inTexcoord;

//�V�F�[�_�\����̏o��
layout(location=0) out vec4 outColor;
layout(location=1) out vec2 outTexcoord;

//�v���O��������̓���
layout(location=1) uniform mat4 matViewProj;
layout(location=2) uniform mat3 matBillboard;

//�e�N�X�`��
layout(binding=0) uniform sampler2D texColor;

//�e�N�X�`�����W�\����
struct Texcoord
{
	vec2 uv;	//texture���W
	vec2 Size;	//�傫��
};

//�p�[�e�B�N���f�[�^�\����
struct ParticleData		//.cpp�̈����ƍ��킹��K�v������
{
	vec4 Position;		//���W(w�͖��g�p)
	vec2 Scale;			//�g��k��
	float cos;			//z����]��cos
	float sin;			//z����]��sin
	vec4 Color;			//�F�ƕs�����x
	Texcoord texcoord;	//�e�N�X�`�����W
};

//�p�[�e�B�N���pSSBO
layout(std430,binding=0) buffer ParticleDataBlock
{
	ParticleData ParticleList[];
};

//�G���g���[�|�C���g
void main()
{
	ParticleData particle = ParticleList[gl_InstanceID];

	//�v���~�e�B�u�̐F��ݒ�
	outColor = particle.Color;

	//�����x��0~1�ɂ���
	outColor.a = clamp(outColor.a,0,1);

	//�v���~�e�B�u��texture���W���v�Z
	outTexcoord = particle.texcoord.uv + (inTexcoord * particle.texcoord.Size);

	//�v���~�e�B�u�̑傫�����摜�̑傫���Ɠ���������
	vec2 v = inPosition * abs(particle.texcoord.Size) * textureSize(texColor, 0);

	//�v���~�e�B�u�̊g��k��
	v *= particle.Scale;

	//�v���~�e�B�u����]
	gl_Position.x = v.x * particle.cos - v.y * particle.sin;
	gl_Position.y = v.x * particle.sin + v.y * particle.cos;

	//�v���~�e�B�u���J���������Ɍ�����i�r���{�[�h���j
	gl_Position.z = 0;
	gl_Position.xyz = matBillboard * gl_Position.xyz;

	//�v���~�e�B�u�����[���h���W�n�ɕ��s�ړ�
	gl_Position.xyz += particle.Position.xyz;

	//�v���~�e�B�u�����[���h���W�n����N���b�v���W�n�֕ϊ�
	gl_Position.w=1;
	gl_Position=matViewProj * gl_Position;



	//gl_Position = vec4(inPosition, 0, 1);
}