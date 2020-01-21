#include "Game/AIType2.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Map.hpp"

RandomNumberGenerator rng = RandomNumberGenerator();

AIType2::AIType2( Game* game , Vec2 position ):Entity(game,position)
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
	m_finalDestination = position;
}

void AIType2::Update( float deltaSeconds )
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
			nextMove = GetNextMoveRandom();
		}

		if ( nextMove == MOVE_UP  )
		{
			MoveUp( deltaSeconds );
		}

		if ( nextMove == MOVE_DOWN  )
		{
			MoveDown( deltaSeconds );
		}

		if ( nextMove == MOVE_LEFT  )
		{
			MoveLeft( deltaSeconds );
		}

		if ( nextMove == MOVE_RIGHT )
		{
			MoveRight( deltaSeconds );
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

void AIType2::Render()
{
	if ( m_state == STATE_CHASE )
	{
		g_theRenderer->TransformVertexArray( 6 , m_vertices , 1.f , 0.f , m_position );
		Texture* tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/frog.png" );
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

void AIType2::Die()
{
	m_isGarbage = true;
}

void AIType2::MoveUp( float deltaSeconds )
{
	m_position.y += AI_TYPE2_SPEED * deltaSeconds;
}

void AIType2::MoveDown( float deltaSeconds )
{
	m_position.y -= AI_TYPE2_SPEED * deltaSeconds;
}

void AIType2::MoveLeft( float deltaSeconds )
{
	m_position.x -= AI_TYPE2_SPEED * deltaSeconds;
}

void AIType2::MoveRight( float deltaSeconds )
{
	m_position.x += AI_TYPE2_SPEED * deltaSeconds;
}



void AIType2::ChangeState( States newState )
{
	m_state = newState;
}

bool AIType2::ValidateMove( Moves move )
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
		if ( m_game->m_map->IsTileSolid( IntVec2( RoundDownToInt( m_position.x-1 ) , RoundDownToInt( m_position.y ) ) ) )
		{
			return false;
		}
	}

	if ( move == MOVE_RIGHT )
	{
		if ( m_game->m_map->IsTileSolid( IntVec2( RoundDownToInt( m_position.x+1 ) , RoundDownToInt( m_position.y ) ) ) )
		{
			return false;
		}
	}

	return true;
}

Moves AIType2::GetNextMove()
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

Moves AIType2::GetNextMoveRandom()
{
	if ( RoundDownToInt( m_position.x ) == RoundDownToInt( m_game->m_map->GetPlayerPosition().x ) )
	{
		if ( m_position.y < m_game->m_map->GetPlayerPosition().y )
		{
			if ( ValidateMove( MOVE_UP ) )
			{
				return MOVE_UP;
			}
		}
		else
		{
			if ( ValidateMove( MOVE_DOWN ) )
			{
				return MOVE_DOWN;
			}
			 
		}
	}

	if ( RoundDownToInt( m_position.y ) == RoundDownToInt( m_game->m_map->GetPlayerPosition().y ) )
	{
		if ( m_position.x < m_game->m_map->GetPlayerPosition().x )
		{
			if ( ValidateMove( MOVE_RIGHT ) )
			{
				return MOVE_RIGHT;
			}
		}
		else
		{
			if ( ValidateMove( MOVE_LEFT ) )
			{
				return MOVE_LEFT;
			}

		}
	}
	
	int randNum = rng.RollRandomIntInRange( 1 , 11 );

	if ( randNum <= 10 )
	{
		if ( ValidateMove( nextMove ) )
		{
			return nextMove;
		}
	}
	
	if ( nextMove == MOVE_UP || nextMove == MOVE_DOWN )
	{

		int ramdomNumber =rng.RollRandomIntInRange( 0 , 1 );

		if ( ramdomNumber == 0 )
		{
			bool validation = ValidateMove( MOVE_LEFT );
			if ( validation )
			{
				return MOVE_LEFT;
			}
			else if ( ValidateMove( MOVE_RIGHT ) )
			{
				return MOVE_RIGHT;
			}
		}
		
		if ( ramdomNumber == 1 )
		{
			bool validation = ValidateMove( MOVE_RIGHT );
			if ( validation )
			{
				return MOVE_RIGHT;
			}
			else if ( ValidateMove( MOVE_LEFT ) )
			{
				return MOVE_LEFT;
			}
		}

		if ( nextMove == MOVE_UP )
		{
			return MOVE_DOWN;
		}
		else
		{
			return MOVE_UP;
		}

	}

	if ( nextMove == MOVE_LEFT || nextMove == MOVE_RIGHT )
	{

		int ramdomNumber = rng.RollRandomIntInRange( 0 , 1 );

		if ( ramdomNumber == 0 )
		{
			bool validation = ValidateMove( MOVE_UP );
			if ( validation )
			{
				return MOVE_UP;
			}
			else if ( ValidateMove( MOVE_DOWN ) )
			{
				return MOVE_DOWN;
			}
		}

		if ( ramdomNumber == 1 )
		{
			bool validation = ValidateMove( MOVE_DOWN );
			if ( validation )
			{
				return MOVE_DOWN;
			}
			else if ( ValidateMove( MOVE_UP ) )
			{
				return MOVE_UP;
			}
		}

		if ( nextMove == MOVE_LEFT )
		{
			return MOVE_RIGHT;
		}
		else
		{
			return MOVE_LEFT;
		}

	}

	return nextMove;
}

void AIType2::ResetVertices()
{
	for ( int index = 0; index < 6; index++ )
	{
		m_vertices[ index ] = m_vertices_original[ index ];
	}
}
