//@file Texture.cpp

#include "Texture.h"
#include "Debug.h"
#include <filesystem>
#include <fstream>
#include <vector>
#include <algorithm>

//�e�N�X�`�����쐬����
//@param filename //�摜�t�@�C����
//@param fileterMode �e�N�X�`���ɐݒ肷��g��k���t�B���^
//@param usage ���p�ړI
// 
//@return �쐬�����e�N�X�`���ւ̃|�C���^
TexturePtr Texture::Create(const std::string& filename, GLenum filterMode,
	Usage usage)
{
	return std::make_shared<Texture>(filename, filterMode, usage);
}

//�e�N�X�`�����쐬����
//@param name           �e�N�X�`�����i�f�o�b�O�p�j
//@param widht          �e�N�X�`���̕�
//@param height         �e�N�X�`���̍���
//@param internalFormat �e�N�X�`���̃f�[�^�`��
//@param filterMode     �e�N�X�`���ɐݒ肷��g��k���t�B���^
// 
//@return �쐬�����e�N�X�`���ւ̃|�C���^
TexturePtr Texture::Create(
	const char* name, GLint width, GLint height,
	GLenum internalFormat, GLenum filterMode)
{
	return std::make_shared<Texture>(
		name, width, height, internalFormat, filterMode);
}

//�L���[�u�}�b�v�e�N�X�`�����쐬����
//@param ParhList �摜�t�@�C�����̔z��

//@return �쐬�����e�N�X�`���ւ̃|�C���^
TexturePtr Texture::CreateCubeMap(const std::string PathList[6])
{
	return std::make_shared<Texture>(PathList);
}

//�t�@�C������摜�f�[�^��ǂݍ���
//@param filename        �摜�t�@�C����
//@param direction       �摜�f�[�^�̊i�[����
//@param colorspace      �g�p����F���
// 
//@return �ǂݍ��񂾃f�[�^��ێ�����ImageData�\����

