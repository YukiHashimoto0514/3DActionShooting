//@file particle.frag

#version 450

//�V�F�[�_�\�ւ̓���
layout(location=0) in vec4 inColor;
layout(location=1) in vec2 inTexcoord;

//�e�N�X�`��
layout(binding=0) uniform sampler2D texColor;

//�o�͂���F�f�[�^
out vec4 outColor;

void main()
{
	outColor = inColor * texture(texColor,inTexcoord);
}
