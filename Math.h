#pragma once
#include "Primitives.h"
#include <math.h>
#include <vector>

namespace Math
{
	struct vector2
	{
		uint32 X, Y;
	};

	struct vector3
	{
		uint32 X, Y, Z;
	};

	struct vector4
	{
		uint32 X, Y, Z, W;
	};

	struct vector2F
	{
		float X, Y;
	};

	struct vector3F
	{
		float X, Y, Z;
	};

	struct vector4F
	{
		float X, Y, Z, W;
	};

	static double pi() { return atan(1) * 4; }

	static double ConvertRadiansToDegrees(float rad)
	{
		return rad * ( 180.0f / pi() );
	}

	static bool IsValueInVector(uint32 value, std::vector<int32> vec)
	{
		for (uint32 i = 0; i < vec.size(); i++)
		{
			if (vec[i] == value)
			{
				return true;
			}
		}
		return false;
	}
}