ImageData LoadImageData(const std::string& filename,
	ImageDirection direction, ColorSpace colorspace)
{
	//�t�@�C�����J��
	std::ifstream file(filename, std::ios::binary);

	if (!file)
	{
		LOG_ERROR("%s���J���܂���", filename.c_str());
		return {};
	}

	//�w�b�_����ǂݍ���
	const size_t tgaHeaderSize = 18;//�w�b�_���̃o�C�g��
	uint8_t header[tgaHeaderSize];
	file.read(reinterpret_cast<char*>(header), tgaHeaderSize);

	//�摜�̎�ނ����o��
	//0:�摜����
	//1:�C���f�b�N�X(�����k)
	//2:�g�D���[�J���[(�����k)
	//3:����(�����k)
	//9:�C���f�b�N�X(RLE)
	//10:�g�D���[�J���[(RLE)
	//11:����(RLE)
	const int ImageType = header[2];//2�o�C�g�ڂ𒲂ׂ�

	//�w�b�_��񂩂�摜�̃T�C�Y�����o��
	const GLsizei width = header[12] + header[13] * 0x100;
	const GLsizei height = header[14] + header[15] * 0x100;

	//1�s�N�Z���̃r�b�g�������o��
	const int PixelDepth = header[16];
	const int PixelBytes = PixelDepth / 8;	//1�s�N�Z���̃o�C�g��

	//�摜����ǂݍ���
	std::vector<char> img(
		std::filesystem::file_size(filename) - tgaHeaderSize);
	file.read(img.data(), img.size());

	//���k����Ă���ꍇ�͓W�J����
	if (ImageType & 0x08)
	{
		//�f�[�^�W�J��ƂȂ�o�b�t�@���쐬
		std::vector<char> tmp(width * height * static_cast<GLsizei>(PixelBytes));
		char* dest = tmp.data();	//�W�J��̃A�h���X
		const char* const end = dest + tmp.size();	//�I�[�A�h���X

		const char* Packet = img.data();//�p�P�b�g�̃A�h���X

		//�o�b�t�@�����܂�܂Ń��[�v
		while (dest < end)
		{
			//�p�P�b�g�̃w�b�_������id�ƃf�[�^�������o��
			const int id = *Packet & 0x80;
			const int count = (*Packet & 0x7f) + 1;

			++Packet;//�f�[�^���ֈړ�

			//ID�ɂ���ĕ���
			if (id)
			{
				//���k�p�P�b�g

				//�ЂƂ̃s�N�Z���f�[�^��count��������
				for (int i = 0; i < count; ++i)
				{
					for (int j = 0; j < PixelBytes; ++j)
					{
						*dest = Packet[i];

						++dest;
					}
				}

				Packet += PixelBytes;
			}
			else
			{
				//�����k�p�P�b�g

				//count�̃s�N�Z���f�[�^�����̂܂܃R�s�[����
				const int RawDataByte = count * PixelBytes;

				dest = std::copy(Packet, Packet + RawDataByte, dest);
				Packet += RawDataByte;
			}
		}

		//�W�J��o�b�t�@�Ɖ摜�f�[�^�o�b�t�@�����ւ���
		img.swap(tmp);
	}

	//TGA�w�b�_18�o�C�g�ڂ̑�T�r�b�g�́A�摜�f�[�^�̊i�[������\��
	//0�̏ꍇ�F�������
	//1�̏ꍇ�F�ォ�牺
	//OpenGL�ł͉摜�f�[�^���u�������v�Ɋi�[���郋�[���ɂȂ��Ă���̂ŁA
	//TGA���u�ォ�牺�v�Ɋi�[����Ă���ꍇ�͉摜�𔽓]����

	bool topToBottom = header[17] & 0x20;

	if (direction==ImageDirection::TopToBottom)
	{
		topToBottom = !topToBottom;//2D�`��p�̏ꍇ�͔��]�������t�ɂ���
	}

	if (topToBottom)
	{
		//�摜�̃o�C�g���Ɖ�1�񕪂̃o�C�g�����v�Z
		const int pixelDepth = header[16];
		const int imageSize = width * height * pixelDepth / 8;
		const size_t lineSize = width * pixelDepth / 8;

		//tmp�ɏ㉺���]�����摜���R�s�[
		std::vector<char> tmp(imageSize);
		auto source = img.begin();
		auto destination = tmp.end();

		for (size_t i = 0; i < height; i++)
		{
			destination -= lineSize;
			std::copy(source, source + lineSize, destination);
			source += lineSize;
		}

		//tmp��img�̃f�[�^������
		img.swap(tmp);
	}

	//[1�s�N�Z���̃r�b�g��]����s�N�Z���`���ƃf�[�^�^�C�v��I��
	struct FormatData
	{
		GLenum ImageFormat;	//�摜�f�[�^�̃s�N�Z���`��
		GLenum ImageType;	//�摜�f�[�^�̌^
		GLenum GPUFormat;	//GPU���̃s�N�Z���`��
		GLenum GPUFormatLinear;//GPU���̃s�N�Z���`��(���j�A�F���)
	};

	constexpr FormatData formatList[] =
	{
		{GL_RED,GL_UNSIGNED_BYTE,				GL_SRGB8,		GL_R8},
		{GL_BGRA,GL_UNSIGNED_SHORT_1_5_5_5_REV,	GL_SRGB8_ALPHA8,GL_RGB5_A1},
		{GL_BGR,GL_UNSIGNED_BYTE,				GL_SRGB8,		GL_RGB8},
		{GL_BGRA,GL_UNSIGNED_BYTE,				GL_SRGB8_ALPHA8,GL_RGBA8},
	};

	FormatData format = formatList[PixelBytes - 1];

	//�s�N�Z���`����I��
	GLenum GPUFormat = GL_SRGB8_ALPHA8;
	if (colorspace==ColorSpace::Linear)
	{
		GPUFormat = format.GPUFormatLinear;
	}

	return ImageData{ img,
	format.ImageFormat,format.ImageType,
	format.GPUFormat,PixelBytes,width,height };
}


//�R���X�g���N�^
Texture::Texture(const std::string& filename, GLenum filterMode,
	Usage usage):
	filename(filename)
{

	//�摜�̌�����ݒ�
	ImageDirection direction = ImageDirection::BottomToTop;
	if (usage == Usage::for2D || usage == Usage::forGltf || usage == Usage::forGltfLinear)
	{
		direction = ImageDirection::TopToBottom;
	}

	//�F��Ԃ�I��
	ColorSpace colorspace = ColorSpace::Gamma;
	if (usage == Usage::for3DLinear || usage == Usage::forGltfLinear)
	{
		colorspace = ColorSpace::Linear;
	}

	//�摜�t�@�C����ǂݍ���
	const ImageData imageData = LoadImageData(filename, direction, colorspace);

	//�摜�̓ǂݎ�莸�s
	if (imageData.Data.empty())
	{
		return;
	}

	//�e�N�X�`�����쐬
	glCreateTextures(GL_TEXTURE_2D, 1, &tex);
	glTextureStorage2D(tex, 1, imageData.GPUFormat, imageData.Width, imageData.Height);

	//���݂̃A���C�������g���L�^���Ă���A���C�������g��ύX
	GLint Alignment;
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &Alignment);

	if (Alignment != imageData.PixelBytes)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}

	//GPU�������Ƀf�[�^���R�s�[
	glTextureSubImage2D(tex, 0, 0, 0, imageData.Width, imageData.Height,
		imageData.ImageFormat, imageData.ImageType, imageData.Data.data());

	//�A���C�������g�����ɖ߂�
	if (Alignment != imageData.PixelBytes)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, Alignment);
	}

	//�����摜�̏ꍇ�A(R,R,R,1)�Ƃ��ēǂݎ����悤�ɐݒ�
	if (imageData.ImageFormat == GL_RED)
	{
		glTextureParameteri(tex, GL_TEXTURE_SWIZZLE_G, GL_RED);
		glTextureParameteri(tex, GL_TEXTURE_SWIZZLE_B, GL_RED);
	}

	//�g��k���t�B���^��ݒ�
	glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, filterMode);
	glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, filterMode);

	//�T�C�Y��ݒ�
	this->width = imageData.Width;
	this->height = imageData.Height;
}

