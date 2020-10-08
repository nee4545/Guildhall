#include "Game/MonsterAI.hpp"
#include "Game/Game.hpp"
#include "Game/Player.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

MonsterAI::MonsterAI( Game* game , AI_TYPE type , SpriteAnimDefinition* idleAnims , SpriteAnimDefinition* walkAnims , SpriteAnimDefinition* meleeAttackAnims )
{
	m_game = game;
	m_currentType = type;
	m_animationAtIdle = idleAnims;
	m_animationAtWalk = walkAnims;
	m_animationAtMeleeAttack = meleeAttackAnims;

	m_aspectAtWalk = Vec2( 0.65f , 0.65f );
	m_aspectAtMeleeAttack = Vec2( 1.f , 1.f );
	SetVerticesBasedOnAspect( Vec2( 1.f , 1.f ) );

	m_helper = new MovingHelper();

}

MonsterAI::MonsterAI( Game* game , AI_TYPE type , SpriteAnimDefinition* idleAnims , SpriteAnimDefinition* walkAnims , SpriteAnimDefinition* meleeAttackAnims , SpriteAnimDefinition* rangeAttack1Anims )
{

}

MonsterAI::MonsterAI( Game* game , AI_TYPE type , SpriteAnimDefinition* idleAnims , SpriteAnimDefinition* walkAnims , SpriteAnimDefinition* meleeAttackAnims , SpriteAnimDefinition* rangeAttack1Anims , SpriteAnimDefinition* rangeAttack2Anims )
{

}

MonsterAI::~MonsterAI()
{

}

void MonsterAI::Update( float deltaseconds )
{
	m_time += deltaseconds;
	CheckPlayerInSight();

	if ( m_currentBehavior == WANDER )
	{
		if ( !m_helper->DoesHavePath() )
		{
			FindPathToRandomTile();
			tileIndex = -1;
		}

		else if ( ( m_nextMovePosition - m_position ).GetLengthSquared() <= 0.1f )
		{
			//m_previousPosition = m_position;
			tileIndex = m_helper->GetNextMove();
			if ( tileIndex > 0 )
			{
				m_nextMovePosition = Vec2( m_game->m_tiles[ tileIndex ].m_tileCoords.x + 0.5f , m_game->m_tiles[ tileIndex ].m_tileCoords.y + 0.5f );
			}
			//m_position = m_nextMovePosition;
			moveVec = ( -m_position + m_nextMovePosition ).GetNormalized();
			m_orientationDegrees = moveVec.GetAngleDegrees();
			//m_orientationDegrees = GetTurnedToward( m_orientationDegrees , moveVec.GetAngleDegrees() , 30.f * deltaseconds );
		}
		else
		{
			float deltaXmov = 0.f;
			float deltaYmov = 0.f;

			deltaXmov = moveVec.x * 5.f * deltaseconds;
			deltaYmov = moveVec.y * 5.f * deltaseconds;

			m_position.x += deltaXmov;
			m_position.y += deltaYmov;
		}
	}
	if ( m_currentBehavior == CHASE_PLAYER )
	{
		Vec2 dir = ( m_game->m_player->m_position - m_position ).GetNormalized();
		m_position += dir * 4.f * deltaseconds;
		//m_orientationDegrees = dir.GetAngleDegrees();
		m_orientationDegrees = GetTurnedToward( m_orientationDegrees , dir.GetAngleDegrees() , 90.f * deltaseconds );
	}


}

void MonsterAI::Render()
{
	switch ( m_currentState )
	{
	case IDLE:
	{
		g_theRenderer->BindTexture( nullptr );
		break;
	}
	case MOVING:
	{
		SpriteDefinition def = m_animationAtWalk->GetSpriteDefAtTime( m_time );
		def.GetUVs( m_minUV , m_maxUV );
		g_theRenderer->BindTexture(&def.GetTexture());
		SetVerticesBasedOnAspect( m_aspectAtWalk );
		break;
	}
	case MELEE_ATTACK:
	{
		SpriteDefinition def = m_animationAtMeleeAttack->GetSpriteDefAtTime( m_time );
		def.GetUVs( m_minUV , m_maxUV );
		g_theRenderer->BindTexture( &def.GetTexture() );
		SetVerticesBasedOnAspect( m_aspectAtMeleeAttack );
		break;
	}
	case RANGE_ATTACK_1:
		break;
	case RANGE_ATTACK_2:
		break;
	case DEAD:
		break;
	default:
		break;
	}

	Vertex_PCU vertCopy[ 6 ];
	memcpy( vertCopy , m_vertices , sizeof( Vertex_PCU ) * 6 );

	TransformVertexArray( 6 , vertCopy , 1.f , m_orientationDegrees+90.f , m_position );

	g_theRenderer->DrawVertexArray( 6 , vertCopy );

	DebugRender();

}

