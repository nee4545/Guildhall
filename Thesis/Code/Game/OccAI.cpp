#include "Game/OccAI.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Player.hpp"
#include "Game/OccupancyMap.hpp"
#include "Game/Game.hpp"
#include "Game/InfluenceMap.hpp"
#include "Game/MainGameMapCreator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/SpriteAnimDefTex.hpp"

constexpr float AI_SEARCH_SPEED = 3.6f;
constexpr float AI_NORMAL_SPEED = 3.6f;
constexpr float AI_ROTATION_SPEED = 250.f;

OccAI::OccAI()
{
	m_helper = new MovingHelper();

	m_animClock = new Clock();
	m_occupancyPropogateTimer = new Timer();
	m_occupancyUpdateTimer = new Timer();

	m_occupancyPropogateTimer->SetSeconds( 0.25f );
	m_occupancyUpdateTimer->SetSeconds( 1.5f );

	LoadAnimations();

	m_vertices[ 0 ] = Vertex_PCU( Vec3( -1.5f , -1.5f , 0.f ) , Rgba8(150 ,150 ,150 , 255 ) , m_minUV );
	m_vertices[ 1 ] = Vertex_PCU( Vec3( 1.5f , -1.5f , 0.f ) , Rgba8( 150, 150, 150, 255 ) , Vec2( m_maxUV.x , m_minUV.y ) );
	m_vertices[ 2 ] = Vertex_PCU( Vec3( -1.5f , 1.5f , 0.f ) , Rgba8( 150, 150, 150, 255 ) , Vec2( m_minUV.x , m_maxUV.y ) );

	m_vertices[ 3 ] = Vertex_PCU( Vec3( 1.5f , -1.5f , 0.f ) , Rgba8(150 ,150 ,150 , 255 ) , Vec2( m_maxUV.x , m_minUV.y ) );
	m_vertices[ 4 ] = Vertex_PCU( Vec3( 1.5f , 1.5f , 0.f ) , Rgba8( 150, 150, 150, 255 ) , m_maxUV );
	m_vertices[ 5 ] = Vertex_PCU( Vec3( -1.5f , 1.5f , 0.f ) , Rgba8(150 ,150 ,150 , 255 ) , Vec2( m_minUV.x , m_maxUV.y ) );
}

OccAI::~OccAI()
{

}

