#pragma once
#include "Game/TileDefinition.hpp"
#include "Engine/ThirdParty/tinyxml2.h"
#include "Engine/Core/FloatRange.hpp"
#include "Engine/Core/IntRange.hpp"
#include <string>

class Map;

class MapGenStep
{
public:
	MapGenStep(const tinyxml2::XMLElement& stepElement);
	virtual ~MapGenStep (){};

	void RunStep(Map& map);
	virtual void RunStepOnce(Map& map)=0;

	static MapGenStep* CreateMapGenStep(const tinyxml2::XMLElement& element);

public:
	std::string m_stepName;
	FloatRange m_chanceToRun= FloatRange(1.f);
	FloatRange m_chancePerTile =FloatRange(1.f);
	IntRange m_numberOfIterations = IntRange(1);
	int currentGenerationStep=0;
	TileDefinition* m_ifTile;
	TileDefinition* m_setTile;

};