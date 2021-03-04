#pragma once
#include "Engine/Math/Vec2.hpp"

class Plane2
{
public:

	Plane2();
	Plane2( Vec2 normal , Vec2 pointOnPlane );


	bool IsPointInFrontOfPlane(Vec2 point);
	float GetSignedDistanceFromPlane( Vec2 point );

	Vec2 m_normal;
	float m_distance;

	bool operator==( const Plane2& compare ) const;
	bool operator!=( const Plane2& compare ) const;
 
};