void MonsterAI::DebugRender()
{
	std::vector<Vertex_PCU> verts;

	std::vector<int>& pathIndices = m_helper->path;

	for ( int i = 0; i < pathIndices.size(); i++ )
	{
		AABB2 aabb = AABB2( m_game->m_tiles[ pathIndices[ i ] ].m_tileCoords.x , m_game->m_tiles[ pathIndices[ i ] ].m_tileCoords.y , m_game->m_tiles[ pathIndices[ i ] ].m_tileCoords.x + 1 , m_game->m_tiles[ pathIndices[ i ] ].m_tileCoords.y + 1 );
		AppendAABB2( verts , aabb , Rgba8( 0 , 0 , 100 , 100 ) );
	}

	if ( verts.size() > 0 )
	{
		g_theRenderer->BindTexture( nullptr );
		g_theRenderer->DrawVertexArray( verts );
	}

	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->DrawSector( m_position , m_forwardVec.GetRotatedDegrees( m_orientationDegrees+90.f ) , 8.f ,50.f, Rgba8( 100 , 0 , 0 , 100 ) );
}

void MonsterAI::Die()
{
	m_isGarbage = true;
}

void MonsterAI::SetVerticesBasedOnAspect( Vec2 aspect )
{
	m_vertices[ 0 ] = Vertex_PCU( Vec3( -aspect.x , -aspect.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , m_minUV );
	m_vertices[ 1 ] = Vertex_PCU( Vec3( aspect.x , -aspect.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( m_maxUV.x , m_minUV.y ) );
	m_vertices[ 2 ] = Vertex_PCU( Vec3( -aspect.x , aspect.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( m_minUV.x , m_maxUV.y ) );

	m_vertices[ 3 ] = Vertex_PCU( Vec3( aspect.x , -aspect.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( m_maxUV.x , m_minUV.y ) );
	m_vertices[ 4 ] = Vertex_PCU( Vec3( aspect.x , aspect.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , m_maxUV );
	m_vertices[ 5 ] = Vertex_PCU( Vec3( -aspect.x , aspect.y , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( m_minUV.x , m_maxUV.y ) );
}

void MonsterAI::ChangeBehavior( AI_Behavior newBehavior )
{
	if ( newBehavior != m_currentBehavior )
	{
		m_currentBehavior = newBehavior;

		m_helper->Reset();
		m_nextMovePosition = m_position;
	}
}

void MonsterAI::CheckPlayerInSight()
{
	if ( IsPointInForwardSector2D( m_game->m_player->m_position,  m_position , m_forwardVec.GetAngleDegrees()+m_orientationDegrees+90.f , 50.f , 8.f ) )
	{
		ChangeBehavior( CHASE_PLAYER );
	}
	else
	{
		ChangeBehavior( WANDER );
	}
}

void MonsterAI::FindPathToRandomTile()
{
	RandomNumberGenerator &rng=*m_game->m_rng;
	bool validRoll = false;
	int x =0 , y =0;
	while ( !validRoll )
	{
		x = rng.RollRandomIntInRange( 1 , m_game->m_mapSize.x - 1 );
		y = rng.RollRandomIntInRange( 1 , m_game->m_mapSize.y - 1 );

		if ( !(m_game->m_tiles[ m_game->GetTileIndexForTileCoords( IntVec2( x , y ) ) ].m_isSolid ) )
		{
			validRoll = true;
		}
	}

	m_game->GetPathUsingAStarIgnoreDiagonalMoves( m_position , Vec2( x , y ) , m_helper->path );
	m_helper->currentIndex = (int)m_helper->path.size() - 1;
}
