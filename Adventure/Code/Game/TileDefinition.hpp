#pragma once
#include <string>
#include <map>
#include "Engine/ThirdParty/tinyxml2.h"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/AABB2.hpp"

class SpriteSheet;


class TileDefinition
{
public:
	explicit TileDefinition(const tinyxml2::XMLElement& xmlElement);
	static std::map <std::string , TileDefinition*> s_definitions;
	 static  SpriteSheet* m_spritesheet;
private:
	friend class Map;
	 
	 std::string tileName;
	 IntVec2 spriteCoords;
	 bool allowWalking;
	 bool allowFlying;
	 bool allowSwimming;
	 bool allowSight;
	 AABB2 m_spriteUVs;

	 
};