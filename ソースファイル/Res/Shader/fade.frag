//@file fade.frag

#version 450

//�V�F�[�_�\�ւ̓���
layout(location = 1) in vec2 inTexcoord;

//�e�N�X�`��
layout(binding = 0) uniform sampler2D texColor;	//�J���[�摜
layout(binding = 1) uniform sampler2D texRule;	//���[���摜

//�s�N�Z����j������臒l
layout(location = 100) uniform float Threshold;

//�o�͂���F�f�[�^
out vec4 outColor;

void main()
{
	//���[���摜�̒l��臒l������������A�`�悵�Ȃ�
	float value = texture(texRule,inTexcoord).r;

	if(value < Threshold)
	{
		discard;
	}

	//�w�肳�ꂽ�摜��`��
	outColor = texture(texColor, inTexcoord);

	//���E�����̂������������Ȃ������߁A�����x���Ȃ߂炩�ɕω�������
	const float SmoothRange = 0.02;
	outColor.a *= smoothstep(Threshold - SmoothRange, Threshold ,value);
}