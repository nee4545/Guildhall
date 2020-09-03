#include "Game/MapRegionType.hpp"
#include "Game/MapMaterial.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Game/GameCommon.hpp"

typedef tinyxml2::XMLDocument XMLDocument;
typedef tinyxml2::XMLElement XMLElement;

std::map<std::string , MapRegionType*> MapRegionType::s_regionTypes;

void MapRegionType::LoadDefinitions( char const* dataFilePath )
{
	XMLDocument doc;
	doc.LoadFile( dataFilePath );

	g_theConsole.PrintString(Rgba8(100,100,100,255),"Parsing Map Regions");
	XMLElement* root = doc.RootElement();

	for ( XMLElement* ele = root->FirstChildElement( "RegionType" ); ele != nullptr; ele = ele->NextSiblingElement( "RegionType" ) )
	{
		MapRegionType* newMapRegion = new MapRegionType( ele );
		auto found = MapRegionType::s_regionTypes.find( newMapRegion->m_name );

		if ( found != MapRegionType::s_regionTypes.end() )
		{
			g_theConsole.PrintError( "Multiple definitions found for " + found->first );
			continue;
		}

		MapRegionType::s_regionTypes[ newMapRegion->m_name ] = newMapRegion;
	}

	std::string defaultRegion = ParseXmlAttribute( *root , "default" , "NULL" );

	auto def = s_regionTypes.find( defaultRegion );
	if(def!=s_regionTypes.end() )
	s_defaultRegion = def->second;
}

MapRegionType const* MapRegionType::GetDefinition( std::string const& name )
{
	auto found = s_regionTypes.find( name );

	if ( found != s_regionTypes.end() )
	{
		return found->second;
	}

	return nullptr;
}

MapRegionType::MapRegionType( tinyxml2::XMLElement* materialDef )
{
	m_name = ParseXmlAttribute( *materialDef , "name" , "NULL" );

	if ( m_name == "NULL" )
	{
		g_theConsole.PrintError( "Error: name not found in XML Skipping this element" );
		delete this;
		return;
	}
	
	std::string isSolid = ParseXmlAttribute( *materialDef , "isSolid" , "NULL" );

	if ( isSolid == "NULL" )
	{
		g_theConsole.PrintError( "isSolid attribute not found, assuming is not solid" );
	}
	else if ( isSolid == "false" )
	{
		m_isSolid = false;
	}
	else
	{
		m_isSolid = true;
	}

	if ( m_isSolid )
	{
		tinyxml2::XMLElement* ele = materialDef->FirstChildElement( "Side" );
		if ( ele == nullptr )
		{
			g_theConsole.PrintError( "Side element not found, skipping this element" );
			delete this;
			return;
		}
		else
		{
			std::string sideMaterial = ParseXmlAttribute( *ele , "material" , "NULL" );
			m_sideMaterial = MapMaterial::GetDefinition( sideMaterial );
		}
	}
	else
	{
		tinyxml2::XMLElement* ele1 = materialDef->FirstChildElement( "Floor" );
		tinyxml2::XMLElement* ele2 = materialDef->FirstChildElement( "Ceiling" );

		std::string floorMaterial = ParseXmlAttribute( *ele1 , "material" , "NULL" );
		m_floorMaterial = MapMaterial::GetDefinition( floorMaterial );

		std::string celilingMaterial = ParseXmlAttribute( *ele2 , "material" , "NULL" );
		m_ceilingMaterial = MapMaterial::GetDefinition( celilingMaterial );

	}
}

MapRegionType* MapRegionType::s_defaultRegion;
