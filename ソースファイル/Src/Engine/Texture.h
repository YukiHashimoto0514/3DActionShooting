//@file Texture.h

#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

#include "glad/glad.h"
#include <string>
#include <memory>
#include <vector>

//��s�錾
class Texture;
using TexturePtr = std::shared_ptr<Texture>;

//�摜�f�[�^���
struct ImageData
{
	std::vector<char> Data;	//�o�C�g�f�[�^
	GLenum ImageFormat = 0;	//�摜�̃s�N�Z���`��
	GLenum ImageType = 0;	//�摜�̌^
	GLenum GPUFormat = 0;	//GPU�̃s�N�Z���`��
	int PixelBytes = 0;		//1�s�N�Z���̃o�C�g��
	GLint Width = 0;		//�摜�̕�(�s�N�Z����)
	GLint Height = 0;		//�摜�̍���(�s�N�Z����)
};

//�摜�̊i�[������\���񋓌^
enum class ImageDirection
{
	BottomToTop,	//�������
	TopToBottom,	//�ォ�牺
};

//�F��Ԃ�\���񋓌^
enum class ColorSpace
{
	Gamma,	//�K���}�F���
	Linear,	//���j�A�F���
};

//�t�@�C������摜�f�[�^��ǂݍ���
ImageData LoadImageData(const std::string& filename,
	ImageDirection direction, ColorSpace colorspace);

//�e�N�X�`��
class Texture
{
public:
	//�e�N�X�`���̗��p�ړI
	enum class Usage
	{
		for2D,			//2D�`��p(�K���}�F���)
		for3D,			//3D�`��p(�K���}�F���)
		for3DLinear,	//3D�`��p(���j�A�F���)
		forGltf,		//gltf�`��p(�K���}�F���)
		forGltfLinear,	//gltf�`��p(���j�A�F���)
	};

	//�e�N�X�`�����쐬
	static TexturePtr Create(
		const std::string& filename, GLenum filterMode,
		Usage usage);

	static TexturePtr Create(
		const char* name, GLint width, GLint height,
		GLenum internalFormat, GLenum filterMode);

	static TexturePtr CreateCubeMap(const std::string PathList[6]);

	//�R���X�g���N�^
	explicit Texture(const std::string& filename, GLenum filterMode = GL_NEAREST,
		Usage usage = Usage::for2D);

	Texture(const char* name, GLint width, GLint height,
		GLenum internalFormat, GLenum filterMode);

	//�R�s�[�A�^�ϊ��̋֎~
	explicit Texture(const std::string PathList[6]);

	//�f�X�g���N�^
	~Texture();

	//�R�s�[�Ƒ�����֎~
	Texture(const Texture&) = delete;
	Texture& operator = (const Texture&) = delete;

	//�Ǘ��ԍ����擾
	operator GLuint() const { return tex; };

	//���ƍ������擾
	GLint GetWidth() const { return width; }
	GLint GetHeight() const { return height; }

	//���b�v���[�h��ݒ�
	void SetWrapMode(GLenum wrapMode);

private:
	GLuint tex = 0;
	std::string filename;

	GLint width = 0;	//��
	GLint height = 0;	//����
};


#endif //TEXTURE_H_INCLUDED