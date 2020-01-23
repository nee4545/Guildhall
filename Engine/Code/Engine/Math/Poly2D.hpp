#pragma once
#include "vector"
#include "Engine/Math/Vec2.hpp"

class Poly2D
{
public:

	int numPoints;
	std::vector<Vec2> points;

	explicit Poly2D( Vec2* points , int numPoints );
};