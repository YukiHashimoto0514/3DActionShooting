//@file Debug.h

#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

//デバッグ用の機能を格納する名前空間

namespace Debug
{
	//メッセージの種類
	enum class Type
	{
		error,		//エラー
		warning,	//警告
		info		//その他の情報
	};

	//                                                        ...は可変長引数リスト
	void Log(Type type, const char* func, const char* format, ...);
}


#ifndef NDEBUG
#define ENABLE_DEBUG_LOG//このマクロを定義するとデバッグ出力が有効になる
#endif //NDEBUG

//デバッグ出力マクロ
#ifdef ENABLE_DEBUG_LOG
#define LOG_ERROR(...)   Debug::Log(Debug::Type::error,__func__,__VA_ARGS__)
#define LOG_WARNING(...) Debug::Log(Debug::Type::warning,__func__,__VA_ARGS__)
#define LOG(...)         Debug::Log(Debug::Type::info,__func__,__VA_ARGS__)
#else
//デバッグ以外は０を返す
#define LOG_ERROR(...)   ((void)0)
#define LOG_WARNING(...) ((void)0)
#define LOG(...)         ((void)0)
#endif //ENABLE_DEBUG_LOG



#endif //DEBUG_H_INCLUDED