#include "Game/MapDefinition.hpp"

#include "Engine/Core/XmlUtils.hpp"

std::map <std::string,MapDefinition*> MapDefinition::s_definitions;

MapDefinition::MapDefinition( const tinyxml2::XMLElement& definitionXmlElement )
{
	mapName = ParseXmlAttribute(definitionXmlElement,"name","None");
	fillTileType = ParseXmlAttribute(definitionXmlElement,"fillTile","None");
	edgeTileType = ParseXmlAttribute(definitionXmlElement,"edgeTile","None");
	width = ParseXmlAttribute(definitionXmlElement,"width",0);
	height = ParseXmlAttribute(definitionXmlElement,"height",0);

	const tinyxml2::XMLElement* generationElements = definitionXmlElement.FirstChildElement();

	if ( generationElements )
	{
		generationElements = generationElements->FirstChildElement();

		while ( generationElements )
		{
			generationStep.push_back( MapGenStep::CreateMapGenStep( *generationElements ) );
			generationElements = generationElements->NextSiblingElement();
		}
	}

	s_definitions[mapName]=this;
}


