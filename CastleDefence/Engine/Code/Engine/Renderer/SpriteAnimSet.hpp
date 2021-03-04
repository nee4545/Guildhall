#pragma once
#include "Engine/ThirdParty/tinyxml2.h"
#include <vector>
#include <map>
#include <string>

class SpriteAnimDefinition;
class SpriteSheet;

class SpriteAnimSet
{
public:

	SpriteAnimSet( const tinyxml2::XMLElement& animElement );
	SpriteAnimDefinition* GetDefinition( const std::string& name );

private:
	std::map<std::string , SpriteAnimDefinition* > m_animDefinitions;
	SpriteSheet* m_spriteSheet = nullptr;

};