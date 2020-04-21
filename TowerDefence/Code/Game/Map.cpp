#include "Game/Map.hpp"
#include "Game/Player.hpp"
#include "Game/HUD.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Hero.hpp"
#include "Game/Wall.hpp"
#include "Game/Spike.hpp"
#include "Game/Dino.hpp"
#include "Game/AIController.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/SpriteAnimDefTex.hpp"


Map::Map( Game* game, const IntVec2& tile_dimensions ):m_game(game),tileDimensions(tile_dimensions)
{
	m_HUD = new HUD();
	m_HUD->m_map = this;

	PopulateTiles();
	//SpawnEnemyUnit( LANE_1 , UNIT_HERO );

	m_enemyController = new AIController();
	m_enemyController->m_map = this;

	LoadTextures();

	playerRunning = new SpriteAnimDefTex( 0 , m_playerRunning.size() - 1 , 1.f );
	playerRunning->m_animations = m_playerRunning;

	playerAttacking = new SpriteAnimDefTex( 0 , m_playerAttacking.size() - 1 , 1.f );
	playerAttacking->m_animations = m_playerAttacking;

	enemyRunning = new SpriteAnimDefTex( 0 , m_enemyRunning.size()-1, 1.f );
	enemyRunning->m_animations = m_enemyRunning;

	enemyAttacking = new SpriteAnimDefTex( 0 , m_enemyAttacking.size() - 1 , 1.f );
	enemyAttacking->m_animations = m_enemyAttacking;
	
	
}

void Map::Update( float deltaSeconds )
{
	UpdatedSelectedUnits();
	SpawnSelectedUnit();
	HandleHeroPositions();
	HandleDinoPositions();
	HandelHerovsHeroCollissions();
	HandelHerovsSpikeCollissions();
	HandelHerovsDinoCollissions();
	m_enemyController->Update();
	

	for ( int i = 0; i < m_Playerheros.size(); i++ )
	{
		if ( m_Playerheros[ i ] == nullptr )
		{
			continue;
		}

		m_Playerheros[ i ]->Update( deltaSeconds );
	}

	for ( int i = 0; i < m_Enemyheros.size(); i++ )
	{
		if ( m_Enemyheros[ i ] == nullptr )
		{
			continue;
		}

		m_Enemyheros[ i ]->Update( deltaSeconds );
	}

	for ( int i = 0; i < m_dinos.size(); i++ )
	{
		if ( m_dinos[ i ] == nullptr )
		{
			continue;
		}

		m_dinos[ i ]->Update( deltaSeconds );
	}

	HandleDeletion();
}


void Map::Render()
{
	Texture* mapTexture = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Terrain_8x8.png" );
	g_theRenderer->BindTexture( mapTexture );
	g_theRenderer->DrawVertexArray( m_tileVertices );
	g_theRenderer->BindTexture( nullptr );
	m_HUD->Render();

	RenderBackRounds();

	m_HUD->RenderSelectedUnit( m_selcetedUnit );

	DisplayLanes();

	for ( int index = 0; index < m_Playerheros.size(); index++ )
	{
		if ( m_Playerheros[ index ] == nullptr )
		{
			continue;
		}
		m_Playerheros[ index ]->Render();
	}

	for ( int i = 0; i < m_Enemyheros.size(); i++ )
	{
		if ( m_Enemyheros[ i ] == nullptr )
		{
			continue;
		}

		m_Enemyheros[ i ]->Render();
	}

	for ( int index = 0; index < m_walls.size(); index++ )
	{
		m_walls[ index ]->Render();
	}

	for ( int index = 0; index < m_spikes.size(); index++ )
	{
		if ( m_spikes[ index ] == nullptr )
		{
			continue;
		}

		m_spikes[ index ]->Render();
	}

	for ( int index = 0; index < m_dinos.size(); index++ )
	{
		if ( m_dinos[ index ] == nullptr )
		{
			continue;
		}
		m_dinos[ index ]->Render();
	}
}

void Map::PopulateTiles()
{

	int numTiles=tileDimensions.x*tileDimensions.y;
	for( int index=0; index<numTiles; index++ )
	{
		int tileX=index % tileDimensions.x;
		int tileY=index / tileDimensions.x;
		
		m_tiles.push_back(Tile(tileX,tileY));
	}

	for ( int verticalIndex = 0; verticalIndex < tileDimensions.y; verticalIndex++ )
	{
		for ( int horizontalIndex = 0; horizontalIndex < tileDimensions.x; horizontalIndex++ )
		{
			int index = verticalIndex * tileDimensions.x + horizontalIndex;
			AABB2 aabb = AABB2( ( float ) horizontalIndex , ( float ) verticalIndex , ( float ) horizontalIndex + 1 , ( float ) verticalIndex + 1 );
			Vec2 minUVs;
			Vec2 maxUvs;

			TileType temp = m_tiles[ index ].m_type;
			TileDefinition::s_definitions[ temp ].m_sprite.GetUVs( minUVs , maxUvs );

			AppendAABB2( m_tileVertices , aabb , Rgba8( 255 , 255 , 255 , 255 ) , minUVs , maxUvs );
		}
	}

}

