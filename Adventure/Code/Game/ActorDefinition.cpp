#include "Game/ActorDefinition.hpp"
#include "Engine/Renderer/SpriteAnimSet.hpp"
#include "Engine/Core/XmlUtils.hpp"


std::map <std::string,ActorDefinition*> ActorDefinition::s_definitions;

ActorDefinition::ActorDefinition( const tinyxml2::XMLElement& xmlElement )
{
	name = ParseXmlAttribute( xmlElement, "name", "UNNAMED" );
	physicsRadius = ParseXmlAttribute( xmlElement, "physicsRadius", "None" );
	cosmeticRadius = ParseXmlAttribute( xmlElement, "cosmeticRadius", "None" );
	canWalk = ParseXmlAttribute( xmlElement, "canWalk", false );
	canSwim = ParseXmlAttribute( xmlElement, "canSwim", false );
	canFly = ParseXmlAttribute( xmlElement, "canFly", false );
	
	const tinyxml2::XMLElement* animElement = xmlElement.FirstChildElement( "SpriteAnimSet" );
	m_spriteAnims = new SpriteAnimSet( *animElement );

	s_definitions[name] = this;
	
}
