#pragma once
#include "Engine/ThirdParty/tinyxml2.h"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/AABB2.hpp"
#include <string>
#include <map>
#include <vector>

#include "Game/MapGenStep.hpp"

class MapDefinition
{
public:
	static std::map< std::string, MapDefinition* >	s_definitions;
	explicit MapDefinition(const tinyxml2::XMLElement& definitionXmlElement);

	friend class Map;
private:
	std::string mapName;
	std::string fillTileType;
	std::string edgeTileType;
	std::vector<MapGenStep*> generationStep;
	int width;
	int height;
};