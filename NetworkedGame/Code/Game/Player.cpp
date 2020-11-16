#include "Game/Player.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Game.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Game/SinglePlayerGame.hpp"
#include "Game/MultiplayerGame.hpp"

Player::Player( Game* game, Vec2 position, Faction faction ):Entity(game,position)
{
	m_vertices[0]=Vertex_PCU(Vec3(-0.4f,-0.4f,0.f),Rgba8(255,255,255),Vec2(0.f,0.f));
	m_vertices[1]=Vertex_PCU(Vec3(0.4f,-0.4f,0.f),Rgba8(255,255,255),Vec2(1.f,0.f));
	m_vertices[2]=Vertex_PCU(Vec3(-0.4f,0.4f,0.f),Rgba8(255,255,255),Vec2(0.f,1.f));

	m_vertices[3]=Vertex_PCU(Vec3(0.4f,-0.4f,0.f),Rgba8(255,255,255),Vec2(1.f,0.f));
	m_vertices[4]=Vertex_PCU(Vec3(0.4f,0.4f,0.f),Rgba8(255,255,255),Vec2(1.f,1.f));
	m_vertices[5]=Vertex_PCU(Vec3(-0.4f,0.4f,0.f),Rgba8(255,255,255),Vec2(0.f,1.f));
	m_faction = faction;

	m_sprite = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/hippo.png" );
	m_physicsRadius = 1.f;
}

void Player::Update( float deltaSeconds )
{
	m_hasMovedLastFrame = m_hasMovedThisFrame;
	m_hasMovedThisFrame = false;

	SinglePlayerGame* game = ( SinglePlayerGame* ) m_game;
	InputSystem* sys = m_game->GetInputSystem();

	if ( !game->isSinglePlayer )
	{
		MultiplayerGame* game = ( MultiplayerGame* ) m_game;
		if ( m_ID == 1 )
		{
			sys = game->m_player1Input;
		}
	}
	
	if ( sys == nullptr )
	{
		return;
	}
	
	MoveOnKeyPress( sys , deltaSeconds );

	if ( sys->WasLeftMouseButtonJustPressed() )
	{
		if ( !game->isSinglePlayer )
		{
			MultiplayerGame* game = ( MultiplayerGame* ) m_game;
			Vec2 forwardVec = ( game->m_mousePosition - m_position ).GetNormalized();
			game->SpawnBullet( m_position , forwardVec , FACTION_GOOD );
		}
	}

	m_position.x = Clamp( m_position.x , 0.f , 80.f );
	m_position.y = Clamp( m_position.y , 0.f , 45.f );

}

void Player::MoveOnKeyPress( InputSystem* sys , float deltaSeconds )
{
	
	if ( sys->IsKeyPressed( 'W' ) )
	{
		m_position.y += 5.f * deltaSeconds;
		m_hasMovedThisFrame = true;
	}

	if ( sys->IsKeyPressed( 'S' ) )
	{
		m_position.y -= 5.f * deltaSeconds;
		m_hasMovedThisFrame = true;
	}

	if ( sys->IsKeyPressed( 'A' ) )
	{
		m_position.x -= 5.f * deltaSeconds;
		m_hasMovedThisFrame = true;
	}

	if ( sys->IsKeyPressed( 'D' ) )
	{
		m_position.x += 5.f * deltaSeconds;
		m_hasMovedThisFrame = true;
	}

}

void Player::MoveUp()
{
	m_position.y += 5.f * 0.016f;
	m_hasMovedThisFrame = true;
}

void Player::MoveDown()
{
	m_position.y -= 5.f * 0.016f;
	m_hasMovedThisFrame = true;
}

void Player::MoveLeft()
{
	m_position.x -= 5.f * 0.016f;
	m_hasMovedThisFrame = true;
}

void Player::MoveRight()
{
	m_position.x += 5.f * 0.016f;
	m_hasMovedThisFrame = true;
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

void Player::SetSpriteBasedOnID()
{
	if ( m_ID == 1 )
	{
		m_sprite = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/hippo.png" );
	}
	else
	{
		m_sprite = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/dog.png" );
	}
}

