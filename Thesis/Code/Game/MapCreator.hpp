#pragma once
#include <vector>
#include "Engine/Core/AABB2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
class Texture;
class SpriteSheet;

struct MapItem
{
	Texture* m_texture = nullptr;
	AABB2 m_bounds;
};

class MapCreator
{
public:
	std::vector<AABB2> m_tileVerts;
	SpriteSheet* m_tileSpriteSheet = nullptr;
};