//@file Texture.cpp

#include "Texture.h"
#include "Debug.h"
#include <filesystem>
#include <fstream>
#include <vector>
#include <algorithm>

//テクスチャを作成する
//@param filename //画像ファイル名
//@param fileterMode テクスチャに設定する拡大縮小フィルタ
//@param usage 利用目的
// 
//@return 作成したテクスチャへのポインタ
TexturePtr Texture::Create(const std::string& filename, GLenum filterMode,
	Usage usage)
{
	return std::make_shared<Texture>(filename, filterMode, usage);
}

//テクスチャを作成する
//@param name           テクスチャ名（デバッグ用）
//@param widht          テクスチャの幅
//@param height         テクスチャの高さ
//@param internalFormat テクスチャのデータ形式
//@param filterMode     テクスチャに設定する拡大縮小フィルタ
// 
//@return 作成したテクスチャへのポインタ
TexturePtr Texture::Create(
	const char* name, GLint width, GLint height,
	GLenum internalFormat, GLenum filterMode)
{
	return std::make_shared<Texture>(
		name, width, height, internalFormat, filterMode);
}

//キューブマップテクスチャを作成する
//@param ParhList 画像ファイル名の配列

//@return 作成したテクスチャへのポインタ
TexturePtr Texture::CreateCubeMap(const std::string PathList[6])
{
	return std::make_shared<Texture>(PathList);
}

//ファイルから画像データを読み込む
//@param filename        画像ファイル名
//@param direction       画像データの格納方向
//@param colorspace      使用する色空間
// 
//@return 読み込んだデータを保持するImageData構造体

