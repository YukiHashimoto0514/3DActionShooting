//@file VertexArray.h

#ifndef VERTEXARRAY_H_INCLUDED
#define VERTEXARRAY_H_INCLUDED

#include "glad/glad.h"
#include <memory>

//先行宣言
class VertexArray;
using VertexArrayPtr = std::shared_ptr<VertexArray>;

//VAOを管理するクラス

class VertexArray
{
public:
	//VAOを作成
	static VertexArrayPtr Create() { return std::make_shared<VertexArray>(); }

	//コンストラクタ
	VertexArray() { glCreateVertexArrays(1, &vao); }

	//デストラクタ
	~VertexArray() { glDeleteVertexArrays(1, &vao); }

	//コピーと代入を禁止
	VertexArray(const VertexArray&) = delete;
	VertexArray& operator=(const VertexArray&) = delete;

	//管理番号を取得
	operator GLuint()const { return vao; }

private:
	GLuint vao = 0;
};



#endif //VERTEXARRAY_H_INCLUDED