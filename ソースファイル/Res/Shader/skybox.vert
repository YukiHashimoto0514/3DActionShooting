//@file skybox.vert

#version 450

//�V�F�[�_�\�ւ̓���
layout(location=0) in vec3 inPosition;

//�V�F�[�_�\����̏o��
layout(location=0) out vec3 outTexcoord;

//�v���O��������̓���

//x:����p�ɂ�鐅���g�嗦
//y:����p�ɂ�鐂���g�嗦
//z:���ߖ@�p�����[�^A
//w:���ߖ@�p�����[�^B
layout(location=3) uniform vec4 CameraData;

//x:�J����x����]��sin
//y:�J����x����]��cos
//z:�J����y����]��sin
//w:�J����y����]��cos
layout(location=5) uniform vec4 CameraSinCosXY;

//�G���g���[�|�C���g
void main()
{
	//���_���W���L���[�u�}�b�v�̃e�N�X�`�����W�Ƃ���
	outTexcoord = inPosition;

	//�J�����̉�]�����f���ɔ��f����ɂ́A�J�����̉�]�Ƌt�����ɉ�]������
	//�W���̉�]�����͔����v���Ȃ̂ŁA���v���ɂȂ�悤�ɂ���
	vec3 pos = inPosition * 2;

	//Y����](x,z����)
	float sY = -CameraSinCosXY.z;	//���v���ɕϊ�
	float cY = CameraSinCosXY.w;

	pos=vec3(pos.x * cY + pos.z * sY,
			 pos.y,
			 pos.x * -sY + pos.z * cY);

	//X����](z,y����)
	float sX = -CameraSinCosXY.x;	//���v���ɕϊ�
	float cX = CameraSinCosXY.y;

	pos=vec3(pos.x,
			 pos.z * -sX + pos.y * cX,
			 pos.z * cX + pos.y * sX);

	//����p�𔽉f
	gl_Position.x = pos.x * CameraData.x;
	gl_Position.y = pos.y * CameraData.y;
	gl_Position.z = -pos.z * CameraData.w + CameraData.z;
	gl_Position.w = -pos.z;
}
