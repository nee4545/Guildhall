#pragma once

#include "Engine/Physics/Manifold2.hpp"
class Collider2D;


struct Collision2D // PoD type - Plane Old Data
{
	Collider2D* me;
	Collider2D* them;
	Manifold2 manifold;
};