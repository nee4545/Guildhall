#include "Game/World.hpp"
#include "Game/Map.hpp"
#include "Game/TileMap.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/ThirdParty/tinyxml2.h"
#include "Engine/Core/XmlUtils.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/Actor.hpp"

World::World(Game* game)
{
	m_game = game;
}

void World::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	m_currentMap->Update( deltaSeconds );
	m_currentMap->UpdateMeshes();
}

void World::Render()
{
	m_currentMap->Render();
}

void World::LoadMap( std::string mapName )
{
	auto found = m_maps.find( mapName );

	if ( found != m_maps.end() )
	{
		g_theConsole.PrintString( Rgba8( 0 , 0 , 0 , 255 ) , "Loading map: " + mapName );
		g_theAudio->PlaySound(m_sound,false,1.f);
		m_currentMap = found->second;

		m_game->m_camera->m_transform.m_position = Vec3( Vec2(m_currentMap->m_playerStartPos.x, m_currentMap->m_playerStartPos.y ), 0.65f );
		m_game->m_yaw = m_currentMap->m_playerStartYaw;

		//Actor* a = new Actor( Vec2( m_currentMap->m_playerStartPos.x , m_currentMap->m_playerStartPos.y ) );
		//m_currentMap->m_entities.push_back( a );
	}
	else
	{
		g_theConsole.PrintError( "no maps with name:" + mapName + " exists" );
	}
	
}

Map* World::GetMap( std::string mapName )
{
	auto found = m_maps.find( mapName );

	if ( found != m_maps.end() )
	{
		//g_theAudio->PlaySound( m_sound , false , 1.f );
		return found->second;
	}

	return nullptr;
}

void World::LoadAllMaps( std::string folderPath )
{
	std::vector<std::string> filenames;

	filenames = GetFileNamesInFolder( folderPath , "*.xml" );

	for ( int i = 0; i < filenames.size(); i++ )
	{
		tinyxml2::XMLDocument mapDoc;

		std::string xmlPath = folderPath + "/" + filenames[ i ];

		mapDoc.LoadFile( xmlPath.c_str() );

		tinyxml2::XMLElement* ele = mapDoc.RootElement();
		std::size_t pos = filenames[ i ].find( ".xml" );
		std::string mapName = filenames[ i ].substr( 0 , pos );
		std::string mapType = ParseXmlAttribute( *ele , "type" , "NULL" );

		if ( mapType == "NULL" )
		{
			g_theConsole.PrintError( "Error:: Map Type not specified in"+filenames[i] );
			continue;
		}
		
		g_theConsole.PrintString( Rgba8( 0 , 0 , 100 , 255 ) , "Loading map:" + mapName );
		CreateMap( mapType , mapName , ele );
	}
	
}

void World::CreateMap( std::string mapType, std::string mapName ,tinyxml2::XMLElement* mapElement )
{
	if ( mapType == "TileMap" )
	{
		TileMap* newMap = new TileMap( mapName.c_str() , mapElement );
		m_maps[ mapName ] = newMap;
	}
}

Strings World::GetAllCurrentMapNames()
{
	Strings toReturn;
	auto itr = m_maps.begin();

	while ( itr != m_maps.end() )
	{
		toReturn.push_back( itr->first );
		itr++;
	}

	return toReturn;
}
