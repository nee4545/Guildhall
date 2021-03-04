#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/MathUtils.hpp"

Vec4 Rgba8::GetAsNormalizedVec4()
{
	Vec4 toReturn;

	toReturn.x = RangeMapFloat( ( float ) 0 , ( float ) 255 , 0.f , 1.f , ( float ) r );
	toReturn.y = RangeMapFloat( ( float ) 0 , ( float ) 255 , 0.f , 1.f , ( float ) g );
	toReturn.z = RangeMapFloat( ( float ) 0 , ( float ) 255 , 0.f , 1.f , ( float ) b );
	toReturn.w = RangeMapFloat( ( float ) 0 , ( float ) 255 , 0.f , 1.f , ( float ) a );

	return toReturn;
}
