#pragma once
#pragma once
#include <Engine\Math\Vec3.hpp>

class IntVec2;

struct AABB3
{
public:
	Vec3 m_mins;
	Vec3 m_maxs;

public:
	AABB3() = default;
	~AABB3() {};
	AABB3( const AABB3& copyFrom );
	explicit AABB3( const Vec3& mins , const Vec3& maxs );
	explicit AABB3( float minX , float minY , float minZ , float maxX , float maxY , float maxZ );


	bool IsPointInside( const Vec2& point )const;
	void Translate( Vec3 translation );
};