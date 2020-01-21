#include "Game/Game.hpp"
#include "Game/Entity.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Map.hpp"
#include "Game/Actor.hpp"
#include "Engine/ThirdParty/tinyxml2.h"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/EngineCommon.hpp"

SpriteSheet* g_tileSpriteSheet= nullptr;

bool SomeEventOccoured( EventArgs& args )
{
	UNUSED( args );
	g_theConsole.PrintString( Rgba8( 100 , 0 , 0 , 255 ) , "Someone Fired this event" );
	return false;
}

bool SomeOtherEventOccoured( EventArgs& args )
{
	UNUSED( args );
	g_theConsole.PrintString( Rgba8( 100 , 0 , 0 , 255 ) , "Someone Fired another event" );
	return false;
}


Game::Game()
{
	rng= RandomNumberGenerator();
	m_camera=new Camera();
	m_devConsoleCamera = new Camera();

	m_camera->SetOrthoView(Vec2(0.f,0.f),Vec2(53.f,30.f));
	m_devConsoleCamera->SetOrthoView(Vec2(-20.f,-20.f),Vec2(10.f,10.f));
    g_tileSpriteSheet= new SpriteSheet(*(g_theRenderer->GetOrCreateTextureFromFile("Data/Images/Terrain_32x32.png")),IntVec2(32,32));
	m_map = nullptr;

	g_theEventSystem.SubscribeToEvent( "Demo" , SomeEventOccoured );
	g_theEventSystem.SubscribeToEvent( "Demo" , SomeOtherEventOccoured );
    
}

void Game::StartUp()
{
	//Event System Check
	g_theEventSystem.FireEvent( "Demo" , g_gameConfigBlackboard );
	g_theEventSystem.UnSubscribeToEvent( "Demo" , SomeEventOccoured );
	g_theEventSystem.FireEvent( "Demo" , g_gameConfigBlackboard );

	//Populate Map and Tile Definitions
	tinyxml2::XMLDocument mapDefDoc;
	tinyxml2::XMLDocument tileDefDoc;
	tinyxml2::XMLDocument actorDefDoc;

	m_BitmapFont = g_theRenderer->CreateBitMapFontFromFile( "Data/Fonts/SquirrelFixedFont" );
	mapDefDoc.LoadFile("Data/GamePlay/Maps.xml");
	tileDefDoc.LoadFile("Data/GamePlay/TileDefs.xml");
	actorDefDoc.LoadFile("Data/GamePlay/Actors.xml");

	tinyxml2:: XMLElement* tileElement = tileDefDoc.RootElement();
	tileElement = tileElement->FirstChildElement();

	while( tileElement )
	{
	    new TileDefinition(*tileElement);
		tileElement = tileElement->NextSiblingElement();
	}

	tinyxml2::XMLElement* mapElemet = mapDefDoc.RootElement();
	mapElemet= mapElemet->FirstChildElement();

	while( mapElemet )
	{
		new MapDefinition(*mapElemet);
		mapElemet = mapDefDoc.NextSiblingElement();
	}


	tinyxml2::XMLElement* actorElement = actorDefDoc.RootElement();
	actorElement = actorElement->FirstChildElement();

	while( actorElement )
	{
		new ActorDefinition( *actorElement );
		actorElement = actorElement->NextSiblingElement();
	}


	m_map = new Map(this,"Level1", MapDefinition::s_definitions["Island"]);

	//Populating dev console
	g_theConsole.PrintString( Rgba8( 100, 0, 0, 255 ), "Neeraj Jairam" );
	g_theConsole.PrintString( Rgba8( 0, 0, 0, 255 ), "Megha Munoli" );
	g_theConsole.PrintString( Rgba8( 0, 100, 0, 255 ), "Frank Lampard" );

	g_theConsole.PrintString( Rgba8( 0, 0, 0, 255 ), g_gameConfigBlackboard.GetValue("startLevel","None") );
	g_theConsole.PrintString( Rgba8( 0, 0, 0, 255 ), g_gameConfigBlackboard.GetValue("windowAspect","None") );
	g_theConsole.PrintString( Rgba8( 0, 0, 0, 255 ), g_gameConfigBlackboard.GetValue("isFullScreen","None") );
}

void Game::Update( float deltaseconds )
{
	ToggleDevConsole();
	ToggleDebugCamera();
	m_map->Update(deltaseconds);

	if ( m_isCameraUpdaing )
	{
		UpdateCamera();
	}
	else
	{
		m_camera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 53.f , 30.f ) );
	}
}

void Game::Render()
{
	g_theRenderer->BeginCamera(*m_camera);
	m_map->Render();

	if(g_theConsole.IsOpen() )
	{ 
	g_theConsole.Render(*g_theRenderer,*m_devConsoleCamera,1.f);
	}

	if ( !m_isCameraUpdaing )
	{
		DisplayMouseInspectorDetails();
	}
	g_theRenderer->EndCamera( *m_camera );
	
	
}



void Game::ShutDown()
{

}

void Game::UpdateCamera()
{
	Vec2 playerPos = m_map->m_player->GetPosition();
	Vec2 camCoords;

	camCoords.x = Clamp( playerPos.x , 8.f , WORLD_SIZE_X - 8 );
	camCoords.y = Clamp( playerPos.y , 4.5f , WORLD_SIZE_Y - 4.5f );

	m_camera->SetOrthoView( camCoords - Vec2( 8.f , 4.5f ) , camCoords + Vec2( 8.f , 4.5f ) );
}

