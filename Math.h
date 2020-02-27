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
		vector4F() { vector4F( 0, 0, 0, 0 ); }
		vector4F( float x, float y, float z, float w ) : X( x ), Y( y ), Z( z ), W( w ) {};
		float X, Y, Z, W;
	};

	static double pi() { return atan(1) * 4; }

	static double ConvertRadiansToDegrees(float rad)
	{
		return rad * ( 180.0f / pi() );
	}


	static bool operator==( const vector4F& lhs, const vector4F& rhs )
	{
		return ( lhs.X == rhs.X && lhs.Y == rhs.Y && lhs.Z == rhs.Z && lhs.W == rhs.W );
	}
}

