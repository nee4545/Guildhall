#include "Game/MonsterAI.hpp"
#include "Game/Game.hpp"
#include "Game/Player.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GreenBeret.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Time.hpp"
#include "Game/OccupancyMap.hpp"

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

	m_rotation1Timer = new Timer();
	m_rotation1Timer->SetSeconds( 0.25f );

	m_sectorRotationTimer = new Timer();
	m_sectorRotationTimer->SetSeconds( 3.5f );

	m_occMovetimer = new Timer();
	m_occMovetimer->SetSeconds( 0.6f );

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

	ChangeBehavior();

	if ( m_currentBehavior == PATROL )
	{
		if ( !m_hasPatrolPoint )
		{
			m_nextMovePosition = GetPatrolPoint();
			m_hasPatrolPoint = true;
			m_currentPatrolIndex = ( m_currentPatrolIndex + 1 ) % m_patrolPoints.size();
			float distanceToPoint = ( m_position - m_nextMovePosition ).GetLength();
			Vec2 direction = ( m_nextMovePosition - m_position ).GetNormalized();

			if ( !m_game->RayCast( m_position , direction , distanceToPoint ).didImpact )
			{
				m_moveWithoutPathfinding = true;
			}

		}

		if ( m_moveWithoutPathfinding )
		{
			Vec2 moveVec = ( m_nextMovePosition - m_position ).GetNormalized();
			m_orientationDegrees = moveVec.GetAngleDegrees();
			m_position += moveVec * 3.f * deltaseconds;

			if ( ( m_nextMovePosition - m_position ).GetLengthSquared() <= 0.1f )
			{
				m_hasPatrolPoint = false;
			}
		}
		else if ( !m_helper->DoesHavePath() )
		{
			m_game->GetPathInGame( m_position , m_nextMovePosition , m_helper->path );
		}
		else if ( ( m_nextMovePosition - m_position ).GetLengthSquared() <= 0.1f )
		{
			tileIndex = m_helper->GetNextMove();
			if ( tileIndex > 0 )
			{
				m_nextMovePosition = Vec2( m_game->m_mainMapTiles[ tileIndex ].m_tileCoords.x + 0.5f , m_game->m_mainMapTiles[ tileIndex ].m_tileCoords.y + 0.5f );
			}
			if ( m_helper->currentIndex == 0 )
			{
				m_hasPatrolPoint = false;
				m_helper->Reset();
			}
			moveVec = ( m_nextMovePosition - m_position ).GetNormalized();
		}
		else
		{
			moveVec = ( m_nextMovePosition - m_position ).GetNormalized();
			m_orientationDegrees = moveVec.GetAngleDegrees();
			m_position += moveVec * 3.f * deltaseconds;
		}
	}

	


	m_time += deltaseconds;
	//CheckPlayerInSight();

	if ( m_currentBehavior == CHASE_PLAYER )
	{
		Vec2 dir = ( m_game->m_greenBeret->m_position - m_position ).GetNormalized();
		m_position += dir * 4.5f * deltaseconds;

		m_helper->path.clear();
		m_helper->Reset();
		m_lastSeenPosition = m_game->m_greenBeret->m_position;
		m_lastSeenSet = true;

		if ( m_occMap != nullptr )
		{
			delete m_occMap;
			m_occMap = nullptr;
		}

		IntVec2 posInInt = IntVec2( RoundDownToInt( m_position.x ) , RoundDownToInt( m_position.y ) );

		if ( m_game->m_mainMapTiles[ m_game->GetTileIndexForTileCoords( posInInt , true ) ].m_doesHaveDirection && !m_game->m_mainMapTiles[ m_game->GetTileIndexForTileCoords( posInInt , true ) ].m_isDirectionPositive )
		{
			//dir+= m_game->m_mainMapTiles[ m_game->GetTileIndexForTileCoords( posInInt , true ) ].m_direction *

			m_position += Vec2::MakeFromPolarDegrees( m_game->m_mainMapTiles[ m_game->GetTileIndexForTileCoords( posInInt , true ) ].m_direction ) * m_game->m_mainMapTiles[ m_game->GetTileIndexForTileCoords( posInInt , true ) ].m_directionIntensity * deltaseconds;
		}

		m_orientationDegrees = GetTurnedToward( m_orientationDegrees , dir.GetAngleDegrees() , 90.f * deltaseconds );
	
	}

	if ( m_currentBehavior == SHOOT_PLAYER )
	{
		Vec2 dir = ( m_game->m_greenBeret->m_position - m_position ).GetNormalized();
		m_orientationDegrees = GetTurnedToward( m_orientationDegrees , dir.GetAngleDegrees() , 90.f * deltaseconds );
	
	}

	//return;
	
	if ( m_currentBehavior == SEARCH_OCCUPANCY_MAP )
	{
		if ( m_lastSeenSet )
		{
			m_helper->path.clear();
			m_helper->Reset();
			Vec2 dir = ( m_lastSeenPosition - m_position ).GetNormalized();
			m_position += dir * 4.f * deltaseconds;

			m_orientationDegrees = GetTurnedToward( m_orientationDegrees , dir.GetAngleDegrees() , 90.f * deltaseconds );

			if ( ( m_position - m_lastSeenPosition ).GetLengthSquared() <= 0.5f )
			{
				m_lastSeenSet = false;
			}

			if ( m_occMap == nullptr )
			{
				m_occMap = new OccupancyMap( m_game , IntVec2( ( int ) m_lastSeenPosition.x , ( int ) m_lastSeenPosition.y ) , IntVec2( 59 , 59 ) , 100.f );

				for ( int i = 0; i < 5; i++ )
				{
					m_occMap->PropgateInfluence();
				}
			}

			if ( m_rotation1Timer->HasElapsed() )
			{
				m_rotation1Timer->Reset();
				m_occMap->PropgateInfluence();
			}


			for ( int i = 0; i < m_occMap->m_nodes.size(); i++ )
			{
				IntVec2 coords = m_occMap->m_nodes[ i ].coords;
				if ( IsPointInForwardSector2D( Vec2( coords.x , coords.y ) , m_position , m_forwardVec.GetAngleDegrees() + m_orientationDegrees + 90.f , 30.f , 100.f ) && !m_game->RayCast( m_position , ( Vec2( coords.x+0.5f , coords.y+0.5f ) - m_position ).GetNormalized() , ( m_position - Vec2( coords.x+0.5f , coords.y+0.5f ) ).GetLength() ).didImpact )
				{
					m_occMap->SetValue( coords , 0.f );
				}
			}

			/*for ( int i = 0; i < m_occMap->m_nodes.size(); i++ )
			{
				if ( IsPointInsideDisc2D( Vec2( m_occMap->m_nodes[ i ].coords.x , m_occMap->m_nodes[ i ].coords.y ) , m_position , 10.f ) && !m_game->RayCast( m_position , ( Vec2( m_occMap->m_nodes[ i ].coords.x , m_occMap->m_nodes[ i ].coords.y ) - m_position ).GetNormalized() , ( m_position - Vec2( m_occMap->m_nodes[ i ].coords.x , m_occMap->m_nodes[ i ].coords.y ) ).GetLength() ).didImpact )
				{
					m_occMap->SetValue( m_occMap->m_nodes[ i ].coords , 0.f );
				}
			}*/

			m_occMap->SetValue( IntVec2( ( int ) m_position.x , ( int ) m_position.y ) , 0.f );
		}
		else if ( m_occMap != nullptr )
		{
			if ( m_rotation1Timer->HasElapsed() )
			{
				m_occMap->PropgateInfluence();
				m_rotation1Timer->Reset();
			}

			for ( int i = 0; i < m_occMap->m_nodes.size(); i++ )
			{
				IntVec2 coords = m_occMap->m_nodes[ i ].coords;
				if ( IsPointInForwardSector2D( Vec2( coords.x , coords.y ) , m_position , m_forwardVec.GetAngleDegrees() + m_orientationDegrees + 90.f , 30.f , 100.f ) && !m_game->RayCast( m_position , ( Vec2( coords.x , coords.y ) - m_position ).GetNormalized() , ( m_position - Vec2( coords.x , coords.y ) ).GetLength() ).didImpact )
				{
					m_occMap->SetValue( coords , 0.f );
				}
			}

			for ( int i = 0; i < m_occMap->m_nodes.size(); i++ )
			{
				if ( IsPointInsideDisc2D( Vec2( m_occMap->m_nodes[ i ].coords.x , m_occMap->m_nodes[ i ].coords.y ) , m_position , 10.f ) && !m_game->RayCast( m_position , ( Vec2( m_occMap->m_nodes[ i ].coords.x + 0.5f , m_occMap->m_nodes[ i ].coords.y + 0.5f ) - m_position ).GetNormalized() , ( m_position - Vec2( m_occMap->m_nodes[ i ].coords.x , m_occMap->m_nodes[ i ].coords.y ) ).GetLength() ).didImpact )
				{
					m_occMap->SetValue( m_occMap->m_nodes[ i ].coords , 0.f );
				}
			}

			m_occMap->SetValue( IntVec2( ( int ) m_position.x , ( int ) m_position.y ) , 0.f );

			
			if ( m_occMovetimer->HasElapsed() )
			{
				if ( m_helper->DoesHavePath() )
				{
					if ( m_occMap->GetMaxValueCoords() != m_currentMoveCoords )
					{
						m_helper->Reset();
					}
				}

				m_occMovetimer->Reset();
			}
			

			if ( !m_helper->DoesHavePath() )
			{
				m_nextMovePosition = m_position;
				IntVec2 maxCoords = m_occMap->GetMaxValueCoords();
				m_currentMoveCoords = maxCoords;
				m_game->GetPathInGame( m_position , Vec2( maxCoords.x , maxCoords.y ) , m_helper->path );
				m_helper->currentIndex = m_helper->path.size() - 1;
			}
			else if ( ( m_nextMovePosition - m_position ).GetLengthSquared() <= 0.1f )
			{
				tileIndex = m_helper->GetNextMove();
				if ( tileIndex > 0 )
				{
					m_nextMovePosition = Vec2( m_game->m_mainMapTiles[ tileIndex ].m_tileCoords.x + 0.5f , m_game->m_mainMapTiles[ tileIndex ].m_tileCoords.y + 0.5f );
				}

				
			}
			else
			{
				moveVec = ( m_nextMovePosition - m_position ).GetNormalized();
				m_orientationDegrees = moveVec.GetAngleDegrees();
				m_position += moveVec * 6.f * deltaseconds;

				IntVec2 maxCoords = m_occMap->GetMaxValueCoords();
				if ( m_currentMoveCoords != maxCoords )
				{
					m_helper->Reset();
					tileIndex = -1;
				}

			}
		}
			
	}
	//if ( IsPointInForwardSector2D( m_game->m_greenBeret->m_position , m_position , m_forwardVec.GetAngleDegrees() + m_orientationDegrees + 90.f , 30.f , 100.f ) && !m_game->RayCast( m_position , -( m_position - m_game->m_greenBeret->m_position ).GetNormalized() , ( m_position - m_game->m_greenBeret->m_position ).GetLength() ).didImpact )
	//{
	//	m_helper->path.clear();
	//	m_helper->Reset();
	//	m_lastSeenPosition = m_game->m_greenBeret->m_position;
	//	m_lastSeenSet = true;

	//	if ( m_occupancyMap != nullptr )
	//	{
	//		delete m_occupancyMap;
	//		m_occupancyMap = nullptr;
	//	}

	//	Vec2 dir = ( m_game->m_greenBeret->m_position - m_position ).GetNormalized();
	//	m_position += dir * 4.f * deltaseconds;

	//	m_orientationDegrees = GetTurnedToward( m_orientationDegrees , dir.GetAngleDegrees() , 90.f * deltaseconds );
	//}
	//else if ( m_lastSeenSet )
	//{
	//	m_helper->path.clear();
	//	m_helper->Reset();
	//	Vec2 dir = ( m_lastSeenPosition - m_position ).GetNormalized();
	//	m_position += dir * 4.f * deltaseconds;

	//	m_orientationDegrees = GetTurnedToward( m_orientationDegrees , dir.GetAngleDegrees() , 90.f * deltaseconds );

	//	if ( ( m_position - m_lastSeenPosition ).GetLengthSquared() <= 0.5f )
	//	{
	//		m_lastSeenSet = false;
	//	}

	//	if ( m_occMap == nullptr )
	//	{
	//		m_occMap = new OccupancyMap(m_game, IntVec2( ( int ) m_lastSeenPosition.x , ( int ) m_lastSeenPosition.y ) , IntVec2( 59 , 59 ) , 100.f );

	//		for ( int i = 0; i < 5; i++ )
	//		{
	//			m_occMap->PropgateInfluence();
	//		}
	//	}

	//	if ( m_rotation1Timer->HasElapsed() )
	//	{
	//		m_rotation1Timer->Reset();
	//		m_occMap->PropgateInfluence();
	//	}
	//	

	//	for ( int i = 0; i < m_occMap->m_nodes.size(); i++ )
	//	{
	//		IntVec2 coords = m_occMap->m_nodes[ i ].coords;
	//		if ( IsPointInForwardSector2D( Vec2( coords.x , coords.y ) , m_position , m_forwardVec.GetAngleDegrees() + m_orientationDegrees + 90.f , 30.f , 100.f ) && !m_game->RayCast( m_position , (Vec2(coords.x,coords.y)-m_position).GetNormalized() , ( m_position - Vec2( coords.x , coords.y ) ).GetLength() ).didImpact )
	//		{
	//			m_occMap->SetValue( coords , 0.f );
	//		}
	//	}

	//	//m_lastSeenSet = false;
	//}
	//else if(m_occMap!= nullptr )
	//{
	//	if ( m_rotation1Timer->HasElapsed() )
	//	{
	//		m_occMap->PropgateInfluence();
	//		m_rotation1Timer->Reset();
	//	}

	//	for ( int i = 0; i < m_occMap->m_nodes.size(); i++ )
	//	{
	//		IntVec2 coords = m_occMap->m_nodes[ i ].coords;
	//		if ( IsPointInForwardSector2D( Vec2( coords.x , coords.y ) , m_position , m_forwardVec.GetAngleDegrees() + m_orientationDegrees + 90.f , 30.f , 100.f ) && !m_game->RayCast( m_position , ( Vec2( coords.x , coords.y ) - m_position ).GetNormalized() , ( m_position - Vec2( coords.x , coords.y ) ).GetLength() ).didImpact )
	//		{
	//			m_occMap->SetValue( coords , 0.f );
	//		}
	//	}

	//	for ( int i = 0; i < m_occMap->m_nodes.size(); i++ )
	//	{
	//		if ( IsPointInsideDisc2D( Vec2( m_occMap->m_nodes[ i ].coords.x , m_occMap->m_nodes[ i ].coords.y ) , m_position , 30.f ) && !m_game->RayCast(m_position,(Vec2(m_occMap->m_nodes[i].coords.x,m_occMap->m_nodes[i].coords.y)-m_position).GetNormalized(),(m_position-Vec2(m_occMap->m_nodes[i].coords.x,m_occMap->m_nodes[i].coords.y)).GetLength()).didImpact)
	//		{
	//			m_occMap->SetValue(m_occMap->m_nodes[i].coords,0.f);
	//		}
	//	}

	//	m_occMap->SetValue( IntVec2( ( int ) m_position.x , ( int ) m_position.y ) , 0.f );

	//	if ( !m_helper->DoesHavePath() )
	//	{
	//		IntVec2 maxCoords = m_occMap->GetMaxValueCoords();
	//		m_nextMovePosition = m_position;
	//		m_game->GetPathInGame( m_position , Vec2(maxCoords.x,maxCoords.y) , m_helper->path );
	//		m_helper->currentIndex = m_helper->path.size() - 1;
	//	}
	//	else if ( ( m_nextMovePosition - m_position ).GetLengthSquared() <= 0.1f )
	//	{
	//		tileIndex = m_helper->GetNextMove();
	//		if ( tileIndex > 0 )
	//		{
	//			m_nextMovePosition = Vec2( m_game->m_mainMapTiles[ tileIndex ].m_tileCoords.x + 0.5f , m_game->m_mainMapTiles[ tileIndex ].m_tileCoords.y + 0.5f );
	//		}

	//		/*	if ( m_helper->currentIndex == 0 )
	//			{
	//				m_hasPatrolPoint = false;
	//				m_helper->Reset();
	//			}*/
	//		
	//		moveVec = ( m_nextMovePosition - m_position ).GetNormalized();

	//	}
	//	else
	//	{
	//		moveVec = ( m_nextMovePosition - m_position ).GetNormalized();
	//		m_orientationDegrees = moveVec.GetAngleDegrees();
	//		m_position += moveVec * 6.f * deltaseconds;

	//		
	//	}

		
	//}



	
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

	TransformVertexArray( 6 , vertCopy , 3.f , m_orientationDegrees+90.f , m_position );

	g_theRenderer->DrawVertexArray( 6 , vertCopy );

	DebugRender();

}

