#include "Engine/Core/AABB3.hpp"

AABB3::AABB3( const AABB3& copyFrom )
{
	*this = copyFrom;
}

AABB3::AABB3( const Vec3& mins , const Vec3& maxs )
{
	this->mins = mins;
	this->maxs = maxs;
}

AABB3::AABB3( float minX , float minY , float minZ , float maxX , float maxY , float maxZ )
{
	mins.x = minX;
	mins.y = minY;
	mins.z = minZ;

	maxs.x = maxX;
	maxs.y = maxY;
	maxs.z = maxZ;
}
