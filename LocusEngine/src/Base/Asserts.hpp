#pragma once

#include "Logging.hpp"
#include "assert.h"

// When disabled, all code inside an Assert will be stripped
// When disabled, code inside a Check will still run

#define LOCUS_USE_ASSERTS 1

#if LOCUS_USE_ASSERTS
	#define LAssertMsg(Expr, Message) do { \
		if (Expr) { } \
		else \
		{ \
			Locus::LogAssertionFailureMsg(#Expr, __FILE__, __LINE__, Message); \
			assert(Expr); \
		} \
	} while (0)
	
	#define LAssert(Expr) do { \
		if (Expr) { } \
		else \
		{ \
			Locus::LogAssertionFailure(#Expr, __FILE__, __LINE__); \
			assert(Expr); \
		} \
	} while (0)
#else
	#define LAssertMsg(Expr, Message)
	#define LAssert(Expr, Message)
#endif

#define LOCUS_USE_CHECKS 1

#if LOCUS_USE_CHECKS
	#define LCheckMsg(Expr, Message) do { \
		if (Expr) { } \
		else \
		{ \
			Locus::LogAssertionFailureMsg(#Expr, __FILE__, __LINE__, Message); \
			assert(Expr); \
		} \
	} while (0)
	
	#define LCheck(Expr) do { \
		if (Expr) { } \
		else \
		{ \
			Locus::LogAssertionFailure(#Expr, __FILE__, __LINE__); \
			assert(Expr); \
		} \
	} while (0)
#else
	#define LCheckMsg(Expr, Message) do { Expr } while (0)
	#define LCheck(Expr, Message) do { Expr } while (0)
#endif