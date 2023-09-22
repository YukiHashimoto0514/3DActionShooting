//@file FramebufferObject.cpp

#include "FramebufferObject.h"
#include "Debug.h"

//FBOを作成する

//@param width	フレームバッファの幅  （ピクセル数）
//@param height	フレームバッファの高さ（ピクセル数）
//@param type	FBOの種類
	
//@return 作成したFBO	
FramebufferObjectPtr FramebufferObject::Create(int width, int height,
	FboType type = FboType::colorDepth)
{
	return std::make_shared<FramebufferObject>(width, height, type);
}

//コンストラクタ

//@param width	フレームバッファの幅  （ピクセル数）
//@param height	フレームバッファの高さ（ピクセル数）
//@param type	FBOの種類
FramebufferObject::FramebufferObject(int width, int height, FboType type)
{
	//カラーテクスチャを作成
	if (type & FboType::color)
	{
		texColor = Texture::Create("FBO(Color)", width, height, GL_RGBA16F, GL_LINEAR);

		//texColorがnullではなく、管理番号が0なら
		if (!texColor || !*texColor)
		{
			LOG_ERROR("FBO用カラーテクスチャの作成に失敗");
			texColor.reset();//カラーテクスチャを破棄

			return;
		}
		texColor->SetWrapMode(GL_CLAMP_TO_EDGE);
	}

	//深度テクスチャを作成
	if (type & FboType::depth)
	{
		texDepth = Texture::Create("FBO(Depth)", width, height,
			GL_DEPTH_COMPONENT32F, GL_LINEAR);

		if (!texDepth || !*texDepth)
		{
			LOG_ERROR("FBO用深度テクスチャの作成に失敗");
			texColor.reset();	//カラーテクスチャを破棄
			texDepth.reset();	//深度テクスチャを破棄

			return;
		}
		texDepth->SetWrapMode(GL_CLAMP_TO_EDGE);

	}
	//FBOを作成
	glCreateFramebuffers(1, &fbo);//作成数、格納するアドレス

	if (static_cast<int>(type) & 1)
	{
		                  //割り当て先、用途、テクスチャID、ミップマップレベル
		glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, *texColor, 0);
	}
	else
	{
		                      //割当先FBO、描画先カラーバッファ
		glNamedFramebufferDrawBuffer(fbo, GL_NONE);
	}

	if (static_cast<int>(type) & 2)
	{
		glNamedFramebufferTexture(fbo, GL_DEPTH_ATTACHMENT, *texDepth, 0);
	}
	
	//FBOの有効性をチェック
	                 //チェックするFBOのID、IDが0だったら代わりにチェックするバインディングポイント
	if (glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("FBOの作成に失敗");
		glDeleteFramebuffers(1, &fbo);
		fbo = 0;
		texColor.reset();
		texDepth.reset();

		return;
	}

	//作成成功
	LOG("FBO(width=%d,height=%d)を作成", width, height);
}

//デストラクタ
FramebufferObject::~FramebufferObject()
{
	       //削除するFBOの数、削除するFBOのID配列
	glDeleteFramebuffers(1, &fbo);
}