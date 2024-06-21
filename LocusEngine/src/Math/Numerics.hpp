#pragma once

#include "Base/Base.hpp"
#include <cmath>

constexpr f64 PI		= 3.14159265358979323846;
constexpr f64 TAU 		= PI * 2.0;

constexpr f64 DEG2RAD 	= PI / 180.0;
constexpr f64 RAD2DEG 	= 180.0 / PI;

constexpr f64 E			= 2.71828182845904523536;
constexpr f64 INF		= INFINITY;

struct LVector2;
struct LVector3;
struct LVector4;

struct LQuat;

namespace Locus::Math 
{
	f32 Sin(f32 Angle);
	f32 Cos(f32 Angle);
	f32 Tan(f32 Angle);
	
	f32 Min(f32 X, f32 Y);
	f32 Max(f32 X, f32 Y);
	f32 Clamp(f32 X, f32 Min, f32 Max);

	f32 Lerp(f32 Min, f32 Max, f32 T);
	f32 LerpClamped(f32 Min, f32 Max, f32 T);
	
	i32 Min(i32 X, i32 Y);
	i32 Max(i32 X, i32 Y);
	i32 Clamp(i32 X, i32 Min, i32 Max);
	
	u32 Min(u32 X, u32 Y);
	u32 Max(u32 X, u32 Y);
	u32 Clamp(u32 X, u32 Min, u32 Max);
}