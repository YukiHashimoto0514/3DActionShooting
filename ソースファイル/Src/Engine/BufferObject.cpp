//@file BufferObject.cpp

#include "BufferObject.h"
#include "Debug.h"

//バッファオブジェクトを作成する
//@param type	バッファオブジェクトの種類
//@param size	バッファオブジェクトのバイトサイズ
//@param data	バッファにコピーするデータアドレス
//				不要な場合はnullptrを指定する
//@param flags  使途を示すビットフラグの論理和(glBufferStorageを参照)

//@return 作成したバッファオブジェクトへのポインタ

BufferObjectPtr BufferObject::Create(
	GLsizeiptr size, const void* data, GLbitfield flags)
{
	return std::make_shared<BufferObject>(size, data, flags);
}

//コンストラクタ
//@param type	バッファオブジェクトの種類
//@param size	バッファオブジェクトのバイトサイズ
//@param data	バッファにコピーするデータアドレス
//				不要な場合はnullptrを指定する
//@param flags  使途を示すビットフラグの論理和(glBufferStorageを参照)

BufferObject::BufferObject(
	GLsizeiptr size, const void* data, GLbitfield flags)
	:bufferSize(size)
{
	glCreateBuffers(1, &id);	//バッファオブジェクトを作成
	glNamedBufferStorage(id, bufferSize, data, flags);	//GPUメモリを確保
}

//デストラクタ
BufferObject::~BufferObject()
{
	glDeleteBuffers(1, &id);
}


//バッファオブジェクトを作成する
//@param size	バッファオブジェクトのバイトサイズ
//@param type	バッファオブジェクトの種類
//@param access	以下のいずれかを指定する
//				GL_READ_WRITE 読み取りと書き込みの両対応でマップする
//				GL_READ_ONLY　読み取り専用としてマップする
//              GL_WRITE_ONLY 書き込み専用としてマップする

//@return 作成したバッファオブジェクトへのポインタ

MappedBufferObjectPtr MappedBufferObject::Create(
	GLsizeiptr size, GLenum type, GLenum access)
{
	return std::make_shared<MappedBufferObject>(size, type, access);
}

//コンストラクタ
//@param size	バッファオブジェクトのバイトサイズ
//@param type	バッファオブジェクトの種類
//@param access	以下のいずれかを指定する
//				GL_READ_WRITE 読み取りと書き込みの両対応でマップする
//				GL_READ_ONLY　読み取り専用としてマップする
//              GL_WRITE_ONLY 書き込み専用としてマップする

MappedBufferObject::MappedBufferObject(
	GLsizeiptr size, GLenum type, GLenum access)
	:type(type)
{
	//ダブルバッファのため、サイズを256バイト境界に繰り上げる
	bufferSize = ((size + 255) / 256) * 256;

	//アクセスの種類によってフラグを選択
	GLbitfield flags = GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

	switch (access)
	{
	default:
		LOG_ERROR("引数が間違っています(access=%d", access);
		[[fallthrough]];

	case GL_READ_WRITE: flags |= GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;
		break;

	case GL_READ_ONLY:  flags |= GL_MAP_READ_BIT;
		break;

	case GL_WRITE_ONLY: flags |= GL_MAP_WRITE_BIT;
		break;
	}

	//バッファオブジェクトを作成
	glCreateBuffers(1, &id);
	glNamedBufferStorage(id, bufferSize * 2,//ダブルバッファように2倍確保
		nullptr, flags);

	//GPUメモリをCPUメモリアドレスにマップする
	p = static_cast<uint8_t*>(glMapNamedBuffer(id, access));
}
	//デストラクタ
MappedBufferObject::~MappedBufferObject()
{
	glDeleteBuffers(1, &id);
}

//GPUがbufferを使い終るのを待つ
void MappedBufferObject::WaitSync()
{
	for (GLsync& sync = syncList[bufferIndex]; sync;)
	{
		const GLuint64 timeout = 16'700'000;//16.7ミリ(約1/60秒)
		const GLenum status = glClientWaitSync(sync, 0, timeout);//処理完了待ち

		if (status != GL_TIMEOUT_EXPIRED)
		{
			//使い終った同期オブジェクトは削除しなくてはならない
			glDeleteSync(sync);
			sync = 0;
			break;
		}
	}
}

//データのコピー先アドレスを取得する
//@return コピー先アドレス

//コピー可能な範囲はGetSize()で取得できる

uint8_t* MappedBufferObject::GetMappedAddress() const
{
	return p + bufferIndex * bufferSize;//CPUが読み書き可能なbufferのアドレス
}

//指定した範囲をOpenGLコンテキストに割り当てる

//@param index  割り当てるバインディングポイント番号
//@param offset 割り当て範囲の先頭位置 
//@param size   割り当て範囲のバイトサイズ

void MappedBufferObject::Bind(
	GLuint index, size_t offset, size_t size)
{
	offset += bufferIndex * bufferSize;
	glBindBufferRange(type, index, id, offset, size);
}

//コピー先のbufferを切り替える

void MappedBufferObject::SwapBuffers()
{
	syncList[bufferIndex] =
		glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

	bufferIndex = (bufferIndex + 1) % 2;
}
