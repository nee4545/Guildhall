#include "Game/AIType3.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Map.hpp"

RandomNumberGenerator airng = RandomNumberGenerator();

AIType3::AIType3( Game* game , Vec2 position ) :Entity( game , position )
{
	m_vertices[ 0 ] = Vertex_PCU( Vec3( -0.4f , -0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 0.f , 0.f ) );
	m_vertices[ 1 ] = Vertex_PCU( Vec3( 0.4f , -0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
	m_vertices[ 2 ] = Vertex_PCU( Vec3( -0.4f , 0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

	m_vertices[ 3 ] = Vertex_PCU( Vec3( 0.4f , -0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
	m_vertices[ 4 ] = Vertex_PCU( Vec3( 0.4f , 0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 1.f , 1.f ) );
	m_vertices[ 5 ] = Vertex_PCU( Vec3( -0.4f , 0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

	m_vertices_original[ 0 ] = Vertex_PCU( Vec3( -0.4f , -0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 0.f , 0.f ) );
	m_vertices_original[ 1 ] = Vertex_PCU( Vec3( 0.4f , -0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
	m_vertices_original[ 2 ] = Vertex_PCU( Vec3( -0.4f , 0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

	m_vertices_original[ 3 ] = Vertex_PCU( Vec3( 0.4f , -0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
	m_vertices_original[ 4 ] = Vertex_PCU( Vec3( 0.4f , 0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 1.f , 1.f ) );
	m_vertices_original[ 5 ] = Vertex_PCU( Vec3( -0.4f , 0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

	m_orientationDegrees = 0.f;
	m_physicsRadius = 0.2f;
	m_cosmeticRadius = 0.4f;

	m_startingPosition = position;
	m_previousPosition = position;
	m_finalDestination = m_destinationPoint1;
}

void AIType3::Update( float deltaSeconds )
{
	if ( !m_game->m_map->IsPlayerAlive() )
	{
		return;
	}

	if ( m_state == STATE_CHASE )
	{
		if ( ( m_previousPosition - m_position ).GetLength() >= 0.5f )
		{
			m_previousPosition = m_position;
			nextMove = GetNextMove();
		}

		if ( nextMove == MOVE_UP )
		{
			MoveUp( deltaSeconds );
		}

		if ( nextMove == MOVE_DOWN )
		{
			MoveDown( deltaSeconds );
		}

		if ( nextMove == MOVE_LEFT )
		{
			MoveLeft( deltaSeconds );
		}

		if ( nextMove == MOVE_RIGHT )
		{
			MoveRight( deltaSeconds );
		}

		if ( RoundDownToInt( m_position.x ) == RoundDownToInt( m_finalDestination.x ) && RoundDownToInt( m_position.y ) == RoundDownToInt( m_finalDestination.y ) )
		{
			SetNewDestination();
		}
	}

	if ( m_state == STATE_FLEE )
	{
		if ( ( m_previousPosition - m_position ).GetLength() >= 0.4f )
		{
			m_previousPosition = m_position;
			nextMove = GetNextMove();
		}

		if ( nextMove == MOVE_UP )
		{
			MoveUp( deltaSeconds * 0.5f );
		}

		if ( nextMove == MOVE_DOWN )
		{
			MoveDown( deltaSeconds * 0.5f );
		}

		if ( nextMove == MOVE_LEFT )
		{
			MoveLeft( deltaSeconds * 0.5f );
		}

		if ( nextMove == MOVE_RIGHT )
		{
			MoveRight( deltaSeconds * 0.5f );
		}

		if ( RoundDownToInt( m_position.x ) == RoundDownToInt( m_finalDestination.x ) && RoundDownToInt( m_position.y ) == RoundDownToInt( m_finalDestination.y ) )
		{
			ChangeState( STATE_CHASE );
		}
	}
}

void AIType3::Render()
{
	if ( m_state == STATE_CHASE )
	{
		g_theRenderer->TransformVertexArray( 6 , m_vertices , 1.f , 0.f , m_position );
		Texture* tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/whale.png" );
		g_theRenderer->BindTexture( tex );
		g_theRenderer->DrawVertexArray( 6 , m_vertices );
		g_theRenderer->BindTexture( nullptr );
		ResetVertices();
	}

	if ( m_state == STATE_FLEE )
	{
		g_theRenderer->TransformVertexArray( 6 , m_vertices , 1.f , 0.f , m_position );
		Texture* tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/pig.png" );
		g_theRenderer->BindTexture( tex );
		g_theRenderer->DrawVertexArray( 6 , m_vertices );
		g_theRenderer->BindTexture( nullptr );
		ResetVertices();
	}
}

void AIType3::Die()
{
	m_isGarbage = true;
}

void AIType3::MoveUp( float deltaSeconds )
{
	m_position.y += AI_TYPE3_SPEED * deltaSeconds;
}

void AIType3::MoveDown( float deltaSeconds )
{
	m_position.y -= AI_TYPE3_SPEED * deltaSeconds;
}

void AIType3::MoveLeft( float deltaSeconds )
{
	m_position.x -= AI_TYPE3_SPEED * deltaSeconds;
}

void AIType3::MoveRight( float deltaSeconds )
{
	m_position.x += AI_TYPE3_SPEED * deltaSeconds;
}



void AIType3::ChangeState( States newState )
{
	m_state = newState;

	if ( m_state == STATE_CHASE )
	{
		m_finalDestination = m_destinationPoint1;
	}
	else
	{
		m_finalDestination = m_startingPosition;
	}
}

void AIType3::SetNewDestination()
{
	int randomNumber = airng.GetRandomIntInRange( 0 , 1 );
	if ( randomNumber == 0 )
	{
		m_finalDestination = m_destinationPoint1;
	}
	else
	{
		m_finalDestination = m_destinationPoint2;
	}

	m_state = STATE_CHASE;
}

bool AIType3::ValidateMove( Moves move )
{
	if ( move == MOVE_UP )
	{
		if ( m_game->m_map->IsTileSolid( IntVec2( RoundDownToInt( m_position.x ) , RoundDownToInt( m_position.y + 1 ) ) ) )
		{
			return false;
		}
	}

	if ( move == MOVE_DOWN )
	{
		if ( m_game->m_map->IsTileSolid( IntVec2( RoundDownToInt( m_position.x ) , RoundDownToInt( m_position.y - 1 ) ) ) )
		{
			return false;
		}
	}

	if ( move == MOVE_LEFT )
	{
		if ( m_game->m_map->IsTileSolid( IntVec2( RoundDownToInt( m_position.x - 1 ) , RoundDownToInt( m_position.y ) ) ) )
		{
			return false;
		}
	}

	if ( move == MOVE_RIGHT )
	{
		if ( m_game->m_map->IsTileSolid( IntVec2( RoundDownToInt( m_position.x + 1 ) , RoundDownToInt( m_position.y ) ) ) )
		{
			return false;
		}
	}

	return true;
}

Moves AIType3::GetNextMove()
{
	
	int finalDestinationIndex = m_game->m_map->GetTileIndexForTileCoords( IntVec2( RoundDownToInt( m_finalDestination.x ) , RoundDownToInt( m_finalDestination.y ) ) );
	int* distanceFields = m_game->m_map->m_distanceFields[ finalDestinationIndex ];

	IntVec2 upTile = IntVec2( RoundDownToInt( m_position.x ) , RoundDownToInt( m_position.y ) + 1 );
	IntVec2 downTile = IntVec2( RoundDownToInt( m_position.x ) , RoundDownToInt( m_position.y ) - 1 );
	IntVec2 rightTile = IntVec2( RoundDownToInt( m_position.x ) + 1 , RoundDownToInt( m_position.y ) );
	IntVec2 leftTile = IntVec2( RoundDownToInt( m_position.x ) - 1 , RoundDownToInt( m_position.y ) );

	int upTileIndex = m_game->m_map->GetTileIndexForTileCoords( upTile );
	int downTileIndex = m_game->m_map->GetTileIndexForTileCoords( downTile );
	int leftTileIndex = m_game->m_map->GetTileIndexForTileCoords( leftTile );
	int rightTileIndex = m_game->m_map->GetTileIndexForTileCoords( rightTile );

	int shortest = distanceFields[ upTileIndex ];
	Moves toReturn = MOVE_UP;

	if ( distanceFields[ downTileIndex ] < shortest )
	{
		shortest = distanceFields[ downTileIndex ];
		toReturn = MOVE_DOWN;
	}

	if ( distanceFields[ leftTileIndex ] < shortest )
	{
		shortest = distanceFields[ leftTileIndex ];
		toReturn = MOVE_LEFT;
	}

	if ( distanceFields[ rightTileIndex ] < shortest )
	{
		shortest = distanceFields[ rightTileIndex ];
		toReturn = MOVE_RIGHT;
	}

	return toReturn;
}


void AIType3::ResetVertices()
{
	for ( int index = 0; index < 6; index++ )
	{
		m_vertices[ index ] = m_vertices_original[ index ];
	}
}
