#include "Game/Game.hpp"
#include "Game/GameObject.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/SimpleTriangleFont.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/RigidBody2D.hpp"


Game::Game()
{
	m_camera = new Camera();
	m_camera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 200.f , 112.5f ));

}

void Game::StartUp()
{
	m_physicsSystem = new Physics2D();

}

GameObject* Game::CreateDisc()
{
	GameObject* obj = new GameObject();
	return nullptr;
}

void Game::Update( float deltaseconds )
{
	UNUSED( deltaseconds );
}

void Game::Render()
{
	g_theRenderer->BeginCamera( *m_camera );
	
	g_theRenderer->EndCamera( *m_camera );

	
}




