//�R���X�g���N�^
//@param name           �e�N�X�`�����i�f�o�b�O�p�j
//@param widht          �e�N�X�`���̕�
//@param height         �e�N�X�`���̍���
//@param internalFormat �e�N�X�`���̃f�[�^�`��
//@param filterMode     �e�N�X�`���ɐݒ肷��g��k���t�B���^
Texture::Texture(const char* name, GLint width, GLint height,
	GLenum internalFormat, GLenum filterMode)
{
	//�e�N�X�`�����쐬
	glCreateTextures(GL_TEXTURE_2D, 1, &tex);
	glTextureStorage2D(tex, 1, internalFormat, width, height);

	//�g��k���t�B���^��ݒ�
	glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, filterMode);
	glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, filterMode);

	//�T�C�Y��ݒ�
	this->width = width;
	this->height = height;
}

//�L���[�u�}�b�v�e�N�X�`�����쐬����R���X�g���N�^
//@param PathList �摜�t�@�C�����̔z��
Texture::Texture(const std::string ParhList[6])
{
	//�摜�t�@�C����ǂݍ���
	ImageData imageData[6];
	for (int i = 0; i < 6; ++i)
	{
		//�L���[�u�}�b�v�͌��_������Ȃ̂ŁA�摜�̕����ɂ͏ォ��ǂݍ���
		imageData[i] = LoadImageData(
			ParhList[i], ImageDirection::TopToBottom, ColorSpace::Gamma);

		//�摜�f�[�^����̏ꍇ�͓ǂݍ��ݎ��s�Ƃ���
		if (imageData[i].Data.empty())
		{
			return;
		}
	}

	//���ƍ�����ݒ�
	width = imageData[0].Width;
	height = imageData[0].Height;

	//�쐬�\�ȃ~�b�v�}�b�v�����v�Z
	const int MipCount = static_cast<int>(std::log2(std::max(width, height)) + 1);
	
	//�L���[�u�}�b�v�e�N�X�`�����쐬
	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &tex);
	glTextureStorage2D(tex, MipCount, imageData[0].GPUFormat, width, height);

	glGetError();	//�G���[��Ԃ����Z�b�g

	//�摜�f�[�^��GPU�������ɃR�s�[
	for (int i = 0; i < 6; ++i)
	{
		glTextureSubImage3D(tex, 0,	//�Ǘ��ԍ��A�~�b�v�}�b�v���x��
			0, 0, i,				//�R�s�[�悘�A�R�s�[�悙�R�s�[�悚
			width, height, 1,		//���A�����A�摜����
			imageData[i].ImageFormat, imageData[i].ImageType, imageData[i].Data.data());//�摜�̏��ȂǂȂ�

		//�����I�ɃG���[�`�F�b�N
		const GLenum result = glGetError();

		if (result != GL_NO_ERROR)
		{
			LOG_ERROR("%s�̍쐬�Ɏ��s", ParhList[i].c_str());
		}
	}

	//�~�b�v�}�b�v�̎�������
	glGenerateTextureMipmap(tex);

	//�ő�~�b�v�}�b�v���x����ݒ�
	glTextureParameteri(tex, GL_TEXTURE_MAX_LEVEL, MipCount - 1);

	//�g��k���t�B���^��ݒ�
	if (MipCount > 1)
	{
		glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	else
	{
		glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//���b�v���[�h��ݒ�
	glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	LOG("�L���[�u�}�b�v:%s�̍쐬", ParhList[0].c_str());
}

//���b�v���[�h��ݒ�
//@param wrapMode �ݒ肷�郉�b�v���[�h
void Texture::SetWrapMode(GLenum wrapMode)
{
	glTextureParameteri(tex, GL_TEXTURE_WRAP_S, wrapMode);	//X��
	glTextureParameteri(tex, GL_TEXTURE_WRAP_T, wrapMode);	//Y��
}

//�f�X�g���N�^
Texture::~Texture()
{
	glDeleteTextures(1, &tex);
}
