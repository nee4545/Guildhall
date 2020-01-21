#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Game/Entity.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/DevConsole.hpp"


DevConsole dev = DevConsole();

Game::Game()
{
	m_camera = new Camera();
	m_camera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 54.f , 30.f ) );
	TileDefinition::CreateTileDefinitions();
	m_map = new Map( this , IntVec2( 54 , 25 ) );
	
}

void Game::Update( float deltaseconds )
{
	m_map->Update( deltaseconds );
}

void Game::Render()
{
	g_theRenderer->BeginCamera( *m_camera );
	m_map->Render();
	
}



void Game::UpdateCamera()
{
	
}

void Game::ToggleDebugCamera()
{

}

void Game::TogglePaused()
{
	
}

void Game::ToggleDebugging()
{
	
}

Vec2 Game::GetMousePositionScreenView()
{
	Vec2 mouseNormalizedPosition = g_theInput->GetCurrentMousePosition();

	Vec2 mousePositionScreen;

	mousePositionScreen.x = RangeMapFloat( 0.f , 1.f , m_camera->GetOrthoBottomLeft().x , m_camera->GetOrthoTopRight().x , mouseNormalizedPosition.x );
	mousePositionScreen.y = RangeMapFloat( 0.f , 1.f , m_camera->GetOrthoBottomLeft().y , m_camera->GetOrthoTopRight().y , mouseNormalizedPosition.y );

	return mousePositionScreen;
}
































