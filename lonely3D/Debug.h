#pragma once
#ifndef __Debug_H__
#define __Debug_H__
#include "stdafx.h"

#include <string>
#include "DataType.h"
#include "lMathf.h"
#include "Types.h"
#include <sstream>
#include "Color.h"

using namespace std;

inline static string Convert(const int32 value)
{
	string res;
	stringstream ss;
	ss << value;
	ss >> res;
	return res;
}

inline static string Convert(float32 value)
{
	return std::to_string(value);
}

template<class T>
inline static string Convert(T value)
{
	return std::to_string(value);
}

inline static string Convert(float64 value)
{
	std::string data = std::to_string(value);
	return data;
}

inline static string Convert(const uint32 value)
{
	string res;
	stringstream ss;
	ss << value;
	ss >> res;
	return res;
}


class Logger
{
public:
	void SetLevel(CL_LOG_LEVEL ll)
	{
		LogLevel = ll;
	}

	void Log(const char* message, int32 flags = 0)
	{
		string tmp(message);
		tmp += "\n";
		//OutputDebugStringA(tmp.c_str());

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WORD(flags));
		printf("%s", tmp.c_str());

		SetConsoleTextAttribute(
			GetStdHandle(STD_OUTPUT_HANDLE),
			FOREGROUND_INTENSITY | FOREGROUND_RED |
			FOREGROUND_GREEN | FOREGROUND_BLUE);//设置三色相加
	}

	void Log(const char* message, CL_LOG_LEVEL ll)
	{
		if (ll < LogLevel)
			return;

		string info = message;
		if (ll == CL_Debug)
		{
			//  
			Log(("日志:[" + info + "]").c_str(), FOREGROUND_INTENSITY);
		}
		else if (ll == CL_ERROR)
		{
			// 
			Log(("错误:[" + info + "]").c_str(), FOREGROUND_INTENSITY | FOREGROUND_RED);
		}
		else if (ll == CL_WARNING)
		{
			// 
			Log(("警告:[" + info + "]").c_str(), FOREGROUND_INTENSITY | FOREGROUND_BLUE);
		}
	}

	void Log(char message)
	{
		string tmp(&message);
		tmp += "\n";
		//OutputDebugStringA(tmp.c_str());
		printf("%s", tmp.c_str());
	}

	void Log(char message, CL_LOG_LEVEL ll)
	{
		if (ll < LogLevel)
			return;
		Log(message);
	}

private:

	CL_LOG_LEVEL LogLevel;
};

static Logger* _logger = new Logger();


class Debug
{
public:
	static void SetLevel(CL_LOG_LEVEL ll);

	template<class T>
	static void LogInfo(T message);
	template<class T>
	static void LogError(T message);
	template<class T>
	static void LogWarning(T message);

	static void LogInfo(int message);
	static void LogError(int message);
	static void LogWarning(int message);

	static void LogInfo(char* message);
	static void LogError(char* message);
	static void LogWarning(char* message);

	static void LogInfo(char message);
	static void LogError(char message);
	static void LogWarning(char message);

	static void LogInfo(const char* message);
	static void LogError(const char* message);
	static void LogWarning(const char* message);

	static void LogInfo(string message);
	static void LogError(string message);
	static void LogWarning(string message);

	static void LogInfo(string name, ColorU32 color);
	static void LogError(string name, ColorU32 color);
	static void LogWarning(string name, ColorU32 color);

	static void LogInfo(string name, Vector4 vec);
	static void LogError(string name, Vector4 vec);
	static void LogWarning(string name, Vector4 vec);

	static void LogInfo(string name, Vector3 vec);
	static void LogError(string name, Vector3 vec);
	static void LogWarning(string name, Vector3 vec);

	static void LogInfo(string name, Vector2 vec);
	static void LogError(string name, Vector2 vec);
	static void LogWarning(string name, Vector2 vec);

	static void LogInfo(string name, Quaternion qua);
	static void LogError(string name, Quaternion qua);
	static void LogWarning(string name, Quaternion qua);

	static void LogInfo(string name, Matrix4x4 mat);
	static void LogError(string name, Matrix4x4 mat);
	static void LogWarning(string name, Matrix4x4 mat);
};



inline void Debug::SetLevel(CL_LOG_LEVEL ll)
{
	_logger->SetLevel(ll);
}

inline void Debug::LogInfo(char* message)
{
	_logger->Log(message, CL_Debug);
}

inline void Debug::LogError(char* message)
{
	_logger->Log(message, CL_ERROR);
}


inline void Debug::LogWarning(char* message)
{
	_logger->Log(message, CL_WARNING);
}

inline void Debug::LogInfo(const char* message)
{
	_logger->Log(message, CL_Debug);
}

inline void Debug::LogError(const char* message)
{
	_logger->Log(message, CL_ERROR);
}