ImageData LoadImageData(const std::string& filename,
	ImageDirection direction, ColorSpace colorspace)
{
	//ファイルを開く
	std::ifstream file(filename, std::ios::binary);

	if (!file)
	{
		LOG_ERROR("%sを開けません", filename.c_str());
		return {};
	}

	//ヘッダ情報を読み込む
	const size_t tgaHeaderSize = 18;//ヘッダ情報のバイト数
	uint8_t header[tgaHeaderSize];
	file.read(reinterpret_cast<char*>(header), tgaHeaderSize);

	//画像の種類を取り出す
	//0:画像無し
	//1:インデックス(無圧縮)
	//2:トゥルーカラー(無圧縮)
	//3:白黒(無圧縮)
	//9:インデックス(RLE)
	//10:トゥルーカラー(RLE)
	//11:白黒(RLE)
	const int ImageType = header[2];//2バイト目を調べる

	//ヘッダ情報から画像のサイズを取り出す
	const GLsizei width = header[12] + header[13] * 0x100;
	const GLsizei height = header[14] + header[15] * 0x100;

	//1ピクセルのビット数を取り出す
	const int PixelDepth = header[16];
	const int PixelBytes = PixelDepth / 8;	//1ピクセルのバイト数

	//画像情報を読み込む
	std::vector<char> img(
		std::filesystem::file_size(filename) - tgaHeaderSize);
	file.read(img.data(), img.size());

	//圧縮されている場合は展開する
	if (ImageType & 0x08)
	{
		//データ展開先となるバッファを作成
		std::vector<char> tmp(width * height * static_cast<GLsizei>(PixelBytes));
		char* dest = tmp.data();	//展開先のアドレス
		const char* const end = dest + tmp.size();	//終端アドレス

		const char* Packet = img.data();//パケットのアドレス

		//バッファが埋まるまでループ
		while (dest < end)
		{
			//パケットのヘッダ部からidとデータ数を取り出す
			const int id = *Packet & 0x80;
			const int count = (*Packet & 0x7f) + 1;

			++Packet;//データ部へ移動

			//IDによって分岐
			if (id)
			{
				//圧縮パケット

				//ひとつのピクセルデータをcount個複製する
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
				//無圧縮パケット

				//count個のピクセルデータをそのままコピーする
				const int RawDataByte = count * PixelBytes;

				dest = std::copy(Packet, Packet + RawDataByte, dest);
				Packet += RawDataByte;
			}
		}

		//展開先バッファと画像データバッファを入れ替える
		img.swap(tmp);
	}

	//TGAヘッダ18バイト目の第５ビットは、画像データの格納方向を表す
	//0の場合：下から上
	//1の場合：上から下
	//OpenGLでは画像データを「下から上」に格納するルールになっているので、
	//TGAが「上から下」に格納されている場合は画像を反転する

	bool topToBottom = header[17] & 0x20;

	if (direction==ImageDirection::TopToBottom)
	{
		topToBottom = !topToBottom;//2D描画用の場合は反転方向を逆にする
	}

	if (topToBottom)
	{
		//画像のバイト数と横1列分のバイト数を計算
		const int pixelDepth = header[16];
		const int imageSize = width * height * pixelDepth / 8;
		const size_t lineSize = width * pixelDepth / 8;

		//tmpに上下反転した画像をコピー
		std::vector<char> tmp(imageSize);
		auto source = img.begin();
		auto destination = tmp.end();

		for (size_t i = 0; i < height; i++)
		{
			destination -= lineSize;
			std::copy(source, source + lineSize, destination);
			source += lineSize;
		}

		//tmpとimgのデータを交換
		img.swap(tmp);
	}

	//[1ピクセルのビット数]からピクセル形式とデータタイプを選択
	struct FormatData
	{
		GLenum ImageFormat;	//画像データのピクセル形式
		GLenum ImageType;	//画像データの型
		GLenum GPUFormat;	//GPU側のピクセル形式
		GLenum GPUFormatLinear;//GPU側のピクセル形式(リニア色空間)
	};

	constexpr FormatData formatList[] =
	{
		{GL_RED,GL_UNSIGNED_BYTE,				GL_SRGB8,		GL_R8},
		{GL_BGRA,GL_UNSIGNED_SHORT_1_5_5_5_REV,	GL_SRGB8_ALPHA8,GL_RGB5_A1},
		{GL_BGR,GL_UNSIGNED_BYTE,				GL_SRGB8,		GL_RGB8},
		{GL_BGRA,GL_UNSIGNED_BYTE,				GL_SRGB8_ALPHA8,GL_RGBA8},
	};

	FormatData format = formatList[PixelBytes - 1];

	//ピクセル形式を選択
	GLenum GPUFormat = GL_SRGB8_ALPHA8;
	if (colorspace==ColorSpace::Linear)
	{
		GPUFormat = format.GPUFormatLinear;
	}

	return ImageData{ img,
	format.ImageFormat,format.ImageType,
	format.GPUFormat,PixelBytes,width,height };
}


//コンストラクタ
Texture::Texture(const std::string& filename, GLenum filterMode,
	Usage usage):
	filename(filename)
{

	//画像の向きを設定
	ImageDirection direction = ImageDirection::BottomToTop;
	if (usage == Usage::for2D || usage == Usage::forGltf || usage == Usage::forGltfLinear)
	{
		direction = ImageDirection::TopToBottom;
	}

	//色空間を選択
	ColorSpace colorspace = ColorSpace::Gamma;
	if (usage == Usage::for3DLinear || usage == Usage::forGltfLinear)
	{
		colorspace = ColorSpace::Linear;
	}

	//画像ファイルを読み込む
	const ImageData imageData = LoadImageData(filename, direction, colorspace);

	//画像の読み取り失敗
	if (imageData.Data.empty())
	{
		return;
	}

	//テクスチャを作成
	glCreateTextures(GL_TEXTURE_2D, 1, &tex);
	glTextureStorage2D(tex, 1, imageData.GPUFormat, imageData.Width, imageData.Height);

	//現在のアラインメントを記録してからアラインメントを変更
	GLint Alignment;
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &Alignment);

	if (Alignment != imageData.PixelBytes)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}

	//GPUメモリにデータをコピー
	glTextureSubImage2D(tex, 0, 0, 0, imageData.Width, imageData.Height,
		imageData.ImageFormat, imageData.ImageType, imageData.Data.data());

	//アラインメントを元に戻す
	if (Alignment != imageData.PixelBytes)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, Alignment);
	}

	//白黒画像の場合、(R,R,R,1)として読み取られるように設定
	if (imageData.ImageFormat == GL_RED)
	{
		glTextureParameteri(tex, GL_TEXTURE_SWIZZLE_G, GL_RED);
		glTextureParameteri(tex, GL_TEXTURE_SWIZZLE_B, GL_RED);
	}

	//拡大縮小フィルタを設定
	glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, filterMode);
	glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, filterMode);

	//サイズを設定
	this->width = imageData.Width;
	this->height = imageData.Height;
}

