#pragma once
#include <map>
#include <string>
#include "Engine/ThirdParty/tinyxml2.h"
class SpriteAnimSet;

class ActorDefinition
{
public:

	explicit ActorDefinition(const tinyxml2::XMLElement& xmlElement);
	static std::map <std::string,ActorDefinition*> s_definitions;
	std::string name;
	std::string physicsRadius;
	std::string cosmeticRadius;
	bool canFly=false;
	bool canWalk=false;
	bool canSwim=false;
	SpriteAnimSet* m_spriteAnims;

};