#include "Game/TileDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/Game.hpp"


std::map <std::string,TileDefinition*> TileDefinition::s_definitions;
SpriteSheet* TileDefinition::m_spritesheet = nullptr;

TileDefinition::TileDefinition( const tinyxml2::XMLElement& xmlElement )
{
	tileName = ParseXmlAttribute(xmlElement,"name","UNNAMED");
	allowFlying = ParseXmlAttribute(xmlElement,"allowsFlying",false);
	allowSight = ParseXmlAttribute(xmlElement,"allowsSight",false);
	allowWalking = ParseXmlAttribute(xmlElement,"allowsWalking",false);
	allowSwimming = ParseXmlAttribute(xmlElement,"allowsSwimming",false);
	spriteCoords = ParseXmlAttribute(xmlElement,"spriteCoords",IntVec2(0,0));
	
	if( m_spritesheet == nullptr )
	{
		m_spritesheet = g_tileSpriteSheet;
	}

	IntVec2 dimensions = m_spritesheet->GetDimensions();
	int index = spriteCoords.x + ( dimensions.x * spriteCoords.y );

	const SpriteDefinition& spr =m_spritesheet->GetSpriteDefinition( index );
	Vec2 uvMins;
	Vec2 uvMaxs;
	spr.GetUVs( uvMins, uvMaxs );
	m_spriteUVs.mins = uvMins;
	m_spriteUVs.maxs = uvMaxs;
	
	s_definitions[tileName] = this;
}
