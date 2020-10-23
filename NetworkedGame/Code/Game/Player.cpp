#include "Game/Player.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Game.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Game/SinglePlayerGame.hpp"

Player::Player( Game* game, Vec2 position ):Entity(game,position)
{
	m_vertices[0]=Vertex_PCU(Vec3(-0.4f,-0.4f,0.f),Rgba8(255,255,255),Vec2(0.f,0.f));
	m_vertices[1]=Vertex_PCU(Vec3(0.4f,-0.4f,0.f),Rgba8(255,255,255),Vec2(1.f,0.f));
	m_vertices[2]=Vertex_PCU(Vec3(-0.4f,0.4f,0.f),Rgba8(255,255,255),Vec2(0.f,1.f));

	m_vertices[3]=Vertex_PCU(Vec3(0.4f,-0.4f,0.f),Rgba8(255,255,255),Vec2(1.f,0.f));
	m_vertices[4]=Vertex_PCU(Vec3(0.4f,0.4f,0.f),Rgba8(255,255,255),Vec2(1.f,1.f));
	m_vertices[5]=Vertex_PCU(Vec3(-0.4f,0.4f,0.f),Rgba8(255,255,255),Vec2(0.f,1.f));

	m_sprite = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/hippo.png" );
	m_physicsRadius = 1.f;
}

void Player::Update( float deltaSeconds )
{
	SinglePlayerGame* game = ( SinglePlayerGame* ) m_game;
	InputSystem* sys = m_game->GetInputSystem();
	
	if ( sys == nullptr )
	{
		return;
	}
	
	if ( sys->IsKeyPressed( 'W' ) )
	{
		m_position.y += 5.f * deltaSeconds;
	}

	if ( sys->IsKeyPressed( 'S' ) )
	{
		m_position.y -= 5.f * deltaSeconds;
	}

	if ( sys->IsKeyPressed( 'A' ) )
	{
		m_position.x -= 5.f * deltaSeconds;
	}

	if ( sys->IsKeyPressed( 'D' ) )
	{
		m_position.x += 5.f * deltaSeconds;
	}

	if ( sys->WasLeftMouseButtonJustPressed() )
	{
		Vec2 forwardVec = ( game->m_mousePosition - m_position ).GetNormalized();
		game->SpawnBullet( m_position , forwardVec , FACTION_GOOD );
	}

	m_position.x = Clamp( m_position.x , 0.f , 80.f );
	m_position.y = Clamp( m_position.y , 0.f , 45.f );

}

void Player::Render()
{
	g_theRenderer->BindTexture( m_sprite );
	
	Vertex_PCU vertCopy[ 6 ];
	memcpy( vertCopy , m_vertices , sizeof( Vertex_PCU ) * 6 );

	TransformVertexArray( 6 , vertCopy , 2.5f , 0.f , m_position );

	g_theRenderer->DrawVertexArray( 6 , vertCopy );
	g_theRenderer->BindTexture( nullptr );

	//g_theRenderer->DrawRing( m_position , m_physicsRadius , Rgba8() , 0.1f );
}

void Player::Die()
{
	m_isGarbage = true;
}

void Player::LoadTextures()
{
	
}

