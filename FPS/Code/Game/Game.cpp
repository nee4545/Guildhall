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
   
	world=new World(this);

	
}

void Game::Update( float deltaseconds )
{
	TogglePaused();
	if( paused )
	{
		deltaseconds=0.f;
	}


	ToggleDebugCamera();
	ToggleDebugging();
	

	world->Update(deltaseconds);

	if(!toggleCamera )
	{ 
	  UpdateCamera();
	}
	
}

void Game::Render()
{
	
	render->BeginCamera(*m_camera);
	
	world->Render();
	render->EndCamera(*m_camera);
	
}



void Game::UpdateCamera()
{
	Vec2 tankPos= world->current_map->m_enitites[0]->GetPosition();
	Vec2 camCoords;

	camCoords.x=Clamp(tankPos.x,8.f,WORLD_SIZE_X-8);
	camCoords.y=Clamp(tankPos.y,4.5f,WORLD_SIZE_Y-4.5f);

	m_camera->SetOrthoView(camCoords-Vec2(8.f,4.5f),camCoords+Vec2(8.f,4.5f));
}

void Game::ToggleDebugCamera()
{
	if(input->WasKeyJustPressed(0x73) )
	{ 
	toggleCamera=!toggleCamera;
	m_camera->SetOrthoView(Vec2(0.f,0.f),Vec2(53.3f,30.f));
	}
}

void Game::TogglePaused()
{
	if( input->WasKeyJustPressed( 'P' ) )
	{
		paused=!paused;
	}
}

void Game::ToggleDebugging()
{
	if( input->WasKeyJustPressed( 0x70 ) )
	{
		isDebugging=!isDebugging;
	}
}

