void OccAI::Update( float deltaseconds )
{
	m_time += deltaseconds;

	UpdateBehavior();

	if ( m_type == OCC_AI_SHARED_MAP )
	{
		if ( m_state == OCC_IDLE )
		{
			if ( m_helper->DoesHavePath() )
			{
				m_helper->Reset();
			}
		}
		if ( m_state == OCC_CHASE_PLAYER )
		{
			if ( m_helper->DoesHavePath() )
			{
				m_helper->Reset();
			}

			float distance = ( m_game->m_player->m_position - m_position ).GetLength();
			Vec2 dir = ( m_game->m_player->m_position - m_position ).GetNormalized();
			m_position += dir * AI_NORMAL_SPEED * deltaseconds;

			m_lastSeenPosition = m_game->m_player->m_position;
			m_orientationDegrees =  GetTurnedToward(m_orientationDegrees,dir.GetAngleDegrees(),AI_ROTATION_SPEED*deltaseconds);
		}

		if ( m_state == OCC_SEARCH_MAP )
		{
			OccupancyMap* sharedMap= m_game->m_player->m_aiSharedMap;
			if ( sharedMap == nullptr )
			{
				m_game->m_player->CreateMap();
			}
			else
			{
				if ( sharedMap->IsCleared() )
				{
					//m_state == OCC_IDLE;
					return;
				}

				for ( int i = 0; i < sharedMap->m_nodes.size(); i++ )
				{
					if ( sharedMap->m_nodes[ i ].value <= 0.f )
					{
						continue;
					}

					IntVec2 coords = sharedMap->m_nodes[ i ].coords;
					if ( !m_game->RayCastInOccGame( m_position , ( Vec2( coords.x + 0.5f , coords.y + 0.5f ) - m_position ).GetNormalized() , ( Vec2( coords.x + 0.5f , coords.y + 0.5f ) - m_position ).GetLength() ).didImpact )
					{
						sharedMap->SetValue( coords , 0.f );
					}
				}

				if ( !m_helper->DoesHavePath() )
				{
				    m_destinationSet = false;
					SetDestinationForSharedAI( sharedMap );
				}
				else if ( ( m_nextMovePos - m_position ).GetLengthSquared() <= 0.1f )
				{
					tileIndex = m_helper->GetNextMove();
					if ( tileIndex > 0 )
					{
						m_nextMovePos = Vec2( m_game->m_occMapTiles[ tileIndex ].m_tileCoords.x + 0.5f , m_game->m_occMapTiles[ tileIndex ].m_tileCoords.y + 0.5f );
					}
				}
				else
				{
					Vec2 moveVec = ( m_nextMovePos - m_position ).GetNormalized();
					m_position += moveVec * AI_SEARCH_SPEED * deltaseconds;
					m_orientationDegrees = GetTurnedToward( m_orientationDegrees , moveVec.GetAngleDegrees() , AI_ROTATION_SPEED * deltaseconds );
				}
			}
		}
	}

	
	if ( m_type == OCC_AI_SINGLE_MAP )
	{

		if ( m_state == OCC_IDLE )
		{
			ExecuteIdleBehavior();
		}

		if ( m_state == OCC_CHASE_PLAYER )
		{
			if ( m_helper->DoesHavePath() )
			{
				m_helper->Reset();
			}
			
			float distance = ( m_game->m_player->m_position - m_position ).GetLength();
			Vec2 dir = ( m_game->m_player->m_position - m_position ).GetNormalized();
			m_orientationDegrees = GetTurnedToward( m_orientationDegrees , dir.GetAngleDegrees() , AI_ROTATION_SPEED * deltaseconds );
			m_position += dir * AI_NORMAL_SPEED * deltaseconds;

			m_lastSeenPosition = m_game->m_player->m_position;
			m_lastSeenSet = true;

			if ( m_occupancyMap != nullptr )
			{
				delete m_occupancyMap;
				m_occupancyMap = nullptr;
			}
		}

		if ( m_state == OCC_SEARCH_MAP )
		{
			
			UpdateAndClearSingleOccupancyMap();

			if ( m_lastSeenSet )
			{
				m_helper->Reset();
				Vec2 dir = ( m_lastSeenPosition - m_position ).GetNormalized();
				m_position += dir * AI_NORMAL_SPEED * deltaseconds;

				if ( ( m_position - m_lastSeenPosition ).GetLengthSquared() <= 0.1f )
				{
					m_lastSeenSet = false;
				}

				if ( m_occupancyMap == nullptr )
				{
					m_occupancyMap = new OccupancyMap( m_game , IntVec2( m_game->m_player->m_position.x , m_game->m_player->m_position.y ) , IntVec2( 80 , 45 ) , 1.f );
					m_occupancyPropogateTimer->Reset();
					//m_occupancyMap->PropgateInfluence();
					m_occupancyMap->Propogate();
				}
			}
			else if ( m_occupancyMap != nullptr )
			{

				m_occupancyMap->SetValue( IntVec2( m_position.x , m_position.y ) , 0.f );

				if ( !m_helper->DoesHavePath() )
				{
					m_nextMovePos = m_position;
					IntVec2 maxCoords = m_occupancyMap->GetMaxValueCoords();
					m_currentMaxCoords = maxCoords;
					//m_game->GetPathInOccupancyGame( m_position , Vec2( maxCoords.x , maxCoords.y ) , m_helper->path,false,true );
					m_game->GetPath( m_position , Vec2( maxCoords.x , maxCoords.y ) , m_game->m_mainMapCreator->m_tiles , m_helper->path );
					m_helper->currentIndex = m_helper->path.size() - 1;
				}
				else if ( ( m_nextMovePos - m_position ).GetLengthSquared() <= 0.1f )
				{
					tileIndex = m_helper->GetNextMove();
					if ( tileIndex > 0 )
					{
						m_nextMovePos = Vec2( m_game->m_occMapTiles[ tileIndex ].m_tileCoords.x + 0.5f , m_game->m_occMapTiles[ tileIndex ].m_tileCoords.y + 0.5f );
					}

				}
				else
				{
					Vec2 moveVec = ( m_nextMovePos - m_position ).GetNormalized();
					m_position += moveVec * AI_SEARCH_SPEED * deltaseconds;
					m_orientationDegrees = GetTurnedToward( m_orientationDegrees , moveVec.GetAngleDegrees() , AI_ROTATION_SPEED * deltaseconds );

					/*IntVec2 coords = m_occupancyMap->GetMaxValueCoords();
					if ( m_currentMaxCoords != coords )
					{
						m_helper->Reset();
					}*/
				}
			}
		}
	}
}

