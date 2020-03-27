#include "Engine/Math/MatrixUtils.hpp"

Mat44 MakeOrthographicProjectionMatrixD3D( const Vec3& min , const Vec3& max )
{
	Vec3 diff = max - min;
	Vec3 sum = max + min;

	float mat[] = { 2.0f / diff.x ,    0.f,         0.f,             0.f,
					0.f,               2.0f / diff.y, 0.f,             0.f,
					0.f,               0.f,         1.0f / diff.z,     0.f,
					-sum.x / diff.x,    -sum.y / diff.y,-min.x / diff.z,   1.f };


	return Mat44( mat );
}



