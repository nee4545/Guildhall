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
#include "Engine/Renderer/BitmapFont.hpp"
#include "Game/OccupancyMap.hpp"
#include "Game/MainGameMapCreator.hpp"
#include "Game/PotentialFieldPathFinder.hpp"
#include "Game/Bomb.hpp"
#include "Game/SymmetricPotentialField.hpp"
#include "Game/Bomb.hpp"

constexpr float AI_ROTATION_SPEED = 200.f;
constexpr float AI_PATROL_SPEED = 3.f;
constexpr float AI_CHASE_SPEED = 3.f;

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
	m_occMovetimer->SetSeconds( 3.f );

	m_occupancyPropogateTimer = new Timer();
	m_occupancyUpdateTimer = new Timer();

	m_occupancyPropogateTimer->SetSeconds( 0.25f );
	m_occupancyUpdateTimer->SetSeconds( 1.5f );

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
	ChangeBehavior();

	if ( m_currentBehavior == AI_IDLE )
	{
		m_currentState = IDLE;
	}

	if ( m_currentBehavior == PATROL )
	{

		if ( !m_hasPatrolPoint )
		{
			m_nextMovePosition = GetPatrolPoint();
			m_hasPatrolPoint = true;

		}
		else
		{
			m_currentState = MOVING;
			if ( m_currentType == TYPE_1 || m_currentType == TYPE_3 )
			{
				int index = m_game->m_mainMapCreator->GetTileIndexForCoords( IntVec2( m_nextMovePosition.x , m_nextMovePosition.y ) );

				std::vector<FlowField>& flowFields = m_game->m_mainMapCreator->m_finder->m_flowFields[ index ];

				int currentIndex = m_game->m_mainMapCreator->GetTileIndexForCoords( IntVec2( m_position.x , m_position.y ) );

				moveVec = flowFields[ currentIndex ].flowDirection;
				m_orientationDegrees = GetTurnedToward( m_orientationDegrees , moveVec.GetAngleDegrees() , AI_ROTATION_SPEED * deltaseconds );
				m_position += moveVec * AI_PATROL_SPEED * deltaseconds;

				if ( ( m_position - m_nextMovePosition ).GetLengthSquared() <= 1.f )
				{
					m_hasPatrolPoint = false;
					m_currentBehavior = AI_IDLE;
					m_occMovetimer->Reset();
				}
			}

			if ( m_currentType == TYPE_2 )
			{
				if ( !m_helper->DoesHavePath() )
				{
					m_nextMovePosition = m_position;
					GetPath();
					m_helper->currentIndex = m_helper->path.size() - 1;
				}
				else if ( ( m_nextMovePosition - m_position ).GetLengthSquared() <= 0.1f )
				{
					tileIndex = m_helper->GetNextMove();
					if ( tileIndex > 0 )
					{
						m_nextMovePosition = Vec2( m_game->m_occMapTiles[ tileIndex ].m_tileCoords.x + 0.5f , m_game->m_occMapTiles[ tileIndex ].m_tileCoords.y + 0.5f );
					}

				}
				else
				{
					Vec2 moveVec = ( m_nextMovePosition - m_position ).GetNormalized();
					m_position += moveVec * AI_PATROL_SPEED * deltaseconds;
					m_orientationDegrees = GetTurnedToward( m_orientationDegrees , moveVec.GetAngleDegrees() , AI_ROTATION_SPEED * deltaseconds );
				}
			}
		}
	}

	if ( m_currentBehavior == CHASE_PLAYER )
	{
		moveVec = ( m_game->m_player->m_position - m_position ).GetNormalized();
		m_orientationDegrees = GetTurnedToward( m_orientationDegrees , moveVec.GetAngleDegrees() , AI_ROTATION_SPEED * deltaseconds );
		m_position += moveVec * AI_CHASE_SPEED * deltaseconds;

		if ( m_helper->DoesHavePath() )
		{
			m_helper->Reset();
		}

		if ( m_currentType == TYPE_2 )
		{
			for ( int i = 0; i < m_game->m_mainMapCreator->m_bombs.size(); i++ )
			{
				if ( m_game->m_mainMapCreator->m_bombs[ i ] != nullptr )
				{
					if ( m_game->m_mainMapCreator->m_bombs[ i ]->m_symmetricPotentialField->GetBoudingBox().IsPointInside( m_position ) )
					{
						Vec2 dir;
						float value;
						if ( m_game->m_mainMapCreator->m_bombs[ i ]->m_symmetricPotentialField->GetDirectionAndValueForCoords( m_position , dir , value ) )
						{
							m_position += dir * value * deltaseconds;
						}
					}
				}
			}
		}
	}

	if ( m_currentBehavior == SEARCH_OCCUPANCY_MAP )
	{

		UpdateAndClearSingleOccupancyMap();

		if ( m_occupancyMap == nullptr )
		{
			m_occupancyMap = new OccupancyMap( m_game , IntVec2( m_game->m_player->m_position.x , m_game->m_player->m_position.y ) , IntVec2( 80 , 45 ) , 40.f );
			m_occupancyPropogateTimer->Reset();
			m_occupancyMap->PropgateInfluence();
			//m_occupancyMap->Propogate();
		}
		else if ( m_occupancyMap != nullptr )
		{

			m_occupancyMap->SetValue( IntVec2( m_position.x , m_position.y ) , 0.f );

			if ( !m_helper->DoesHavePath() )
			{
				m_nextMovePosition = m_position;
				IntVec2 maxCoords = m_occupancyMap->GetMaxValueCoords();
				//m_currentMaxCoords = maxCoords;
				//m_game->GetPathInOccupancyGame( m_position , Vec2( maxCoords.x , maxCoords.y ) , m_helper->path,false,true );
				m_game->GetPath( m_position , Vec2( maxCoords.x , maxCoords.y ) , m_game->m_mainMapCreator->m_tiles , m_helper->path, false, true, nullptr );
				m_helper->currentIndex = m_helper->path.size() - 1;
			}
			else if ( ( m_nextMovePosition - m_position ).GetLengthSquared() <= 0.1f )
			{
				tileIndex = m_helper->GetNextMove();
				if ( tileIndex > 0 )
				{
					m_nextMovePosition = Vec2( m_game->m_occMapTiles[ tileIndex ].m_tileCoords.x + 0.5f , m_game->m_occMapTiles[ tileIndex ].m_tileCoords.y + 0.5f );
				}

			}
			else
			{
				Vec2 moveVec = ( m_nextMovePosition - m_position ).GetNormalized();
				m_position += moveVec * AI_CHASE_SPEED * deltaseconds;
				m_orientationDegrees = GetTurnedToward( m_orientationDegrees , moveVec.GetAngleDegrees() , AI_ROTATION_SPEED * deltaseconds );

			}
		}

	}
}

