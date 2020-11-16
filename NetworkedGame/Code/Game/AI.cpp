#include "Game/AI.hpp"
#include "Game/SinglePlayerGame.hpp"
#include "Engine/Core/Timer.hpp"
#include "Game/Player.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/MultiplayerGame.hpp"

AI::AI( Game* game , Vec2 position , AiType type ):Entity(game,position)
{

	m_vertices[ 0 ] = Vertex_PCU( Vec3( -0.4f , -0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 0.f , 0.f ) );
	m_vertices[ 1 ] = Vertex_PCU( Vec3( 0.4f , -0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
	m_vertices[ 2 ] = Vertex_PCU( Vec3( -0.4f , 0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

	m_vertices[ 3 ] = Vertex_PCU( Vec3( 0.4f , -0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
	m_vertices[ 4 ] = Vertex_PCU( Vec3( 0.4f , 0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 1.f , 1.f ) );
	m_vertices[ 5 ] = Vertex_PCU( Vec3( -0.4f , 0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

	m_moveTimer = new Timer();
	m_shootTimer = new Timer();
	m_moveTimer->SetSeconds( 1.f );
	m_shootTimer->SetSeconds( 3.f );
	m_type = type;
	m_physicsRadius = 1.f;

	if ( m_type == TYPE_1 )
	{
		m_sprite = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/dog.png" );
	}
	else
	{
		m_sprite = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/rhino.png" );
	}
}

AI::~AI()
{

}

void AI::Update( float deltaseconds )
{
	if ( m_game->isSinglePlayer )
	{
		SinglePlayerGame* game = ( SinglePlayerGame* ) m_game;

		if ( m_type == TYPE_1 )
		{
			if ( m_moveTimer->HasElapsed() )
			{
				float xPos = ( float ) game->m_rng->RollRandomIntInRange( 10 , 70 );
				float yPos = ( float ) game->m_rng->RollRandomIntInRange( 10 , 40 );

				m_nextMovePosition = Vec2( xPos , yPos );
				m_moveTimer->Reset();
			}

			Vec2 moveVec = ( -m_position + m_nextMovePosition ).GetNormalized();
			m_position += moveVec * 3.f * deltaseconds;

			if ( m_shootTimer->HasElapsed() )
			{
				game->SpawnBullet( m_position , ( -m_position + game->m_player->m_position ).GetNormalized() , FACTION_BAD );
				m_shootTimer->Reset();
			}
		}

		if ( m_type == TYPE_2 )
		{
			m_nextMovePosition = game->m_player->m_position;

			Vec2 moveVec = ( -m_position + m_nextMovePosition ).GetNormalized();
			m_position += moveVec * 3.f * deltaseconds;
		}

		m_position.x = Clamp( m_position.x , 10.f , 75.f );
		m_position.y = Clamp( m_position.y , 10.f , 40.f );
	}
	else
	{
		MultiplayerGame* game = ( MultiplayerGame* ) m_game;

		if ( m_faction == FACTION_GOOD )
		{
			m_nextMovePosition = game->m_player2->m_position;

			Vec2 moveVec = ( m_nextMovePosition - m_position ).GetNormalized();
			m_position += moveVec * 1.2f * deltaseconds;
		}

		if ( m_faction == FACTION_BAD )
		{
			m_nextMovePosition = game->m_player1->m_position;

			Vec2 moveVec = ( m_nextMovePosition - m_position ).GetNormalized();
			m_position += moveVec * 1.2f * deltaseconds;
		}
	}

}

void AI::Render()
{
	g_theRenderer->BindTexture( m_sprite );

	Vertex_PCU vertCopy[ 6 ];
	memcpy( vertCopy , m_vertices , sizeof( Vertex_PCU ) * 6 );

	TransformVertexArray( 6 , vertCopy , 2.5f , 0.f , m_position );

	g_theRenderer->DrawVertexArray( 6 , vertCopy );
	g_theRenderer->BindTexture( nullptr );
}

void AI::Die()
{
	m_isGarbage = true;
}

void AI::SetSpriteBasedOnFaction()
{
	if ( m_faction == FACTION_GOOD )
	{
		m_sprite = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/narwhal.png" );
	}
	else
	{
		m_sprite = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/rhino.png" );
	}
}
