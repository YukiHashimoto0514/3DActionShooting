//@file skybox.frag

#version 450

//�V�F�[�_�\�ւ̓���
layout(location=0) in vec3 inTexcoord;//�L���[�u�̒��S����O�Ɍ������x�N�g��

//�e�N�X�`��
layout(binding=0) uniform samplerCube texSkyBox;

//�o�͂���F�f�[�^
out vec4 outColor;

//�G���g���[�|�C���g
void main()
{
	outColor = texture(texSkyBox, inTexcoord);
}