void MonsterAI::Render()
{
	switch ( m_currentState )
	{
	case IDLE:
	{
		SpriteDefinition def = m_animationAtWalk->GetSpriteDefAtTime( 0.f );
		def.GetUVs( m_minUV , m_maxUV );
		g_theRenderer->BindTexture( &def.GetTexture() );
		SetVerticesBasedOnAspect( m_aspectAtWalk );
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
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->DrawSector( m_position , m_forwardVec.GetRotatedDegrees( m_orientationDegrees+90.f ) , m_visionRadius ,30.f, Rgba8( 100 , 0 , 0 , 100 ) );

	std::vector<Vertex_PCU> pathVerts;

	for ( int i = 0; i < m_helper->path.size(); i++ )
	{
		AABB2 aabb = AABB2( m_game->m_mainMapCreator->m_tiles[ m_helper->path[ i ] ].m_tileCoords.x , m_game->m_mainMapCreator->m_tiles[ m_helper->path[ i ] ].m_tileCoords.y , m_game->m_mainMapCreator->m_tiles[ m_helper->path[ i ] ].m_tileCoords.x + 1 , m_game->m_mainMapCreator->m_tiles[ m_helper->path[ i ] ].m_tileCoords.y + 1 );
		AppendAABB2( pathVerts , aabb , Rgba8( 0 , 0 , 100 , 100 ) );
	}

	g_theRenderer->DrawVertexArray( pathVerts );

	if ( m_occupancyMap != nullptr )
	{
		std::vector<Vertex_PCU> verts;
		std::vector<Vertex_PCU> fontVerts;
		float maxValue = m_occupancyMap->GetMaxValue();

		for ( int i = 0; i < m_occupancyMap->m_nodes.size(); i++ )
		{
			if ( m_occupancyMap->m_nodes[ i ].value > 0.f )
			{
				Rgba8 baseColor = Rgba8( 255 , 0 , 0 , 0 );
				baseColor.a = RangeMapFloat( 0 , maxValue , 0 , 100 , m_occupancyMap->m_nodes[ i ].value );

				AABB2 aabb = AABB2( m_occupancyMap->m_nodes[ i ].coords.x , m_occupancyMap->m_nodes[ i ].coords.y , m_occupancyMap->m_nodes[ i ].coords.x + 1 , m_occupancyMap->m_nodes[ i ].coords.y + 1 );
				AppendAABB2( verts , aabb , baseColor );

				m_game->m_font->AddVertsForTextInBox2D( fontVerts , aabb , 0.4f , std::to_string( ( int ) m_occupancyMap->m_nodes[ i ].value ) , Rgba8( 0 , 0 , 0 , 255 ) , 0.5f , Vec2( 0.2f , 0.2f ) );
			}
		}
		g_theRenderer->DrawVertexArray( verts );
		g_theRenderer->BindTexture( m_game->m_font->GetTexture() );
		g_theRenderer->DrawVertexArray( fontVerts );
		g_theRenderer->BindTexture( nullptr );
		g_theRenderer->DrawDisc( m_position , m_senseRadius , Rgba8(100,0,0,100) );
		g_theRenderer->BindTexture( nullptr );
	}

	//g_theRenderer->DrawRing( m_position , m_shootingRadius , Rgba8( 100 , 100 , 100 , 100 ) , 0.3f );
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

	Vec2 aiToPlayerVec = ( m_game->m_player->m_position - m_position );
	Vec2 playerDirection = aiToPlayerVec.GetNormalized();
	float distance = aiToPlayerVec.GetLength();
	bool aiToPlayerRayCastHitResult = m_game->RayCastInOccGame( m_position , playerDirection , distance ).didImpact;

	if ( m_currentType == TYPE_1 || m_currentType == TYPE_2 )
	{
		if ( m_currentBehavior == AI_IDLE )
		{
			bool aiVisionResult = IsPointInForwardSector2D( m_game->m_player->m_position , m_position , m_orientationDegrees , 30.f , m_visionRadius );
			if ( !aiToPlayerRayCastHitResult && aiVisionResult )
			{
				m_currentBehavior = CHASE_PLAYER;
			}
			else if ( m_occMovetimer->HasElapsed() )
			{
				if ( m_currentBehavior == AI_IDLE )
				{
					m_currentBehavior = PATROL;
				}
			}
		}

		if ( m_currentBehavior == PATROL )
		{
			bool aiVisionResult = IsPointInForwardSector2D( m_game->m_player->m_position , m_position , m_orientationDegrees , 30.f , m_visionRadius );
			if ( !aiToPlayerRayCastHitResult && aiVisionResult )
			{
				m_currentBehavior = CHASE_PLAYER;
			}
		}

		if ( m_currentBehavior == CHASE_PLAYER )
		{
			bool aiVisionResult = IsPointInForwardSector2D( m_game->m_player->m_position , m_position , m_orientationDegrees , 30.f , m_visionRadius );
			if ( aiToPlayerRayCastHitResult || !aiVisionResult )
			{
				m_currentBehavior = PATROL;
			}
		}
	}

	if ( m_currentType == TYPE_3 )
	{

		if ( m_currentBehavior == AI_IDLE || m_currentBehavior == PATROL || m_currentBehavior == CHASE_PLAYER )
		{
			if ( m_occupancyMap != nullptr )
			{
				delete m_occupancyMap;
				m_occupancyMap = nullptr;
			}
		}

		if ( m_currentBehavior == AI_IDLE )
		{
			bool aiVisionResult = IsPointInForwardSector2D( m_game->m_player->m_position , m_position , m_orientationDegrees , 30.f , m_visionRadius );
			if ( !aiToPlayerRayCastHitResult && aiVisionResult )
			{
				m_currentBehavior = CHASE_PLAYER;
			}
			else if ( m_occMovetimer->HasElapsed() )
			{
				if ( m_currentBehavior == AI_IDLE )
				{
					m_currentBehavior = PATROL;
				}
			}
		}

		if ( m_currentBehavior == PATROL )
		{
			bool aiVisionResult = IsPointInForwardSector2D( m_game->m_player->m_position , m_position , m_orientationDegrees , 30.f , m_visionRadius );
			if ( !aiToPlayerRayCastHitResult && aiVisionResult )
			{
				m_currentBehavior = CHASE_PLAYER;
			}
		}

		if ( m_currentBehavior == CHASE_PLAYER )
		{
			bool aiVisionResult = IsPointInForwardSector2D( m_game->m_player->m_position , m_position , m_orientationDegrees , 30.f , m_visionRadius );
			if ( aiToPlayerRayCastHitResult || !aiVisionResult )
			{
				m_currentBehavior = SEARCH_OCCUPANCY_MAP;
			}
		}

		if ( m_currentBehavior == SEARCH_OCCUPANCY_MAP )
		{

			bool aiVisionResult = IsPointInForwardSector2D( m_game->m_player->m_position , m_position , m_orientationDegrees , 30.f , m_visionRadius );
			if ( !aiToPlayerRayCastHitResult && aiVisionResult )
			{
				m_currentBehavior = CHASE_PLAYER;
			}
		}
	}
}

void MonsterAI::GetPath()
{
	InfluenceMap baseMap = InfluenceMap( m_game , IntVec2( 40 , 22 ) , IntVec2( 80 , 45 ) , 0.f , 0.f );
	baseMap.Create();

	for ( int i = 0; i < m_game->m_mainMapCreator->m_infMaps.size(); i++ )
	{
		baseMap.AddFromMap( m_game->m_mainMapCreator->m_infMaps[ i ] );
	}

	for ( int i = 0; i < m_game->m_mainMapCreator->m_bombs.size(); i++ )
	{
		if ( m_game->m_mainMapCreator->m_bombs[ i ] != nullptr )
		{
			baseMap.AddFromMap( m_game->m_mainMapCreator->m_bombs[ i ]->m_infmap );
		}
	}

	m_game->GetPath( m_position , GetPatrolPoint() , m_game->m_mainMapCreator->m_tiles , m_helper->path , true , false , &baseMap );


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
	MainGameMapCreator* c = m_game->m_mainMapCreator;

	InfluenceMap m = InfluenceMap( m_game , IntVec2( 40 , 22 ) , IntVec2( 80 , 45 ) , 0.f , 0.f );
	m.Create();
	m.ClearInfluence();

	for ( int i = 0; i < c->m_ais.size(); i++ )
	{
		InfluenceMap inf = InfluenceMap( m_game , IntVec2( c->m_ais[ i ]->m_position.x , c->m_ais[ i ]->m_position.y ) , IntVec2( 6 , 6 ) , -6.f , -1.f );
		inf.Create();

		if ( c->m_ais[ i ]->m_currentBehavior == PATROL )
		{
			Vec2 dir = ( c->m_ais[ i ]->m_nextMovePosition - c->m_ais[ i ]->m_position ).GetNormalized();
			Vec2 nextPos = c->m_ais[ i ]->m_position + dir * 3.f * 0.166f;
			InfluenceMap in = InfluenceMap( m_game , IntVec2( nextPos.x , nextPos.y ) , IntVec2( 6 , 6 ) , -6.f , -1.f );

			in.Create();
			m.AddFromMap( &in );
		}

		m.AddFromMap( &inf );
	}

	for ( int i = 0; i < c->m_monsterBases.size(); i++ )
	{
		InfluenceMap inf = InfluenceMap( m_game , IntVec2( c->m_monsterBases[ i ]->GetCenter().x , c->m_monsterBases[ i ]->GetCenter().y ) , IntVec2( 4 , 4 ) , 4.f );
		inf.Create();

		for ( int j = 0; j < inf.m_nodes.size(); j++ )
		{
			if ( inf.m_nodes[ j ].influenceValue > 2.f )
			{
				inf.m_nodes[ j ].influenceValue = 0.f;
			}
		}

		m.AddFromMap( &inf );
	}

	std::vector<IntVec2> points;
	points = m.GetAllCoordsWithValue( 1.f );

	if ( points.size() > 0 )
	{
		int randomNumber = m_game->m_rng->RollRandomIntInRange( 0 , points.size() - 1 );
		IntVec2 point = points[ randomNumber ];
		return Vec2(point.x+0.5f,point.y+0.5f);
	}


	return Vec2( 0.f,0.f );

	//return m_patrolPoints[ m_currentPatrolIndex ];
}

void MonsterAI::UpdateAndClearSingleOccupancyMap()
{
	if ( m_occupancyMap != nullptr )
	{
		if ( m_occupancyPropogateTimer->HasElapsed() )
		{
			m_occupancyMap->PropgateInfluence();
			//m_occupancyMap->PropogateInnerInfluence();
			//m_occupancyMap->Propogate();
			m_occupancyPropogateTimer->Reset();
		}

		if ( m_occupancyUpdateTimer->HasElapsed() )
		{
			m_occupancyMap->Update();
			m_occupancyUpdateTimer->Reset();
		}

		if ( m_occupancyMap->IsCleared() )
		{
			m_currentBehavior = AI_IDLE;
			return;
		}

		for ( int i = 0; i < m_occupancyMap->m_nodes.size(); i++ )
		{
			if ( m_occupancyMap->m_nodes[ i ].value <= 0.f )
			{
				continue;
			}

			IntVec2 coords = m_occupancyMap->m_nodes[ i ].coords;
			if ( !m_game->RayCastInOccGame( m_position , ( Vec2( coords.x + 0.5f , coords.y + 0.5f ) - m_position ).GetNormalized() , ( Vec2( coords.x + 0.5f , coords.y + 0.5f ) - m_position ).GetLength() ).didImpact
				&& (IsPointInForwardSector2D(Vec2(coords.x+0.5f,coords.y+0.5f),m_position,m_orientationDegrees,30.f,m_visionRadius) || IsPointInsideDisc2D( Vec2( coords.x + 0.5f , coords.y + 0.5f ),m_position,m_senseRadius ) ) )
			{
				m_occupancyMap->SetValue( coords , 0.f );
			}

			m_occupancyMap->SetValue( IntVec2( m_position.x , m_position.y ) , 0.f );
		}
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

		if ( !(m_game->m_mainMapTiles[ m_game->GetTileIndexForTileCoords( IntVec2( x , y ), true ) ].m_isSolid ) )
		{
			validRoll = true;
		}
	}

	m_game->GetPathInGame( m_position , Vec2( x , y ) , m_helper->path );
	m_helper->currentIndex = (int)m_helper->path.size() - 1;
}
