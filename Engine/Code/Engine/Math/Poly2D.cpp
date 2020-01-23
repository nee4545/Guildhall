#include "Engine/Math/Poly2D.hpp"

Poly2D::Poly2D( Vec2* points , int numPoints )
{
	for ( int index = 0; index < numPoints; index++ )
	{
		this->points.push_back( points[ index ] );
	}

	this->numPoints = numPoints;
}