inline void Debug::LogWarning(const char* message)
{
	_logger->Log(message, CL_WARNING);
}

inline void Debug::LogError(string message)
{
	_logger->Log(message.c_str(), CL_ERROR);
}


inline void Debug::LogWarning(string message)
{
	_logger->Log(message.c_str(), CL_WARNING);
}

inline void Debug::LogInfo(string message)
{
	_logger->Log(message.c_str(), CL_Debug);
}


inline void Debug::LogInfo(char message)
{
	_logger->Log(message, CL_Debug);
}
inline void Debug::LogError(char message)
{
	_logger->Log(message, CL_ERROR);
}
inline void Debug::LogWarning(char message)
{
	_logger->Log(message, CL_WARNING);
}

template<class T>
inline void Debug::LogInfo(T message)
{
	string res;
	stringstream ss;
	ss << message;
	ss >> res;

	_logger->Log(res.c_str(), CL_Debug);
}
template<class T>
inline void Debug::LogError(T message)
{
	string res;
	stringstream ss;
	ss << message;
	ss >> res;

	_logger->Log(res.c_str(), CL_ERROR);
}

template<class T>
inline void Debug::LogWarning(T message)
{
	string res;
	stringstream ss;
	ss << message;
	ss >> res;
	_logger->Log(res.c_str(), CL_WARNING);
}


inline void Debug::LogInfo(int message)
{
	string res;
	stringstream ss;
	ss << message;
	ss >> res;
	_logger->Log(res.c_str(), CL_Debug);
}
inline void Debug::LogError(int message)
{
	string res;
	stringstream ss;
	ss << message;
	ss >> res;
	_logger->Log(res.c_str(), CL_ERROR);
}
inline void Debug::LogWarning(int message)
{
	string res;
	stringstream ss;
	ss << message;
	ss >> res;
	_logger->Log(res.c_str(), CL_WARNING);
}


inline void Debug::LogInfo(string name, Vector4 vec)
{
	string context = name+"{" +
		Convert(vec.x) + "," +
		Convert(vec.y) + "," +
		Convert(vec.z) + "," +
		Convert(vec.w) + "}";

	_logger->Log(context.c_str(), CL_Debug);
}
inline void Debug::LogError(string name, Vector4 vec)
{
	string context = name+"{" +
		Convert(vec.x) + "," +
		Convert(vec.y) + "," +
		Convert(vec.z) + "," +
		Convert(vec.w) + "}";
	_logger->Log(context.c_str(), CL_ERROR);
}
inline void Debug::LogWarning(string name, Vector4 vec)
{
	string context = name+"{" +
		Convert(vec.x) + "," +
		Convert(vec.y) + "," +
		Convert(vec.z) + "," +
		Convert(vec.w) + "}";
	_logger->Log(context.c_str(), CL_WARNING);
}


inline void Debug::LogInfo(string name, Matrix4x4 mat)
{
	string context = "标签:"+name+"\n" +
		Convert(mat.Get(0, 0)) + "," +
		Convert(mat.Get(0, 1)) + "," +
		Convert(mat.Get(0, 2)) + "," +
		Convert(mat.Get(0, 3)) + "\n" +

		Convert(mat.Get(1, 0)) + "," +
		Convert(mat.Get(1, 1)) + "," +
		Convert(mat.Get(1, 2)) + "," +
		Convert(mat.Get(1, 3)) + "\n" +

		Convert(mat.Get(2, 0)) + "," +
		Convert(mat.Get(2, 1)) + "," +
		Convert(mat.Get(2, 2)) + "," +
		Convert(mat.Get(2, 3)) + "\n" +

		Convert(mat.Get(3, 0)) + "," +
		Convert(mat.Get(3, 1)) + "," +
		Convert(mat.Get(3, 2)) + "," +
		Convert(mat.Get(3, 3)) + "\n";

	_logger->Log(context.c_str(), CL_Debug);
}
inline void Debug::LogError(string name, Matrix4x4 mat)
{
	string context = name+"\n" +
		Convert(mat.Get(0, 0)) + "," +
		Convert(mat.Get(0, 1)) + "," +
		Convert(mat.Get(0, 2)) + "," +
		Convert(mat.Get(0, 3)) + "\n" +

		Convert(mat.Get(1, 0)) + "," +
		Convert(mat.Get(1, 1)) + "," +
		Convert(mat.Get(1, 2)) + "," +
		Convert(mat.Get(1, 3)) + "\n" +

		Convert(mat.Get(2, 0)) + "," +
		Convert(mat.Get(2, 1)) + "," +
		Convert(mat.Get(2, 2)) + "," +
		Convert(mat.Get(2, 3)) + "\n" +

		Convert(mat.Get(3, 0)) + "," +
		Convert(mat.Get(3, 1)) + "," +
		Convert(mat.Get(3, 2)) + "," +
		Convert(mat.Get(3, 3)) + "\n";
	_logger->Log(context.c_str(), CL_ERROR);
}
inline void Debug::LogWarning(string name, Matrix4x4 mat)
{
	string context = name+"\n" +
		Convert(mat.Get(0, 0)) + "," +
		Convert(mat.Get(0, 1)) + "," +
		Convert(mat.Get(0, 2)) + "," +
		Convert(mat.Get(0, 3)) + "\n" +

		Convert(mat.Get(1, 0)) + "," +
		Convert(mat.Get(1, 1)) + "," +
		Convert(mat.Get(1, 2)) + "," +
		Convert(mat.Get(1, 3)) + "\n" +

		Convert(mat.Get(2, 0)) + "," +
		Convert(mat.Get(2, 1)) + "," +
		Convert(mat.Get(2, 2)) + "," +
		Convert(mat.Get(2, 3)) + "\n" +

		Convert(mat.Get(3, 0)) + "," +
		Convert(mat.Get(3, 1)) + "," +
		Convert(mat.Get(3, 2)) + "," +
		Convert(mat.Get(3, 3)) + "\n";
	_logger->Log(context.c_str(), CL_WARNING);
}