void OccAI::Render()
{
	g_theRenderer->BindTexture( m_anims->GetSpriteTextureAtTime(m_time) );
	Vertex_PCU vertCopy[ 6 ];
	memcpy( vertCopy ,m_vertices, sizeof( Vertex_PCU ) * 6 );
	TransformVertexArray( 6 , vertCopy , 0.5f , m_orientationDegrees+90.f , m_position );
	g_theRenderer->DrawVertexArray( 6 , vertCopy );
	g_theRenderer->BindTexture( nullptr );
	/*if(m_type == OCC_AI_SINGLE_MAP )
	{
		g_theRenderer->DrawSector( m_position , Vec2::MakeFromPolarDegrees( m_orientationDegrees ) , m_visionRadius ,30.f, Rgba8( 100 , 0 , 0 , 100 ) );
	}*/
	DebugRender();
}

void OccAI::DebugRender()
{
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

				m_game->m_font->AddVertsForTextInBox2D( fontVerts , aabb , 0.4f , std::to_string( (int)m_occupancyMap->m_nodes[ i ].value ) , Rgba8( 0 , 0 , 0 , 255 ) , 0.5f , Vec2( 0.2f , 0.2f ) );
			}
		}
		g_theRenderer->DrawVertexArray( verts );
		g_theRenderer->BindTexture( m_game->m_font->GetTexture() );
		g_theRenderer->DrawVertexArray( fontVerts );
		g_theRenderer->BindTexture( nullptr );
	}


	std::vector<Vertex_PCU> pathVerts;
	/*std::vector<int>& pathIndices = m_helper->path;

	for ( int i = 0; i < pathIndices.size(); i++ )
	{
		AABB2 aabb = AABB2( m_game->m_occMapTiles[ pathIndices[ i ] ].m_tileCoords.x , m_game->m_occMapTiles[ pathIndices[ i ] ].m_tileCoords.y , m_game->m_occMapTiles[ pathIndices[ i ] ].m_tileCoords.x + 1 , m_game->m_occMapTiles[ pathIndices[ i ] ].m_tileCoords.y + 1 );
		AppendAABB2( pathVerts , aabb , Rgba8( 0 , 0 , 255 , 70 ) );
	}*/

	AABB2 aabb = AABB2( m_currentMaxCoords.x , m_currentMaxCoords.y , m_currentMaxCoords.x + 1 , m_currentMaxCoords.y + 1 );
	AppendAABB2( pathVerts , aabb ,Rgba8( 0 , 0 , 255 , 70 ) );

	/*if ( m_destinationSet )
	{

	}*/

	g_theRenderer->DrawVertexArray( pathVerts );

	std::vector<Vertex_PCU> infMapVerts;

	if ( m_destinationSet )
	{
		InfluenceMap m = InfluenceMap( m_game , m_currentMaxCoords ,IntVec2(5,5), 0.f , 0.f );
		m.Create();

		for ( int i = 0; i < m.m_nodes.size(); i++ )
		{
			AABB2 aabb = AABB2( m.m_nodes[ i ].coords.x , m.m_nodes[ i ].coords.y , m.m_nodes[ i ].coords.x + 1 , m.m_nodes[ i ].coords.y + 1 );
			AppendAABB2( infMapVerts , aabb , Rgba8( 100 , 100 , 100 , 100 ) );
		}
	}

	g_theRenderer->DrawVertexArray( infMapVerts );
}

