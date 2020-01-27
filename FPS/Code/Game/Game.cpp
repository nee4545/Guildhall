#include "Game/Game.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Game/World.hpp"
#include "Game/Entity.hpp"




Game::Game()
{
	rng= RandomNumberGenerator();
	m_camera=new Camera();
	m_camera->SetOrthoView(Vec2(0.f,0.f),Vec2(53.33f,30.f));
   
	//world=new World(this);

	
}

void Game::Update( float deltaseconds )
{
	
	m_camera->SetClearMode( CLEAR_COLOR_BIT , Rgba8( 100 , 0 , 0 , 255 ) , 0.f , 0 );
}

void Game::Render()
{
	
	render->BeginCamera(*m_camera);
	
	//world->Render();
	render->EndCamera(*m_camera);
	
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

































