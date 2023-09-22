//@file Debug.cpp

#include "Debug.h"
#include <Windows.h>
#include <stdio.h>
#include <Stdarg.h>

namespace Debug
{
	//書式付き文字列をデバッグウィンドウに表示する

	void Log(Type type, const char* func, const char* format, ...)
	{
		char buffer[1024];
		char* p = buffer;
		size_t size = sizeof(buffer) - 1;	//末尾に改行を追加するため

		//ログの種類と関数名を設定
		static const char* const typeNames[] = { "エラー","警告","情報" };
		size_t n = snprintf(p, size, "[%s]%s:",
			typeNames[static_cast<int>(type)], func);
		p += n;
		size -= n;

		//メッセージを設定
		va_list ap;

		va_start(ap, format);	//va_listの初期化
		//           （出力先のアドレス、出力可能なバイト数、書式付き文字列、va_list型変数）
		p += vsnprintf(p, size, format, ap);
		va_end(ap);				//va_listの破棄

		//末尾に改行を追加
		p[0] = '\n';
		p[1] = '\0';

		//作成した文字列をデバッグウィンドウに表示
		OutputDebugString(buffer);

	}
}