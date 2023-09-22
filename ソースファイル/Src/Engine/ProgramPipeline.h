//@file ProgramPipeline.h

#ifndef PROGRAMPIPELINE_H_INCLUDED
#define PROGRAMPIPELINE_H_INCLUDED

#include "glad/glad.h"
#include <memory>
#include <string>

//先行宣言
class ProgramPipeline;
using ProgramPipelinePtr = std::shared_ptr<ProgramPipeline>;

//プログラムパイプラインを管理するクラス

class ProgramPipeline
{
public:
	//プログラムパイプラインを作成
	static ProgramPipelinePtr Create(
		const std::string& filenameVS, const std::string& fikenameFS);

	//コンストラクタ
	ProgramPipeline(const std::string& filenameVS, const std::string& fikenameFS);

	//デストラクタ
	~ProgramPipeline();

	//コピーと代入を禁止
	ProgramPipeline(const ProgramPipeline&) = delete;
	ProgramPipeline& operator=(const ProgramPipeline&) = delete;

	//管理番号を取得
	operator GLuint()const { return pp; }


private:
	GLuint vs = 0;	//頂点シェーダー
	GLuint fs = 0;	//フラグメントシェーダー
	GLuint pp = 0;	//プログラムパイプライン
	std::string filenameVS;	//頂点シェーダーファイル名
	std::string filenameFS;	//フラグメントシェーダーファイル名
};



#endif //PROGRAMPIPELINE_H_INCLUDED