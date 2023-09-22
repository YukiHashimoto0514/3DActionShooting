//@file BufferObject.h

#ifndef BUFFEROBJECT_H_INCLUDED
#define BUFFEROBJECT_H_INCLUDED

#include "glad/glad.h"
#include <memory>

//先行宣言
class BufferObject;
using BufferObjectPtr = std::shared_ptr<BufferObject>;

class MappedBufferObject;	//SSBOを管理する
using MappedBufferObjectPtr = std::shared_ptr<MappedBufferObject>;

class BufferObject
{
public:
	static BufferObjectPtr Create(GLsizeiptr size,
		const void* data = nullptr, GLbitfield flags = 0);

	//コンストラクタ
	explicit BufferObject(GLsizeiptr size,
		const void* data = nullptr, GLbitfield flags = 0);

	//デストラクタ
	~BufferObject();

	operator GLuint() const { return id; }
	GLsizeiptr GetSize() const { return bufferSize; }

private:
	GLuint id = 0;
	GLsizeiptr bufferSize = 0;
};

//バッファオブジェクト（マップあり）
class MappedBufferObject
{
public:
	static MappedBufferObjectPtr Create(
		GLsizeiptr size, GLenum type, GLenum access);

	//コンストラクタ
	MappedBufferObject(
		GLsizeiptr size, GLenum type, GLenum access);

	//デストラクタ
	~MappedBufferObject();

	operator GLuint() const { return id; }
	GLsizeiptr GetSize() const { return bufferSize; }

	void WaitSync();										//CPUメモリへのコピーの完了を待つ
	uint8_t* GetMappedAddress() const;						//取得したアドレスにデータをコピーする
	void Bind(GLuint index, size_t offset, size_t size);	//GPUメモリを割り当てて描画に利用する
	void SwapBuffers();										//コピー用と描画用のバッファを切り替える

	GLenum GetType() const { return type; }

private:
	GLuint id = 0;
	GLsizeiptr bufferSize = 0;

	uint8_t* p = nullptr;			//データコピー先アドレス
	size_t bufferIndex = 0;			//コピー先バッファのインデックス
	GLsync syncList[2] = { 0,0 };	//同期オブジェクト
	GLenum type = GL_ARRAY_BUFFER;	//バッファの種類
};


#endif //BUFFEROBJECT_H_INCLUDED