void OccAI::Die()
{
	m_isGarbage = true;
}

void OccAI::SetVerticesBasedOnAspect( Vec2 aspect )
{

}

void OccAI::UpdateAndClearSingleOccupancyMap()
{
	if ( m_occupancyMap != nullptr )
	{
		if ( m_occupancyPropogateTimer->HasElapsed() )
		{
			//m_occupancyMap->PropgateInfluence();
			//m_occupancyMap->PropogateInnerInfluence();
			m_occupancyMap->Propogate();
			m_occupancyPropogateTimer->Reset();
		}

		//if ( m_occupancyUpdateTimer->HasElapsed() )
		//{
		//	//m_occupancyMap->Update();
		//	//m_occupancyUpdateTimer->Reset();
		//}

		if ( m_occupancyMap->IsCleared() )
		{
			m_state = OCC_IDLE;
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
				/*&& IsPointInForwardSector2D(Vec2(coords.x+0.5f,coords.y+0.5f),m_position,m_orientationDegrees,30.f,m_visionRadius) */ )
			{
				m_occupancyMap->SetValue( coords , 0.f );
			}

			m_occupancyMap->SetValue( IntVec2( m_position.x , m_position.y ) , 0.f );
		}
	}
}

void OccAI::CheckPlayerInSight()
{

}

void OccAI::UpdateBehavior()
{

	if ( m_type == OCC_AI_SHARED_MAP )
	{
		if ( m_state == OCC_IDLE )
		{
			Vec2 direction = ( m_game->m_player->m_position - m_position ).GetNormalized();
			float distance = ( m_game->m_player->m_position - m_position ).GetLength();
			if ( !( m_game->RayCastInOccGame( m_position , direction , distance ).didImpact ) )
			{
				m_state = OCC_CHASE_PLAYER;
			}
		}
		if ( m_state == OCC_CHASE_PLAYER )
		{
			Vec2 direction = ( m_game->m_player->m_position - m_position ).GetNormalized();
			float distance = ( m_game->m_player->m_position - m_position ).GetLength();
			if ( ( m_game->RayCastInOccGame( m_position , direction , distance ).didImpact ) )
			{
				m_state = OCC_SEARCH_MAP;
				for ( int i = 0; i < m_game->m_occAIs.size(); i++ )
				{
					if ( m_game->m_occAIs[ i ] != this )
					{
						if ( m_game->m_occAIs[ i ]->m_type == OCC_AI_SHARED_MAP )
						{
							m_game->m_occAIs[ i ]->m_state = OCC_SEARCH_MAP;
						}
					}
				}
			}
		}

		if ( m_state == OCC_SEARCH_MAP )
		{
			Vec2 direction = ( m_game->m_player->m_position - m_position ).GetNormalized();
			float distance = ( m_game->m_player->m_position - m_position ).GetLength();
			if ( !( m_game->RayCastInOccGame( m_position , direction , distance ).didImpact ) )
			{
				m_state = OCC_CHASE_PLAYER;

				m_game->m_player->DeleteMap();
				if ( m_helper->DoesHavePath() )
				{
					m_helper->Reset();
				}

				for ( int i = 0; i < m_game->m_occAIs.size(); i++ )
				{
					if ( m_game->m_occAIs[ i ] != this )
					{
						if ( m_game->m_occAIs[ i ]->m_type == OCC_AI_SHARED_MAP )
						{
							m_game->m_occAIs[ i ]->m_state = OCC_IDLE;
						}
					}
				}

			}
		}
	}


	if ( m_type == OCC_AI_SINGLE_MAP )
	{
		Vec2 aiToPlayerVec = ( m_game->m_player->m_position - m_position );
		Vec2 playerDirection = aiToPlayerVec.GetNormalized();
		float distance = aiToPlayerVec.GetLength();
		bool aiToPlayerRayCastHitResult = m_game->RayCastInOccGame( m_position , playerDirection , distance ).didImpact;
		bool aiVisionResult = IsPointInForwardSector2D( m_game->m_player->m_position , m_position , m_orientationDegrees , 30.f , m_visionRadius );
		
		if ( m_state == OCC_IDLE )
		{
			if ( !aiToPlayerRayCastHitResult  )
			{
				m_state = OCC_CHASE_PLAYER;
			}
		}

		if ( m_state == OCC_CHASE_PLAYER )
		{
			if ( aiToPlayerRayCastHitResult  )
			{
				m_state = OCC_SEARCH_MAP;
			}
		}

		if ( m_state == OCC_SEARCH_MAP )
		{
			if ( !aiToPlayerRayCastHitResult  )
			{
				m_state = OCC_CHASE_PLAYER;
			}
		}
	}
}