void Map::RenderBackRounds()
{
	AABB2 d1 = AABB2( 8.f , 18.f , 9.2f , 19.f );
	AABB2 d2 = AABB2( 20.f , 10.f , 21.2f , 11.f );
	AABB2 d3 = AABB2( 30.f , 21.f , 31.2f , 22.f );

	AABB2 healthPlayerTower = AABB2( 0.3f , 3.f , 1.5f , 23.f );
	AABB2 healthEnemyTower = AABB2( 52.2f , 3.f , 53.4f , 23.f );

	AABB2 actualPlayerPlayerHealth = healthPlayerTower;
	actualPlayerPlayerHealth.CarveBoxOffBottom( m_manager.playerHealth * 0.01f );

	AABB2 actualEnemyHealth = healthEnemyTower;
	actualEnemyHealth.CarveBoxOffBottom( m_manager.enemyHealth * 0.01f );

	AABB2 t1 = AABB2( 2.f , 1.f , 3.5f , 4.5f );
	AABB2 t2 = AABB2( 2.f , 2.5f , 3.5f , 6.5f );
	AABB2 t3 = AABB2( 2.f , 4.f , 3.5f , 8.f );
	AABB2 t4 = AABB2( 2.f , 5.5f , 3.5f , 9.5f );
	AABB2 t5 = AABB2( 2.f , 7.f , 3.5f , 11.f );
	AABB2 t6 = AABB2( 2.f , 8.5f , 3.5f , 12.5f );
	AABB2 t7 = AABB2( 2.f , 10.f , 3.5f , 14.f );
	AABB2 t8 = AABB2( 2.f , 11.5f , 3.5f , 15.5f );
	AABB2 t9 = AABB2( 2.f , 13.f , 3.5f , 17.f );
	AABB2 t10 = AABB2( 2.f , 14.5f , 3.5f , 18.5f );
	AABB2 t11 = AABB2( 2.f , 16.f , 3.5f , 20.f );
	AABB2 t12 = AABB2( 2.f , 17.5f , 3.5f , 21.5f );
	AABB2 t13 = AABB2( 2.f , 19.f , 3.5f , 23.f );
	AABB2 t14 = AABB2( 2.f , 20.5f , 3.5f , 24.5f );

	AABB2 e1 = AABB2( 50.5f, 1.f , 52.f , 4.5f );
	AABB2 e2 = AABB2( 50.5f, 2.5f , 52.f , 6.5f );
	AABB2 e3 = AABB2( 50.5f, 4.f , 52.f , 8.f );
	AABB2 e4 = AABB2( 50.5f, 5.5f , 52.f , 9.5f );
	AABB2 e5 = AABB2( 50.5f, 7.f , 52.f , 11.f );
	AABB2 e6 = AABB2( 50.5f, 8.5f , 52.f , 12.5f );
	AABB2 e7 = AABB2( 50.5f, 10.f , 52.f , 14.f );
	AABB2 e8 = AABB2( 50.5f, 11.5f , 52.f , 15.5f );
	AABB2 e9 = AABB2( 50.5f, 13.f , 52.f , 17.f );
	AABB2 e10 = AABB2(50.5f , 14.5f , 52.f , 18.5f );
	AABB2 e11 = AABB2(50.5f , 16.f , 52.f , 20.f );
	AABB2 e12 = AABB2(50.5f , 17.5f , 52.f , 21.5f );
	AABB2 e13 = AABB2(50.5f , 19.f , 52.f , 23.f );
	AABB2 e14 = AABB2(50.5f , 20.5f , 52.f , 24.5f );



	Texture* d1Tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Backrounds/d1.png" );
	g_theRenderer->BindTexture( d1Tex );
	g_theRenderer->DrawAABB2D( d1 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->DrawAABB2D( d2 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->DrawAABB2D( d3 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->BindTexture( nullptr );

	Texture* t1Tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/b1.png" );
	g_theRenderer->BindTexture( t1Tex );
	g_theRenderer->DrawAABB2D( t14 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->DrawAABB2D( t13 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->DrawAABB2D( t12 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->DrawAABB2D( t11 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->DrawAABB2D( t10 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->DrawAABB2D( t9 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->DrawAABB2D( t8 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->DrawAABB2D( t7 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->DrawAABB2D( t6 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->DrawAABB2D( t5 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->DrawAABB2D( t4 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->DrawAABB2D( t3 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->DrawAABB2D( t2 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->DrawAABB2D( t1 , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->BindTexture( nullptr );

	Texture* t2Tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/b1.png" );
	g_theRenderer->BindTexture( t2Tex );
	g_theRenderer->DrawXFlippedAABB2D( e14 , Rgba8(100 ,100 ,100 , 255 ) );
	g_theRenderer->DrawXFlippedAABB2D( e13 , Rgba8(100 ,100 ,100 , 255 ) );
	g_theRenderer->DrawXFlippedAABB2D( e12 , Rgba8(100 ,100 ,100 , 255 ) );
	g_theRenderer->DrawXFlippedAABB2D( e11 , Rgba8(100 ,100 ,100 , 255 ) );
	g_theRenderer->DrawXFlippedAABB2D( e10 , Rgba8(100 ,100 ,100 , 255 ) );
	g_theRenderer->DrawXFlippedAABB2D( e9 , Rgba8( 100, 100 , 100, 255 ) );
	g_theRenderer->DrawXFlippedAABB2D( e8 , Rgba8( 100, 100 , 100, 255 ) );
	g_theRenderer->DrawXFlippedAABB2D( e7 , Rgba8( 100, 100 , 100, 255 ) );
	g_theRenderer->DrawXFlippedAABB2D( e6 , Rgba8( 100, 100 , 100, 255 ) );
	g_theRenderer->DrawXFlippedAABB2D( e5 , Rgba8( 100, 100 , 100, 255 ) );
	g_theRenderer->DrawXFlippedAABB2D( e4 , Rgba8( 100, 100 , 100, 255 ) );
	g_theRenderer->DrawXFlippedAABB2D( e3 , Rgba8( 100, 100 , 100, 255 ) );
	g_theRenderer->DrawXFlippedAABB2D( e2 , Rgba8( 100, 100 , 100, 255 ) );
	g_theRenderer->DrawXFlippedAABB2D( e1 , Rgba8( 100, 100 , 100, 255 ) );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->DrawAABB2D( healthPlayerTower , Rgba8( 255 , 255 , 255 , 100 ));
	g_theRenderer->DrawAABB2D( healthEnemyTower , Rgba8( 255 , 255 , 255 , 100 ));
	g_theRenderer->DrawAABB2D( actualPlayerPlayerHealth , Rgba8( 255 , 0 , 0 , 80 ));
	g_theRenderer->DrawAABB2D( actualEnemyHealth , Rgba8( 255 , 0 , 0 , 100 ));

}



void Map::CreatePlayer()
{

}

void Map::ToggleNoClip()
{

}

void Map::LoadTextures()
{
	//Hero running 
	Texture* t1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Player/Running/p1.png" );
	Texture* t2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Player/Running/p2.png" );
	Texture* t3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Player/Running/p3.png" );
	Texture* t4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Player/Running/p4.png" );
	Texture* t5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Player/Running/p5.png" );
	Texture* t6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Player/Running/p6.png" );
	Texture* t7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Player/Running/p7.png" );
	Texture* t8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Player/Running/p8.png" );
	Texture* t9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Player/Running/p9.png" );
	Texture* t10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Player/Running/p10.png" );
	Texture* t11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Player/Running/p11.png" );
	Texture* t12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Player/Running/p12.png" );

	m_playerRunning.push_back( t1 );
	m_playerRunning.push_back( t2 );
	m_playerRunning.push_back( t3 );
	m_playerRunning.push_back( t4 );
	m_playerRunning.push_back( t5 );
	m_playerRunning.push_back( t6 );
	m_playerRunning.push_back( t7 );
	m_playerRunning.push_back( t8 );
	m_playerRunning.push_back( t9 );
	m_playerRunning.push_back( t10 );
	m_playerRunning.push_back( t11 );
	m_playerRunning.push_back( t12 );

	Texture* a1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Player/Attack/a1.png" );
	Texture* a2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Player/Attack/a2.png" );
	Texture* a3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Player/Attack/a3.png" );
	Texture* a4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Player/Attack/a4.png" );
	Texture* a5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Player/Attack/a5.png" );
	Texture* a6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Player/Attack/a6.png" );
	Texture* a7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Player/Attack/a7.png" );
	Texture* a8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Player/Attack/a8.png" );
	Texture* a9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Player/Attack/a9.png" );
	Texture* a10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Player/Attack/a10.png" );
	Texture* a11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Player/Attack/a11.png" );
	Texture* a12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Player/Attack/a12.png" );

	m_playerAttacking.push_back( a1 );
	m_playerAttacking.push_back( a2 );
	m_playerAttacking.push_back( a3 );
	m_playerAttacking.push_back( a4 );
	m_playerAttacking.push_back( a5 );
	m_playerAttacking.push_back( a6 );
	m_playerAttacking.push_back( a7 );
	m_playerAttacking.push_back( a8 );
	m_playerAttacking.push_back( a9 );
	m_playerAttacking.push_back( a10 );
	m_playerAttacking.push_back( a11 );
	m_playerAttacking.push_back( a12 );

	Texture* e1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Enemy/Running/0_Golem_Running_000.png" );
	Texture* e2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Enemy/Running/0_Golem_Running_001.png" );
	Texture* e3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Enemy/Running/0_Golem_Running_002.png" );
	Texture* e4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Enemy/Running/0_Golem_Running_003.png" );
	Texture* e5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Enemy/Running/0_Golem_Running_004.png" );
	Texture* e6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Enemy/Running/0_Golem_Running_005.png" );
	Texture* e7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Enemy/Running/0_Golem_Running_006.png" );
	Texture* e8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Enemy/Running/0_Golem_Running_007.png" );
	Texture* e9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Enemy/Running/0_Golem_Running_008.png" );
	Texture* e10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Enemy/Running/0_Golem_Running_009.png" );
	Texture* e11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Enemy/Running/0_Golem_Running_010.png" );
	Texture* e12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Enemy/Running/0_Golem_Running_011.png" );

	m_enemyRunning.push_back( e1 );
	m_enemyRunning.push_back( e2 );
	m_enemyRunning.push_back( e3 );
	m_enemyRunning.push_back( e4 );
	m_enemyRunning.push_back( e5 );
	m_enemyRunning.push_back( e6 );
	m_enemyRunning.push_back( e7 );
	m_enemyRunning.push_back( e8 );
	m_enemyRunning.push_back( e9 );
	m_enemyRunning.push_back( e10 );
	m_enemyRunning.push_back( e11 );
	m_enemyRunning.push_back( e12 );

	Texture* d1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Enemy/Attack/0_Golem_Slashing_000.png" );
	Texture* d2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Enemy/Attack/0_Golem_Slashing_001.png" );
	Texture* d3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Enemy/Attack/0_Golem_Slashing_002.png" );
	Texture* d4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Enemy/Attack/0_Golem_Slashing_003.png" );
	Texture* d5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Enemy/Attack/0_Golem_Slashing_004.png" );
	Texture* d6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Enemy/Attack/0_Golem_Slashing_005.png" );
	Texture* d7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Enemy/Attack/0_Golem_Slashing_006.png" );
	Texture* d8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Enemy/Attack/0_Golem_Slashing_007.png" );
	Texture* d9 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Enemy/Attack/0_Golem_Slashing_008.png" );
	Texture* d10 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Enemy/Attack/0_Golem_Slashing_009.png" );
	Texture* d11 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Enemy/Attack/0_Golem_Slashing_010.png" );
	Texture* d12 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Enemy/Attack/0_Golem_Slashing_011.png" );

	m_enemyAttacking.push_back( d1 );
	m_enemyAttacking.push_back( d2 );
	m_enemyAttacking.push_back( d3 );
	m_enemyAttacking.push_back( d4 );
	m_enemyAttacking.push_back( d5 );
	m_enemyAttacking.push_back( d6 );
	m_enemyAttacking.push_back( d7 );
	m_enemyAttacking.push_back( d8 );
	m_enemyAttacking.push_back( d9 );
	m_enemyAttacking.push_back( d10 );
	m_enemyAttacking.push_back( d11 );
	m_enemyAttacking.push_back( d12 );

}

void Map::SetTileType( int tileX, int tileY, TileType type )
{
	int tileIndex=GetTileIndexForTileCoords(IntVec2(tileX,tileY));
	m_tiles[tileIndex].m_type=type;
}

int Map::GetTileIndexForTileCoords( const IntVec2& tileCoords ) const
{
	return tileCoords.x+(tileDimensions.x*tileCoords.y);
}

IntVec2 Map::GetTileCoordsForIndex( int index )
{
	return m_tiles[index].m_tileCoords;
}

void Map::PushEntityOutOfSolid( Entity* entity, const IntVec2& tileCoords )
{
	AABB2 box=AABB2((float)tileCoords.x,(float)tileCoords.y,(float)tileCoords.x+1,(float)tileCoords.y+1);
	PushDiscOutOFAABB2D(entity->m_position,1.f, box);
}

void Map::HandleCollissions()
{
	//collission between spikes and heros


}

void Map::DisplayLanes()
{
	Vec2 mousePos = m_game->GetMousePositionScreenView();

	if ( m_selcetedUnit == UNIT_NONE )
	{
		return;
	}

	if ( m_selcetedUnit == UNIT_DINO && ( m_lane0.IsPointInside( mousePos ) || m_lane1.IsPointInside( mousePos ) || m_lane2.IsPointInside( mousePos ) || m_lane3.IsPointInside( mousePos ) ) )
	{
		g_theRenderer->BindTexture( nullptr );
		g_theRenderer->DrawAABB2D( m_lane0 , Rgba8( 100 , 100 , 100 , 100 ) );
		g_theRenderer->DrawAABB2D( m_lane1 , Rgba8( 100 , 100 , 100 , 100 ) );
		g_theRenderer->DrawAABB2D( m_lane2 , Rgba8( 100 , 100 , 100 , 100 ) );
		g_theRenderer->DrawAABB2D( m_lane3 , Rgba8( 100 , 100 , 100 , 100 ) );
		return;
	}


	g_theRenderer->BindTexture( nullptr );

	if ( m_lane0.IsPointInside( mousePos ) )
	{
		g_theRenderer->DrawAABB2D( m_lane0 , Rgba8( 100 , 100 , 100 , 100 ) );
	}

	if ( m_lane1.IsPointInside( mousePos ) )
	{
		g_theRenderer->DrawAABB2D( m_lane1 , Rgba8( 100 , 100 , 100 , 100 ) );
	}

	if ( m_lane2.IsPointInside( mousePos ) )
	{
		g_theRenderer->DrawAABB2D( m_lane2 , Rgba8( 100 , 100 , 100 , 100 ) );
	}

	if ( m_lane3.IsPointInside( mousePos ) )
	{
		g_theRenderer->DrawAABB2D( m_lane3 , Rgba8( 100 , 100 , 100 , 100 ) );
	}

}

void Map::SpawnSelectedUnit()
{
	Vec2 mousePos = m_game->GetMousePositionScreenView();

	if ( m_selcetedUnit == UNIT_NONE )
	{
		return;
	}

	if ( m_selcetedUnit == UNIT_HERO )
	{
		if ( m_manager.playerCoins < m_manager.playerCurrentHeroCost )
		{
			return;
		}

		if ( m_lane1.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
		{
			Hero* temp = new Hero( m_game , Vec2( 4.f , 9.5f ) );
			temp->m_map = this;
			m_Playerheros.push_back( temp );
			m_selcetedUnit = UNIT_NONE;
			m_manager.playerCoins -= m_manager.playerCurrentHeroCost;
		}

		if ( m_lane0.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
		{
			Hero* temp = new Hero( m_game , Vec2( 4.f , 4.5f ) );
			temp->m_map = this;
			m_Playerheros.push_back( temp );
			m_selcetedUnit = UNIT_NONE;
			m_manager.playerCoins -= m_manager.playerCurrentHeroCost;
		}

		if ( m_lane2.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
		{
			Hero* temp = new Hero( m_game , Vec2( 4.f , 14.5f ) );
			temp->m_map = this;
			m_Playerheros.push_back( temp );
			m_selcetedUnit = UNIT_NONE;
			m_manager.playerCoins -= m_manager.playerCurrentHeroCost;
		}

		if ( m_lane3.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
		{
			Hero* temp = new Hero( m_game , Vec2( 4.f , 19.5f ) );
			temp->m_map = this;
			m_Playerheros.push_back( temp );
			m_selcetedUnit = UNIT_NONE;
			m_manager.playerCoins -= m_manager.playerCurrentHeroCost;
		}

	}

	if ( m_selcetedUnit == UNIT_BOX )
	{
		if ( m_lane1.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
		{
			Wall* temp = new Wall( m_game , Vec2( mousePos.x , 9.5f ) );
			m_walls.push_back( temp );
			m_selcetedUnit = UNIT_NONE;
		}

		if ( m_lane0.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
		{
			Wall* temp = new Wall( m_game , Vec2( mousePos.x , 4.5f ) );
			m_walls.push_back( temp );
			m_selcetedUnit = UNIT_NONE;
		}

		if ( m_lane2.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
		{
			Wall* temp = new Wall( m_game , Vec2( mousePos.x , 14.5f ) );
			m_walls.push_back( temp );
			m_selcetedUnit = UNIT_NONE;
		}

		if ( m_lane3.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
		{
			Wall* temp = new Wall( m_game , Vec2( mousePos.x , 19.5f ) );
			m_walls.push_back( temp );
			m_selcetedUnit = UNIT_NONE;
		}
	}


	if ( m_selcetedUnit == UNIT_SPIKE )
	{

		if ( m_manager.playerCoins < m_manager.playerCurrentSpikeCost )
		{
			return;
		}

		if ( m_lane1.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
		{
			Spike* temp = new Spike( m_game , Vec2( mousePos.x , 9.5f ) );
			m_spikes.push_back( temp );
			m_selcetedUnit = UNIT_NONE;
			m_manager.playerCoins -= m_manager.playerCurrentSpikeCost;
		}

		if ( m_lane0.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
		{
			Spike* temp = new Spike( m_game , Vec2( mousePos.x , 4.5f ) );
			m_spikes.push_back( temp );
			m_selcetedUnit = UNIT_NONE;
			m_manager.playerCoins -= m_manager.playerCurrentSpikeCost;
		}

		if ( m_lane2.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
		{
			Spike* temp = new Spike( m_game , Vec2( mousePos.x , 14.5f ) );
			m_spikes.push_back( temp );
			m_selcetedUnit = UNIT_NONE;
			m_manager.playerCoins -= m_manager.playerCurrentSpikeCost;
		}

		if ( m_lane3.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
		{
			Spike* temp = new Spike( m_game , Vec2( mousePos.x , 19.5f ) );
			m_spikes.push_back( temp );
			m_selcetedUnit = UNIT_NONE;
			m_manager.playerCoins -= m_manager.playerCurrentSpikeCost;
		}
	}

	if ( m_selcetedUnit == UNIT_DINO )
	{

		if ( m_manager.playerCoins < m_manager.playerCurrentDinoCost )
		{
			return;
		}

		if ( ( m_lane0.IsPointInside( mousePos ) || m_lane1.IsPointInside( mousePos ) || m_lane2.IsPointInside( mousePos ) || m_lane3.IsPointInside( mousePos ) ) && g_theInput->WasLeftMouseButtonJustPressed() )
		{
			Dino* temp = new Dino( m_game , Vec2( 5.5f , 9.5f ) );
			m_dinos.push_back( temp );

			Dino* temp1 = new Dino( m_game , Vec2( 5.5f , 4.5f ) );
			m_dinos.push_back( temp1 );

			Dino* temp2 = new Dino( m_game , Vec2( 5.5f , 19.5f ) );
			m_dinos.push_back( temp2 );

			Dino* temp3 = new Dino( m_game , Vec2( 5.5f , 14.5f ) );
			m_dinos.push_back( temp3 );

			m_selcetedUnit = UNIT_NONE;

			m_manager.playerCoins -= m_manager.playerCurrentDinoCost;
		}
	}
}

void Map::HandleHeroPositions()
{
	for ( int i = 0; i < m_Playerheros.size(); i++ )
	{
		if ( m_Playerheros[ i ] == nullptr )
		{
			continue;
		}

		if ( m_Playerheros[ i ]->m_position.x >= 50.f )
		{
			m_Playerheros[ i ]->Die();
			DamageEnemy( 1.f );
			m_manager.playerCoins += m_manager.towerHitPoints;
		}

	}

	for ( int i = 0; i < m_Enemyheros.size(); i++ )
	{
		if ( m_Enemyheros[ i ] == nullptr )
		{
			continue;
		}

		if ( m_Enemyheros[ i ]->m_position.x <= 2.8f )
		{
			m_Enemyheros[ i ]->Die();
			DamagePlayer( 1.f );
			m_manager.enemyCoins += m_manager.towerHitPoints;
		}
	}

}

void Map::HandleDinoPositions()
{
	for ( int i = 0; i < m_dinos.size(); i++ )
	{
		if ( m_dinos[ i ] == nullptr )
		{
			continue;
		}

		if ( m_dinos[ i ]->m_faction == FACTION_GOOD )
		{
			if ( m_dinos[ i ]->m_position.x >= 50.f )
			{
				m_dinos[ i ]->Die();
				DamageEnemy( 5.f );
				m_manager.playerCoins += m_manager.dinoTowerHitPoints;
			}
		}
		else
		{
			if ( m_dinos[ i ]->m_position.x <= 2.8f )
			{
				m_dinos[ i ]->Die();
				DamagePlayer( 5.f );
				m_manager.enemyCoins += m_manager.dinoTowerHitPoints;
			}
		}
	}
}

void Map::DamagePlayer( float damageAmt )
{
	m_manager.playerHealth -= damageAmt;
}

void Map::DamageEnemy( float damageAmt )
{
	m_manager.enemyHealth -= damageAmt;
}

void Map::HandleDeletion()
{
	for( int i = 0; i < m_Playerheros.size(); i++ )
	{
		if ( m_Playerheros[ i ] == nullptr )
		{
			continue;
		}

		if ( m_Playerheros[ i ]->m_isGarbage )
		{
			delete m_Playerheros[ i ];
			m_Playerheros[ i ] = nullptr;
		}
	}

	for ( int i = 0; i < m_Enemyheros.size(); i++ )
	{
		if ( m_Enemyheros[ i ] == nullptr )
		{
			continue;
		}

		if ( m_Enemyheros[ i ]->m_isGarbage )
		{
			delete m_Enemyheros[ i ];
			m_Enemyheros[ i ] = nullptr;
		}
	}


	for ( int i = 0; i < m_spikes.size(); i++ )
	{
		if ( m_spikes[ i ] == nullptr )
		{
			continue;
		}

		if ( m_spikes[ i ]->m_isGarbage )
		{
			delete m_spikes[ i ];
			m_spikes[ i ] = nullptr;
		}
	}

	for ( int i = 0; i < m_dinos.size(); i++ )
	{
		if ( m_dinos[ i ] == nullptr )
		{
			continue;
		}

		if ( m_dinos[ i ]->m_isGarbage )
		{
			delete m_dinos[ i ];
			m_dinos[ i ] = nullptr;
		}
	}
}

void Map::HandelHerovsHeroCollissions()
{
	for ( int i = 0; i < m_Playerheros.size(); i++ )
	{
		if ( m_Playerheros[ i ] == nullptr )
		{
			continue;
		}

		for ( int j = 0; j < m_Enemyheros.size(); j++ )
		{
			if ( m_Enemyheros[ j ] == nullptr )
			{
				continue;
			}

			if ( DoDiscsOverlap( m_Playerheros[ i ]->m_position , 0.5f , m_Enemyheros[ j ]->m_position , 0.5f ) )
			{
				m_Playerheros[ i ]->heroToAttack = m_Enemyheros[ j ];
				m_Enemyheros[ j ]->heroToAttack = m_Playerheros[ i ];
			}
		}
	}
}

void Map::HandelHerovsSpikeCollissions()
{
	for ( int i = 0; i < m_spikes.size(); i++ )
	{
		if ( m_spikes[ i ] == nullptr )
		{
			continue;
		}

		for ( int j = 0; j < m_Playerheros.size(); j++ )
		{
			if ( m_Playerheros[ j ] == nullptr || m_spikes[ i ]->m_faction == FACTION_GOOD )
			{
				continue;
			}

			if ( DoDiscsOverlap( m_spikes[ i ]->m_position , 0.5f , m_Playerheros[ j ]->m_position , 0.5f ) )
			{
				m_Playerheros[ j ]->TakeDamage( 2.f );
				m_spikes[ i ]->Die();
				m_manager.enemyCoins += m_manager.spikeHitPoints;
			}
		}
	}


	for ( int i = 0; i < m_spikes.size(); i++ )
	{
		if ( m_spikes[ i ] == nullptr )
		{
			continue;
		}

		for ( int j = 0; j < m_Enemyheros.size(); j++ )
		{
			if ( m_Enemyheros[ j ] == nullptr || m_spikes[ i ]->m_faction == FACTION_BAD )
			{
				continue;
			}

			if ( DoDiscsOverlap( m_spikes[ i ]->m_position , 0.5f , m_Enemyheros[ j ]->m_position , 0.5f ) )
			{
				m_Enemyheros[ j ]->TakeDamage( 2.f );
				m_spikes[ i ]->Die();
				m_manager.playerCoins += m_manager.spikeHitPoints;
			}
		}
	}

	
}

void Map::HandelHerovsDinoCollissions()
{
	for ( int i = 0; i < m_dinos.size(); i++ )
	{
		if ( m_dinos[ i ] == nullptr )
		{
			continue;
		}

		for ( int j = 0; j < m_Playerheros.size(); j++ )
		{
			if ( m_Playerheros[ j ] == nullptr || m_dinos[ i ]->m_faction == FACTION_GOOD )
			{
				continue;
			}

			if ( DoDiscsOverlap( m_dinos[ i ]->m_position , 0.5f , m_Playerheros[ j ]->m_position , 0.5f ) )
			{
				if ( !m_dinos[ i ]->m_isGarbage )
				{
					m_Playerheros[ j ]->Die();
				}
				m_dinos[ i ]->Die();
			}
		}

		for ( int j = 0; j < m_Enemyheros.size(); j++ )
		{
			if ( m_Enemyheros[ j ] == nullptr || m_dinos[ i ]->m_faction == FACTION_BAD )
			{
				continue;
			}

			if ( DoDiscsOverlap( m_dinos[ i ]->m_position , 0.5f , m_Enemyheros[ j ]->m_position , 0.5f ) )
			{
				if ( !m_dinos[ i ]->m_isGarbage )
				{
					m_Enemyheros[ j ]->Die();
				}
				m_dinos[ i ]->Die();
			}
			
		}
	}
}

int Map::GetNumberOfHerosInLane( int laneNumber )
{
	int toReturn = 0;

	if ( laneNumber == 1 )
	{
		for ( int i = 0; i < m_Playerheros.size(); i++ )
		{
			if ( m_Playerheros[ i ] == nullptr )
			{
				continue;
			}

			if ( m_Playerheros[ i ]->m_position.y == 4.5f )
			{
				toReturn += 1;
			}
		}

		return toReturn;
	}

	if ( laneNumber == 2 )
	{
		for ( int i = 0; i < m_Playerheros.size(); i++ )
		{
			if ( m_Playerheros[ i ] == nullptr )
			{
				continue;
			}

			if ( m_Playerheros[ i ]->m_position.y == 9.5f )
			{
				toReturn += 1;
			}
		}

		return toReturn;
	}

	if ( laneNumber == 3 )
	{
		for ( int i = 0; i < m_Playerheros.size(); i++ )
		{
			if ( m_Playerheros[ i ] == nullptr )
			{
				continue;
			}

			if ( m_Playerheros[ i ]->m_position.y == 14.5f )
			{
				toReturn += 1;
			}
		}

		return toReturn;
	}

	if ( laneNumber == 4 )
	{
		for ( int i = 0; i < m_Playerheros.size(); i++ )
		{
			if ( m_Playerheros[ i ] == nullptr )
			{
				continue;
			}

			if ( m_Playerheros[ i ]->m_position.y == 19.5f )
			{
				toReturn += 1;
			}
		}

		return toReturn;
	}
}

int Map::GetNumberOfSpikesInLane( int laneNumber )
{
	int toReturn = 0;

	if ( laneNumber == 1 )
	{
		for ( int i = 0; i < m_spikes.size(); i++ )
		{
			if ( m_spikes[ i ] == nullptr)
			{
				continue;
			}

			if ( m_spikes[ i ]->m_position.y == 4.5f && m_spikes[ i ]->m_faction == FACTION_GOOD )
			{
				toReturn += 1;
			}
		}

		return toReturn;
	}

	if ( laneNumber == 2 )
	{
		for ( int i = 0; i < m_spikes.size(); i++ )
		{
			if ( m_spikes[ i ] == nullptr )
			{
				continue;
			}

			if ( m_spikes[ i ]->m_position.y == 9.5f && m_spikes[ i ]->m_faction == FACTION_GOOD )
			{
				toReturn += 1;
			}
		}

		return toReturn;
	}

	if ( laneNumber == 3 )
	{
		for ( int i = 0; i < m_spikes.size(); i++ )
		{
			if ( m_spikes[ i ] == nullptr )
			{
				continue;
			}

			if ( m_spikes[ i ]->m_position.y == 14.5f && m_spikes[ i ]->m_faction == FACTION_GOOD )
			{
				toReturn += 1;
			}
		}

		return toReturn;
	}

	if ( laneNumber == 4 )
	{
		for ( int i = 0; i < m_spikes.size(); i++ )
		{
			if ( m_spikes[ i ] == nullptr )
			{
				continue;
			}

			if ( m_spikes[ i ]->m_position.y == 19.5f && m_spikes[ i ]->m_faction == FACTION_GOOD )
			{
				toReturn += 1;
			}
		}

		return toReturn;
	}
}

void Map::IncreasePlayerCoins( int increament )
{
	m_manager.playerCoins += increament;
}

void Map::IncreaseEnemyCoins( int increament )
{
	m_manager.enemyCoins += increament;
}

void Map::SpawnEnemyUnit( Lanes lane , SelectedUnit unit )
{

	if ( unit == UNIT_DINO )
	{
		if ( m_manager.enemyCoins < m_manager.enemyCurrentDinoCost )
		{
			return;
		}

		Dino* temp = new Dino( m_game , Vec2( 50.f , 9.5f ) , FACTION_BAD );
		m_dinos.push_back( temp );

		Dino* temp1 = new Dino( m_game , Vec2( 50.f , 4.5f ) , FACTION_BAD );
		m_dinos.push_back( temp1 );

		Dino* temp2 = new Dino( m_game , Vec2( 50.f , 19.5f ) , FACTION_BAD );
		m_dinos.push_back( temp2 );

		Dino* temp3 = new Dino( m_game , Vec2( 50.f , 14.5f ) , FACTION_BAD );
		m_dinos.push_back( temp3 );

		return;
	}

	if ( m_manager.enemyCoins < m_manager.enemyCurrentHeroCost )
	{
		return;
	}

	switch ( lane )
	{
		case LANE_1:
		{
			if ( unit == UNIT_HERO )
			{
				Hero* temp = new Hero( m_game , Vec2( 50.f , 4.5f ),FACTION_BAD );
				temp->m_map = this;
				m_Enemyheros.push_back( temp );
				m_manager.enemyCoins -= m_manager.enemyCurrentHeroCost;
			}
			break;
		}
	case LANE_2:
	{
		if ( unit == UNIT_HERO )
		{
			Hero* temp = new Hero( m_game , Vec2( 50.f , 9.5f ) , FACTION_BAD );
			temp->m_map = this;
			m_Enemyheros.push_back( temp );
			m_manager.enemyCoins -= m_manager.enemyCurrentHeroCost;
		}
		break;
	}
	case LANE_3:
	{
		if ( unit == UNIT_HERO )
		{
			Hero* temp = new Hero( m_game , Vec2( 50.f , 14.5f ) , FACTION_BAD );
			temp->m_map = this;
			m_Enemyheros.push_back( temp );
			m_manager.enemyCoins -= m_manager.enemyCurrentHeroCost;
		}
		break;
	}
	case LANE_4:
	{
		if ( unit == UNIT_HERO )
		{
			Hero* temp = new Hero( m_game , Vec2( 50.f , 19.5f ) , FACTION_BAD );
			temp->m_map = this;
			m_Enemyheros.push_back( temp );
			m_manager.enemyCoins -= m_manager.enemyCurrentHeroCost;
		}
		break;
	}
	default:
		break;
	}
}

void Map::SpawnEnemySpike( Lanes lane , float xPosition )
{
	if ( m_manager.enemyCoins < m_manager.enemyCurrentSpikeCist )
	{
		return;
	}

	switch ( lane )
	{
	case LANE_1:
	{
		Spike* s = new Spike( m_game , Vec2( xPosition , 4.5f ) , FACTION_BAD );
		m_spikes.push_back( s );
		m_manager.enemyCoins -= m_manager.enemyCurrentSpikeCist;
		break;
	}
	case LANE_2:
	{
		Spike* s = new Spike( m_game , Vec2( xPosition , 9.5f ) , FACTION_BAD );
		m_spikes.push_back( s );
		m_manager.enemyCoins -= m_manager.enemyCurrentSpikeCist;
		break;
	}
	case LANE_3:
	{
		Spike* s = new Spike( m_game , Vec2( xPosition , 14.5f ) , FACTION_BAD );
		m_spikes.push_back( s );
		m_manager.enemyCoins -= m_manager.enemyCurrentSpikeCist;
		break;
	}
	case LANE_4:
	{
		Spike* s = new Spike( m_game , Vec2( xPosition , 19.5f ) , FACTION_BAD );
		m_spikes.push_back( s );
		m_manager.enemyCoins -= m_manager.enemyCurrentSpikeCist;
		break;
	}
	default:
		break;
	}
}

void Map::UpdatedSelectedUnits()
{
	Vec2 mousePos = m_game->GetMousePositionScreenView();

	if ( g_theInput->WasRightMouseButtonJustPressed() )
	{
		m_selcetedUnit = UNIT_NONE;
	}

	if ( m_HUD->m_dinoBox.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
	{
		m_selcetedUnit = UNIT_DINO;
	}

	if ( m_HUD->m_heroBox.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
	{
		m_selcetedUnit = UNIT_HERO;
	}

	if ( m_HUD->m_spikeBox.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
	{
		m_selcetedUnit = UNIT_SPIKE;
	}

	/*if ( m_HUD->m_stoneBox.IsPointInside( mousePos ) && g_theInput->WasLeftMouseButtonJustPressed() )
	{
		m_selcetedUnit = UNIT_BOX;
	}*/
}