void Game::ToggleDevConsole()
{
	if(g_theInput->WasKeyJustPressed(0x20) )
	{ 
		m_isConsoleOpen=!m_isConsoleOpen;
		g_theConsole.SetIsOpen( m_isConsoleOpen );
	}
}

void Game::ToggleDebugCamera()
{
	if ( g_theInput->WasKeyJustPressed( 0x70 ) )
	{
		m_isCameraUpdaing = !m_isCameraUpdaing;
	}
}

void Game::TogglePaused()
{
	
}

void Game::ToggleDebugging()
{
	
}

void Game::DisplayMouseInspectorDetails()
{
	float m_posx = RangeMapFloat( 0.f , 1.f , m_camera->GetOrthoBottomLeft().x , m_camera->GetOrthoTopRight().x , g_theInput->GetCurrentMousePosition().x );
	float m_posy = RangeMapFloat( 0.f , 1.f , m_camera->GetOrthoBottomLeft().y , m_camera->GetOrthoTopRight().y , g_theInput->GetCurrentMousePosition().y );
	g_theRenderer->DrawRing( Vec2( m_posx , m_posy ) , .2f , Rgba8( 100 , 0 , 0 , 255 ) , 0.1f );
	g_theRenderer->DrawAABB2D( AABB2( Vec2( m_posx , m_posy ) , Vec2( m_posx + 10.f , m_posy + 3.f ) ) , Rgba8( 100 , 0 , 0 , 200 ) );
	IntVec2 tileInfo = m_map->GetTileInfoForMousePosition( Vec2( m_posx , m_posy ) );

	std::string tileCoordInString = "";
	tileCoordInString = tileCoordInString + std::to_string( tileInfo.x ) + ',' + std::to_string( tileInfo.y );
	std::string tileInfoInString = m_map->GetTileInfoForTileCoords( tileInfo );
	std::vector<Vertex_PCU> tileCoodInfo;
	std::vector<Vertex_PCU> tileDefInfo;
	m_BitmapFont->AddVertsForTextInBox2D( tileCoodInfo , AABB2( Vec2( m_posx , m_posy ) , Vec2( m_posx + 5.f , m_posy + 5.f ) ) , 1.f , tileCoordInString , Rgba8( 200 , 100 , 100 , 255 ) , 1.f , Vec2( 0.f , 0.f ) );
	m_BitmapFont->AddVertsForTextInBox2D( tileDefInfo , AABB2( Vec2( m_posx , m_posy+1.2f ) , Vec2( m_posx + 5.f , m_posy + 5.f ) ) , .5f , tileInfoInString , Rgba8( 200 , 100 , 100 , 255 ) , 1.f , Vec2( 0.f , 0.f ) );
	g_theRenderer->BindTexture( m_BitmapFont->GetTexture() );
	g_theRenderer->DrawVertexArray( tileCoodInfo );
	g_theRenderer->DrawVertexArray( tileDefInfo );
	g_theRenderer->BindTexture( nullptr );

	if ( m_map->IsMousePointingActor( Vec2( m_posx , m_posy ) ) )
	{
		g_theRenderer->DrawAABB2D( AABB2( Vec2( m_posx , m_posy + 3.f ) , Vec2( m_posx + 10.f , m_posy + 7.f ) ) , Rgba8( 0 , 0 , 100 , 200 ) );
		Strings actorInfo = m_map->GetPlayerDetails();
		std::vector<Vertex_PCU> actorInfoVerts;
		float tempTextStartPos = 0.f;
		for ( int index = 0; index < actorInfo.size(); index++ )
		{
			m_BitmapFont->AddVertsForText2D( actorInfoVerts , Vec2( m_posx , m_posy + 3.f + tempTextStartPos ) , 0.5f , actorInfo[ index ] , Rgba8( 100 , 100 , 200 , 255 ) );
			tempTextStartPos += 1.f;
		}
		g_theRenderer->BindTexture( m_BitmapFont->GetTexture() );
		g_theRenderer->DrawVertexArray( actorInfoVerts );
		g_theRenderer->BindTexture( nullptr );
		
	}

	if ( m_map->IsMousePointingNpcUnits( Vec2( m_posx , m_posy ) ) && !m_map->IsMousePointingActor( Vec2( m_posx , m_posy ) ) )
	{
		g_theRenderer->DrawAABB2D( AABB2( Vec2( m_posx , m_posy + 3.f ) , Vec2( m_posx + 10.f , m_posy + 7.f ) ) , Rgba8( 0 , 0 , 100 , 200 ) );
		Strings npcInfo = m_map->GetNpcDetails( Vec2( m_posx , m_posy ) );
		std::vector<Vertex_PCU> npcInfoVerts;
		float tempTextStartPos = 0.f;
		for ( int index = 0; index < npcInfo.size(); index++ )
		{
			m_BitmapFont->AddVertsForText2D( npcInfoVerts , Vec2( m_posx , m_posy + 3.f + tempTextStartPos ) , 0.5f , npcInfo[ index ] , Rgba8( 100 , 100 , 200 , 255 ) );
			tempTextStartPos += 1.f;
		}
		g_theRenderer->BindTexture( m_BitmapFont->GetTexture() );
		g_theRenderer->DrawVertexArray( npcInfoVerts );
		g_theRenderer->BindTexture( nullptr );
	}


}
