void OccAI::ExecuteIdleBehavior()
{
	if ( m_helper->DoesHavePath() )
	{
		m_helper->Reset();
	}
}

void OccAI::ExecuteChaseBehavior()
{
	
}

void OccAI::LoadAnimations()
{
	std::vector<Texture*> aianims;

	/*Texture* a1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/5LVL/Walk/1.png" );
	Texture* a2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/5LVL/Walk/2.png" );
	Texture* a3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/5LVL/Walk/3.png" );
	Texture* a4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/5LVL/Walk/4.png" );*/

	Texture* a1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/5LVL/Walk/Walk_000.png" );
	Texture* a2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/5LVL/Walk/Walk_001.png" );
	Texture* a3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/5LVL/Walk/Walk_002.png" );
	Texture* a4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/5LVL/Walk/Walk_003.png" );
	Texture* a5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/5LVL/Walk/Walk_004.png" );
	Texture* a6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/5LVL/Walk/Walk_005.png" );
	Texture* a7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/5LVL/Walk/Walk_006.png" );
	Texture* a8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/Monsters/5LVL/Walk/Walk_007.png" );

	aianims.push_back( a1 );
	aianims.push_back( a2 );
	aianims.push_back( a3 );
	aianims.push_back( a4 );
	aianims.push_back( a5 );
	aianims.push_back( a6 );
	aianims.push_back( a7 );
	aianims.push_back( a8 );

	m_anims = new SpriteAnimDefTex( 0 , ( int ) aianims.size() - 1 , 1.f );
	m_anims->m_animations = aianims;
}

void OccAI::SetDestinationForSharedAI( OccupancyMap* sharedMap )
{
	m_nextMovePos = m_position;

	InfluenceMap baseMap = InfluenceMap( m_game , IntVec2(40,22) , IntVec2( 80 , 44 ) , 0.f , 0.f );
	baseMap.Create();

	baseMap.AddFromOccupancyMap( sharedMap );

	for ( int i = 0; i < m_game->m_occAIs.size(); i++ )
	{
		if ( m_game->m_occAIs[ i ] == this || m_game->m_occAIs[ i ] == nullptr || m_game->m_occAIs[ i ]->m_type == OCC_AI_SINGLE_MAP )
		{
			continue;
		}

		if ( m_game->m_occAIs[ i ]->m_destinationSet )
		{
			IntVec2 startPos = IntVec2( m_game->m_occAIs[i]->m_currentMaxCoords );
			InfluenceMap m = InfluenceMap( m_game , startPos , IntVec2( 5 , 5 ) , -5.f , -1.f );
			m.Create();
			baseMap.AddFromMap( &m );
		}
	}

	IntVec2 maxCoords = baseMap.GetMaxValueCoords();
	
	m_game->GetPathInOccupancyGame( m_position , Vec2( maxCoords.x , maxCoords.y ) , m_helper->path , false , true );
	m_helper->currentIndex = m_helper->path.size() - 1;

	m_destinationSet = true;
	m_currentMaxCoords = maxCoords;
}

void OccAI::Move( float deltaSeconds ,float speed, Vec2 dirVec )
{
	m_position += speed * dirVec * deltaSeconds;
}