inline void Debug::LogInfo(string name, Vector3 vec)
{
	string context = name+"{" +
		Convert(vec.x) + "," +
		Convert(vec.y) + "," +
		Convert(vec.z) + "}";

	_logger->Log(context.c_str(), CL_Debug);
}

inline void Debug::LogError(string name, Vector3 vec)
{
	string context = name+"{" +
		Convert(vec.x) + "," +
		Convert(vec.y) + "," +
		Convert(vec.z) + "}";
	_logger->Log(context.c_str(), CL_ERROR);
}

inline void Debug::LogWarning(string name, Vector3 vec)
{
	string context = name+"{" +
		Convert(vec.x) + "," +
		Convert(vec.y) + "," +
		Convert(vec.z) + "}";
	_logger->Log(context.c_str(), CL_WARNING);
}

inline void Debug::LogInfo(string name, Vector2 vec)
{
	string context = name + "{" +
		Convert(vec.x) + "," +
		Convert(vec.y) + "}";

	_logger->Log(context.c_str(), CL_Debug);
}

inline void Debug::LogError(string name, Vector2 vec)
{
	string context = name + "{" +
		Convert(vec.x) + "," +
		Convert(vec.y) + "}";
	_logger->Log(context.c_str(), CL_ERROR);
}

inline void Debug::LogWarning(string name, Vector2 vec)
{
	string context = name + "{" +
		Convert(vec.x) + "," +
		Convert(vec.y) + "}";
	_logger->Log(context.c_str(), CL_WARNING);
}

inline void Debug::LogInfo(string name, Quaternion qua)
{
	string context = name+"{" +
		Convert(qua.x) + "," +
		Convert(qua.y) + "," +
		Convert(qua.z) + "," +
		Convert(qua.w) + "}";

	_logger->Log(context.c_str(), CL_Debug);
}

inline void Debug::LogError(string name, Quaternion qua)
{
	string context = name+"{" +
		Convert(qua.x) + "," +
		Convert(qua.y) + "," +
		Convert(qua.z) + "," +
		Convert(qua.w) + "}";
	_logger->Log(context.c_str(), CL_ERROR);
}

inline void Debug::LogWarning(string name, Quaternion qua)
{
	string context = name+"{" +
		Convert(qua.x) + "," +
		Convert(qua.y) + "," +
		Convert(qua.z) + "," +
		Convert(qua.w) + "}";
	_logger->Log(context.c_str(), CL_WARNING);
}


inline void Debug::LogInfo(string name, ColorU32 color)
{
	string context = name + "{A=" +
		Convert(color.GetAlpha()) + ",R=" +
		Convert(color.GetRed()) + ",G=" +
		Convert(color.GetGreen()) + ",B=" +
		Convert(color.GetBlue()) + "}";

	_logger->Log(context.c_str(), CL_Debug);
}

inline void Debug::LogError(string name, ColorU32 color)
{
	string context = name + "{A=" +
		Convert(color.GetAlpha()) + ",R=" +
		Convert(color.GetRed()) + ",G=" +
		Convert(color.GetGreen()) + ",B=" +
		Convert(color.GetBlue()) + "}";
	_logger->Log(context.c_str(), CL_ERROR);
}

inline void Debug::LogWarning(string name, ColorU32 color)
{
	string context = name + "{A=" +
		Convert(color.GetAlpha()) + ",R=" +
		Convert(color.GetRed()) + ",G=" +
		Convert(color.GetGreen()) + ",B=" +
		Convert(color.GetBlue()) + "}";
	_logger->Log(context.c_str(), CL_WARNING);
}



#endif

