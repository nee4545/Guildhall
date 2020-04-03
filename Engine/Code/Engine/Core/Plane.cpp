#include "Engine/Core/Plane.hpp"
#include "Engine/Math/MathUtils.hpp"

Plane2::Plane2()
{
	m_normal = Vec2( 0.f , 1.f );
	m_distance = 0.f;
}

Plane2::Plane2( Vec2 normal , Vec2 pointOnPlane )
{
	m_normal = normal;
	m_distance = DotProduct2D( pointOnPlane , normal );
}

bool Plane2::IsPointInFrontOfPlane( Vec2 point )
{
	float dist = DotProduct2D( point , m_normal );
	float distanceFromPlane = dist - m_distance;

	return distanceFromPlane > 0.f;
}

float Plane2::GetSignedDistanceFromPlane( Vec2 point )
{
	float dist = DotProduct2D( point , m_normal );
	float distanceFromPlane = dist - m_distance;

	return distanceFromPlane;
}

bool Plane2::operator!=( const Plane2& compare ) const
{
	return !( *this == compare );
}

bool Plane2::operator==( const Plane2& compare ) const
{
	float nearZero = 0.00001f;

	if ( ( m_normal - compare.m_normal ).GetLength() <= nearZero && m_distance - compare.m_distance <= nearZero )
	{
		return true;
	}

	return false;
}