void MonsterAI::DebugRender()
{
	std::vector<Vertex_PCU> verts;

	std::vector<int>& pathIndices = m_helper->path;

	for ( int i = 0; i < pathIndices.size(); i++ )
	{
		AABB2 aabb = AABB2( m_game->m_mainMapTiles[ pathIndices[ i ] ].m_tileCoords.x , m_game->m_mainMapTiles[ pathIndices[ i ] ].m_tileCoords.y , m_game->m_mainMapTiles[ pathIndices[ i ] ].m_tileCoords.x + 1 , m_game->m_mainMapTiles[ pathIndices[ i ] ].m_tileCoords.y + 1 );
		AppendAABB2( verts , aabb , Rgba8( 0 , 0 , 100 , 100 ) );
	}

	/*if ( m_occupancyMap != nullptr )
	{
		for ( int i = 0; i < m_occupancyMap->m_nodes.size(); i++ )
		{
			if ( m_occupancyMap->m_nodes[ i ].influenceValue > 0 )
			{
				AABB2 aabb = AABB2( m_occupancyMap->m_nodes[ i ].coords.x , m_occupancyMap->m_nodes[ i ].coords.y , m_occupancyMap->m_nodes[ i ].coords.x + 1 , m_occupancyMap->m_nodes[ i ].coords.y + 1 );
				AppendAABB2( verts , aabb , Rgba8( 100 , 100 , 0 , 100 ) );
			}
		}
	}*/

	if ( m_occMap != nullptr )
	{
		float maxValue =m_occMap->GetMaxValue();
		
		for ( int i = 0; i < m_occMap->m_nodes.size(); i++ )
		{
			if ( m_occMap->m_nodes[ i ].value > 0.f )
			{

				Rgba8 baseColor = Rgba8( 255 , 0 , 0 , 255 );
				baseColor.a = RangeMapFloat( 0 , maxValue , 0 , 255 , m_occMap->m_nodes[ i ].value );
				
				/*if ( m_occMap->m_nodes[ i ].value < 70.f )
				{
					baseColor.r = 100;
					baseColor.g = 100;
				}*/
				
				AABB2 aabb = AABB2( m_occMap->m_nodes[ i ].coords.x , m_occMap->m_nodes[ i ].coords.y , m_occMap->m_nodes[ i ].coords.x + 1 , m_occMap->m_nodes[ i ].coords.y + 1 );
				AppendAABB2( verts , aabb , baseColor );
			}
		}
	}

	if ( verts.size() > 0 )
	{
		g_theRenderer->BindTexture( nullptr );
		g_theRenderer->DrawVertexArray( verts );
	}


	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->DrawSector( m_position , m_forwardVec.GetRotatedDegrees( m_orientationDegrees+90.f ) , 100.f ,30.f, Rgba8( 100 , 0 , 0 , 100 ) );
	g_theRenderer->DrawRing( m_position , m_shootingRadius , Rgba8( 100 , 100 , 100 , 100 ) , 0.3f );
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

void MonsterAI::ChangeBehavior()
{
	if ( m_currentBehavior == CHASE_PLAYER )
	{
		if ( !(IsPointInForwardSector2D( m_game->m_greenBeret->m_position , m_position , m_forwardVec.GetAngleDegrees() + m_orientationDegrees + 90.f , 30.f , 100.f ) && !m_game->RayCast( m_position , -( m_position - m_game->m_greenBeret->m_position ).GetNormalized() , ( m_position - m_game->m_greenBeret->m_position ).GetLength() ).didImpact) )
		{
			m_currentBehavior = SEARCH_OCCUPANCY_MAP;
		}

		if ( m_currentBehavior == SEARCH_OCCUPANCY_MAP )
		{

		}

	}
	else if ( IsPointInForwardSector2D( m_game->m_greenBeret->m_position , m_position , m_forwardVec.GetAngleDegrees() + m_orientationDegrees + 90.f , 30.f , 100.f ) && !m_game->RayCast( m_position , -( m_position - m_game->m_greenBeret->m_position ).GetNormalized() , ( m_position - m_game->m_greenBeret->m_position ).GetLength() ).didImpact )
	{
		m_currentBehavior = CHASE_PLAYER;

		if ( IsPointInsideDisc2D( m_position , m_game->m_greenBeret->m_position , m_shootingRadius ) )
		{
			m_currentBehavior = SHOOT_PLAYER;
		}

	}
}

void MonsterAI::CheckPlayerInSight()
{
	if ( IsPointInForwardSector2D( m_game->m_greenBeret->m_position,  m_position , m_forwardVec.GetAngleDegrees()+m_orientationDegrees+90.f , 30.f , 100.f ) && !m_game->RayCast(m_position,-(m_position-m_game->m_greenBeret->m_position).GetNormalized(),(m_position-m_game->m_greenBeret->m_position).GetLength()).didImpact )
	{
		ChangeBehavior( CHASE_PLAYER );
		m_lastSeenPosition = m_game->m_greenBeret->m_position;
		m_lastSeenSet = true;
	}
	else if( m_lastSeenSet)
	{
		if( (m_position - m_lastSeenPosition ).GetLengthSquared() <= 0.1f)
		{
			m_lastSeenSet = false;
		}
		//ChangeBehavior( WANDER );
	}
	else
	{
		ChangeBehavior( PATROL );
	}
}

Vec2 MonsterAI::GetPatrolPoint()
{
	return m_patrolPoints[ m_currentPatrolIndex ];
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

		if ( !(m_game->m_mainMapTiles[ m_game->GetTileIndexForTileCoords( IntVec2( x , y ), true ) ].m_isSolid ) )
		{
			validRoll = true;
		}
	}

	m_game->GetPathInGame( m_position , Vec2( x , y ) , m_helper->path );
	m_helper->currentIndex = (int)m_helper->path.size() - 1;
}
