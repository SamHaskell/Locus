#include "Logging.hpp"

#include "Defines.hpp"
#include <cstdarg>
#include <cstdio>

static void LogMsgf_Impl(const char *Category, Locus::LogSeverity Severity, const char *FormatString, va_list Args)
{
	static const arch BufferLength = 512;
	char Message[BufferLength];
	
	static const char* const LogSeverityStrings[5] = {
	    [Locus::Error] = "Error",
	    [Locus::Warning] = "Warning",
	    [Locus::Debug]  = "Debug",
	    [Locus::Info]  = "Info",
	    [Locus::Trace] = "Trace"
	};
	
	static u8 LogSeverityColorCodes[5] = {
	    [Locus::Error] = 31,
	    [Locus::Warning] = 33,
	    [Locus::Debug]  = 34,
	    [Locus::Info]  = 32,
	    [Locus::Trace] = 0
	};
	
	vsnprintf(Message, BufferLength, FormatString, Args);
	printf("\033[%dm[%s] %s: %s\033[0m\n", LogSeverityColorCodes[Severity], Category, LogSeverityStrings[Severity], Message);
}

void Locus::LogAssertionFailureMsg(const char* Expr, const char* File, u32 Line, const char* Message)
{
	LogMsgf("Assert", Error, "%s in %s:%d. %s", Expr, File, Line, Message);
}

void Locus::LogAssertionFailure(const char* Expr, const char* File, u32 Line)
{
	LogMsgf("Assert", Error, "%s in %s:%d", Expr, File, Line);
}

void Locus::LogMsgf(const char *Category, Locus::LogSeverity Severity, const char *FormatString, ...)
{
	va_list Args;
	va_start(Args, FormatString);
	LogMsgf_Impl(Category, Severity, FormatString, Args);
	va_end(Args);
}