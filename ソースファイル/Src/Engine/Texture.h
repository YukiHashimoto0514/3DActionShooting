//@file Texture.h

#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

#include "glad/glad.h"
#include <string>
#include <memory>
#include <vector>

//先行宣言
class Texture;
using TexturePtr = std::shared_ptr<Texture>;

//画像データ情報
struct ImageData
{
	std::vector<char> Data;	//バイトデータ
	GLenum ImageFormat = 0;	//画像のピクセル形式
	GLenum ImageType = 0;	//画像の型
	GLenum GPUFormat = 0;	//GPUのピクセル形式
	int PixelBytes = 0;		//1ピクセルのバイト数
	GLint Width = 0;		//画像の幅(ピクセル数)
	GLint Height = 0;		//画像の高さ(ピクセル数)
};

//画像の格納方向を表す列挙型
enum class ImageDirection
{
	BottomToTop,	//下から上
	TopToBottom,	//上から下
};

//色空間を表す列挙型
enum class ColorSpace
{
	Gamma,	//ガンマ色空間
	Linear,	//リニア色空間
};

//ファイルから画像データを読み込む
ImageData LoadImageData(const std::string& filename,
	ImageDirection direction, ColorSpace colorspace);

//テクスチャ
class Texture
{
public:
	//テクスチャの利用目的
	enum class Usage
	{
		for2D,			//2D描画用(ガンマ色空間)
		for3D,			//3D描画用(ガンマ色空間)
		for3DLinear,	//3D描画用(リニア色空間)
		forGltf,		//gltf描画用(ガンマ色空間)
		forGltfLinear,	//gltf描画用(リニア色空間)
	};

	//テクスチャを作成
	static TexturePtr Create(
		const std::string& filename, GLenum filterMode,
		Usage usage);

	static TexturePtr Create(
		const char* name, GLint width, GLint height,
		GLenum internalFormat, GLenum filterMode);

	static TexturePtr CreateCubeMap(const std::string PathList[6]);

	//コンストラクタ
	explicit Texture(const std::string& filename, GLenum filterMode = GL_NEAREST,
		Usage usage = Usage::for2D);

	Texture(const char* name, GLint width, GLint height,
		GLenum internalFormat, GLenum filterMode);

	//コピー、型変換の禁止
	explicit Texture(const std::string PathList[6]);

	//デストラクタ
	~Texture();

	//コピーと代入を禁止
	Texture(const Texture&) = delete;
	Texture& operator = (const Texture&) = delete;

	//管理番号を取得
	operator GLuint() const { return tex; };

	//幅と高さを取得
	GLint GetWidth() const { return width; }
	GLint GetHeight() const { return height; }

	//ラップモードを設定
	void SetWrapMode(GLenum wrapMode);

private:
	GLuint tex = 0;
	std::string filename;

	GLint width = 0;	//幅
	GLint height = 0;	//高さ
};


#endif //TEXTURE_H_INCLUDED