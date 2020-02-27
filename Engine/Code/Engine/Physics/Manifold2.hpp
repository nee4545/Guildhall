#pragma once

#include "Engine/Math/Vec2.hpp"

struct Manifold2
{
	Vec2 centre;
	Vec2 normal;
	float penetration;
};