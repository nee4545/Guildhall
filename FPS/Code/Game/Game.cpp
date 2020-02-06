#include "Game/Game.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Game/World.hpp"
#include "Game/Entity.hpp"
#define UNUSED(x) (void)(x);




Game::Game()
{
	rng= RandomNumberGenerator();
	m_camera=new Camera();
	m_camera->SetOrthoView(Vec2(0.f,0.f),Vec2(53.33f,30.f));
   
	
	
}

void Game::Update( float deltaseconds )
{

	UNUSED( deltaseconds );
	
	m_rColorValue = 1.f;


	m_camera->SetClearMode( CLEAR_COLOR_BIT , Rgba8( (unsigned char)(100 * m_rColorValue) , 0 , 0 , 0 ) , 0.f , 0 );

	
}

void Game::Render()
{
	render->BeginCamera(*m_camera);
	
	//render->Draw( 3 , 0 );
	//render->DrawVertexArray( 1 , temp );
	render->DrawAABB2D( AABB2( 0.5f , 0.5f , -0.5f , -0.5f ) , Rgba8( 255 , 255 , 255 , 255 ) );
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

































