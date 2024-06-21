#include "Math/Numerics.hpp"

#include <math.h>

f32 Locus::Math::Sin(f32 Angle)
{
	return sinf(Angle);
}

f32 Locus::Math::Cos(f32 Angle)
{
	return cosf(Angle);
}

f32 Locus::Math::Tan(f32 Angle)
{
	return tanf(Angle);
}

f32 Locus::Math::Min(f32 X, f32 Y)
{
	return X < Y ? X : Y;
}

f32 Locus::Math::Max(f32 X, f32 Y)
{
	return X > Y ? X : Y;
}

f32 Locus::Math::Clamp(f32 X, f32 Min, f32 Max)
{
	return X < Min ? Min : (X > Max ? Max : X); 
}

f32 Locus::Math::Lerp(f32 Min, f32 Max, f32 T)
{
	return Min + ((Max - Min) * T);
}

f32 Locus::Math::LerpClamped(f32 Min, f32 Max, f32 T)
{
	f32 ClampedT = Locus::Math::Clamp(T, Min, Max);
	return Lerp(Min, Max, ClampedT);
}

i32 Locus::Math::Min(i32 X, i32 Y)
{
	return X < Y ? X : Y;
}

i32 Locus::Math::Max(i32 X, i32 Y)
{
	return X > Y ? X : Y;
}

i32 Locus::Math::Clamp(i32 X, i32 Min, i32 Max)
{
	return X < Min ? Min : (X > Max ? Max : X);
}

u32 Locus::Math::Min(u32 X, u32 Y)
{
	return X < Y ? X : Y;
}

u32 Locus::Math::Max(u32 X, u32 Y)
{
	return X > Y ? X : Y;
}

u32 Locus::Math::Clamp(u32 X, u32 Min, u32 Max)
{
	return X < Min ? Min : (X > Max ? Max : X);
}