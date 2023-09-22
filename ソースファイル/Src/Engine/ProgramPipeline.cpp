//@file ProgramPipeline.cpp

#include "ProgramPipeline.h"
#include "Debug.h"
#include <fstream>
#include <filesystem>
#include <vector>

//プログラムパイプラインを作成する
//@param filenameVS 頂点シェーダーファイル名
//@param filenameFS フラグメントシェーダー名
//
//@return 作成したプログラムパイプラインへのポインター

ProgramPipelinePtr ProgramPipeline::Create(
	const std::string& filenameVS, const std::string& filenameFS)
{
	return std::make_shared<ProgramPipeline>(filenameVS, filenameFS);
}

//コンストラクタ
//@param filenameVS 頂点シェーダーファイル名
//@param filenameFS フラグメントシェーダー名

ProgramPipeline::ProgramPipeline(
	const std::string& filenameVS, const std::string& filenameFS) :
	filenameVS(filenameVS), filenameFS(filenameFS)
{
	//頂点シェーダーを読み込んでコンパイルする
	std::ifstream file;
	vs = glCreateShader(GL_VERTEX_SHADER);
	file.open(filenameVS, std::ios::binary);//ファイルを開く
	if (file)
	{
		//メインメモリ領域を確保
		std::vector<char> s(std::filesystem::file_size(filenameVS));

		//ファイルを読み込む
		file.read(s.data(), s.size());
		s.push_back('\0');//null終端を追加

		//ソースを設定してコンパイル
		const char* a[] = { s.data() };
		glShaderSource(vs, 1, a, nullptr);//(管理番号、文字列の長さ文字列の配列、文字列長の配列)
		glCompileShader(vs);//直前のやつをコンパイル(管理番号)
	}
	else
	{
		LOG_ERROR("頂点シェーダ―%sを開けません", filenameVS.c_str());
	}
	file.close();

	//フラグメントシェーダを読み込んでコンパイルする
	fs = glCreateShader(GL_FRAGMENT_SHADER);
	file.open(filenameFS, std::ios::binary);
	if (file)
	{
		std::vector<char> s(std::filesystem::file_size(filenameFS));
		file.read(s.data(), s.size());
		s.push_back('\0');
		const char* ss[] = { s.data() };
		glShaderSource(fs, 1, ss, nullptr);
		glCompileShader(fs);
	}
	else
	{
		LOG_ERROR("フラグメントシェーダ%sを開けません", filenameFS.c_str());
	}
	file.close();
	
	//２つのシェーダをリンクする

	if (vs && fs)
	{
		pp = glCreateProgram();
		glAttachShader(pp, vs);	//(プログラムの管理番号とシェーダーの管理番号)
		glAttachShader(pp, fs);
		glLinkProgram(pp);

		//リンクの成否を調べる
		GLint result;

		               //パイプラインの管理番号、調べる状態名、格納するアドレス
		glGetProgramiv(pp, GL_LINK_STATUS, &result);	

		if (result != GL_TRUE)
		{
			LOG_ERROR("シェーダーのリンクに失敗(vs=%s,fs=%s)",
				filenameVS.c_str(), filenameFS.c_str());
		}
	}
}

//デストラクタ

ProgramPipeline::~ProgramPipeline()
{
	glDeleteProgram(pp);
	glDeleteShader(vs);
	glDeleteShader(fs);
}