//コンストラクタ
//@param name           テクスチャ名（デバッグ用）
//@param widht          テクスチャの幅
//@param height         テクスチャの高さ
//@param internalFormat テクスチャのデータ形式
//@param filterMode     テクスチャに設定する拡大縮小フィルタ
Texture::Texture(const char* name, GLint width, GLint height,
	GLenum internalFormat, GLenum filterMode)
{
	//テクスチャを作成
	glCreateTextures(GL_TEXTURE_2D, 1, &tex);
	glTextureStorage2D(tex, 1, internalFormat, width, height);

	//拡大縮小フィルタを設定
	glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, filterMode);
	glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, filterMode);

	//サイズを設定
	this->width = width;
	this->height = height;
}

//キューブマップテクスチャを作成するコンストラクタ
//@param PathList 画像ファイル名の配列
Texture::Texture(const std::string ParhList[6])
{
	//画像ファイルを読み込む
	ImageData imageData[6];
	for (int i = 0; i < 6; ++i)
	{
		//キューブマップは原点が左上なので、画像の方向には上から読み込む
		imageData[i] = LoadImageData(
			ParhList[i], ImageDirection::TopToBottom, ColorSpace::Gamma);

		//画像データが空の場合は読み込み失敗とする
		if (imageData[i].Data.empty())
		{
			return;
		}
	}

	//幅と高さを設定
	width = imageData[0].Width;
	height = imageData[0].Height;

	//作成可能なミップマップ数を計算
	const int MipCount = static_cast<int>(std::log2(std::max(width, height)) + 1);
	
	//キューブマップテクスチャを作成
	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &tex);
	glTextureStorage2D(tex, MipCount, imageData[0].GPUFormat, width, height);

	glGetError();	//エラー状態をリセット

	//画像データをGPUメモリにコピー
	for (int i = 0; i < 6; ++i)
	{
		glTextureSubImage3D(tex, 0,	//管理番号、ミップマップレベル
			0, 0, i,				//コピー先ｘ、コピー先ｙコピー先ｚ
			width, height, 1,		//幅、高さ、画像枚数
			imageData[i].ImageFormat, imageData[i].ImageType, imageData[i].Data.data());//画像の情報などなど

		//明示的にエラーチェック
		const GLenum result = glGetError();

		if (result != GL_NO_ERROR)
		{
			LOG_ERROR("%sの作成に失敗", ParhList[i].c_str());
		}
	}

	//ミップマップの自動生成
	glGenerateTextureMipmap(tex);

	//最大ミップマップレベルを設定
	glTextureParameteri(tex, GL_TEXTURE_MAX_LEVEL, MipCount - 1);

	//拡大縮小フィルタを設定
	if (MipCount > 1)
	{
		glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	else
	{
		glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//ラップモードを設定
	glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	LOG("キューブマップ:%sの作成", ParhList[0].c_str());
}

//ラップモードを設定
//@param wrapMode 設定するラップモード
void Texture::SetWrapMode(GLenum wrapMode)
{
	glTextureParameteri(tex, GL_TEXTURE_WRAP_S, wrapMode);	//X軸
	glTextureParameteri(tex, GL_TEXTURE_WRAP_T, wrapMode);	//Y軸
}

//デストラクタ
Texture::~Texture()
{
	glDeleteTextures(1, &tex);
}
