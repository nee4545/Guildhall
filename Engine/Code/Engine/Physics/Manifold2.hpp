#pragma once

#include "Engine/Math/Vec2.hpp"

struct Manifold2
{
	Vec2 contactPoint1;
	Vec2 contactPoint2;
	Vec2 normal;
	float penetration;
};