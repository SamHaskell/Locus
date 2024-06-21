#pragma once

#include "Defines.hpp"

namespace Locus
{
	enum LogSeverity
	{
		Error,
		Warning,
		Debug,
		Info,
		Trace
	};

	void LogMsgf(const char* Category, LogSeverity Severity, const char* FormatString, ...);
	void LogAssertionFailureMsg(const char* Expr, const char* File, u32 Line, const char* Message);
	void LogAssertionFailure(const char* Expr, const char* File, u32 Line);
}

#define LLOG(Category, Severity, Message, ...) do { \
	Locus::LogMsgf(STR(Category), Severity, Message, ##__VA_ARGS__); \
} while (0)