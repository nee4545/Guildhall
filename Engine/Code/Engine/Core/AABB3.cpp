#include "Engine/Core/AABB3.hpp"

AABB3::AABB3( const AABB3& copyFrom )
{
	*this = copyFrom;
}

AABB3::AABB3( const Vec3& mins , const Vec3& maxs )
{
	this->m_mins = mins;
	this->m_maxs = maxs;
}

AABB3::AABB3( float minX , float minY , float minZ , float maxX , float maxY , float maxZ )
{
	m_mins.x = minX;
	m_mins.y = minY;
	m_mins.z = minZ;

	m_maxs.x = maxX;
	m_maxs.y = maxY;
	m_maxs.z = maxZ;
}

void AABB3::Translate( Vec3 translation )
{
	m_mins += translation;
	m_maxs += translation;
}
