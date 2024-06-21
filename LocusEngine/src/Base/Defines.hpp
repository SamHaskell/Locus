#pragma once

#include <cstdint>
#include <stddef.h>

#define STR(x) STR_IMPL(x)
#define STR_IMPL(x) #x

#define CHECK_SIZE_COMPTIME(type, size) static_assert(sizeof(type) == size, "Size of type does not match the expected size.");

typedef uint8_t 	u8; 	CHECK_SIZE_COMPTIME(u8, 1)
typedef uint16_t 	u16;	CHECK_SIZE_COMPTIME(u16, 2)
typedef uint32_t 	u32;	CHECK_SIZE_COMPTIME(u32, 4)
typedef uint64_t 	u64;	CHECK_SIZE_COMPTIME(u64, 8)

typedef int8_t 		i8;		CHECK_SIZE_COMPTIME(i8, 1)
typedef int16_t 	i16;	CHECK_SIZE_COMPTIME(i16, 2)
typedef int32_t 	i32;	CHECK_SIZE_COMPTIME(i32, 4)
typedef int64_t 	i64;	CHECK_SIZE_COMPTIME(i64, 8)

typedef float		f32;	CHECK_SIZE_COMPTIME(f32, 4)
typedef double		f64;	CHECK_SIZE_COMPTIME(f64, 8)

typedef size_t		arch; 	// Do not check as we expect this to vary.

#if defined(_WIN32) || defined(_WIN64)
	#define LOCUS_PLATFORM_WINDOWS 	1
#elif defined(__APPLE__) && defined(__MACH__)
	#define LOCUS_PLATFORM_MACOS 	1
#elif defined (__linux__)
	#define LOCUS_PLATFORM_LINUX	1
#else
	#define LOCUS_PLATFORM_UNKNOWN	1
#endif

#if !defined(LOCUS_PLATFORM_WINDOWS)
	#define LOCUS_PLATFORM_WINDOWS	0
#endif

#if !defined(LOCUS_PLATFORM_MACOS)
	#define LOCUS_PLATFORM_MACOS	0
#endif

#if !defined(LOCUS_PLATFORM_LINUX)
	#define LOCUS_PLATFORM_LINUX	0
#endif

#if !defined(LOCUS_PLATFORM_UNKNOWN)
	#define LOCUS_PLATFORM_UNKNOWN	0
#endif

static_assert(LOCUS_PLATFORM_UNKNOWN == 0, "Locus does not recognize your platform/OS.");

namespace Locus
{
	enum class PlatformAPI
	{
		SDL,
		None
	};
	
	enum class RenderAPI
	{
		OpenGL,
		None
	};
};


#define LAPI // for DLL symbol exports, currently nothing because we are developing on MacOS