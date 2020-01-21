#include "Game/Map.hpp"
#include "Game/Player.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Player.hpp"
#include "Game/AIType1.hpp"
#include "Game/AIType2.hpp"
#include "Game/AIType3.hpp"
#include "Game/Collectable.hpp"
#include "Game/SpecialCollectable.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/SimpleTriangleFont.hpp"


Map::Map( Game* game , const IntVec2& tile_dimensions ):m_game(game)
{
	m_tileDimensions = tile_dimensions;
	PopulateTiles();
	m_player = new Player( m_game , Vec2( 1.4f , 1.4f ) );
	SetMapMetaData();
	/*int startTileIndex = GetTileIndexForTileCoords( IntVec2( 1 , 1 ) );
	SetDistanceField( startTileIndex );*/

	for ( int index = 0; index < 900; index++ )
	{
		if ( m_tiles[ index ].m_type == TILE_TYPE_WOOD && m_tiles[ index ].m_hasBeenSeen )
		{
			SetDistanceField( index );
		}
	}
	PoplulateCollectables();
}

void Map::Update( float deltaSeconds )
{
	

	HandleCollissions();
	HandleCollectableCollissions();
	HandleEntityCollissions();
	
	m_player->Update( deltaSeconds );
	m_aiOne->Update( deltaSeconds );
	m_aiTwo->Update( deltaSeconds );
	m_aiThree->Update( deltaSeconds );
	DeleteGarbageEntites();

	if ( g_theInput->WasKeyJustPressed( 0x20 ) )
	{
		RespawnPlayer();
	}

	DetectWinOrLoss();
	
}

void Map::Render() 
{
	Texture* mapTexture = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Terrain_8x8.png" );
	g_theRenderer->BindTexture( mapTexture );
	g_theRenderer->DrawVertexArray( m_tileVertices );
	g_theRenderer->BindTexture( nullptr );
	for ( int index = 0; index < m_collectables.size(); index++ )
	{
		if ( m_collectables[ index ] == nullptr ) continue;
		m_collectables[ index ]->Render();
	}
	for ( int index = 0; index < 5; index++ )
	{
		if ( m_specialCollectables[ index ] == nullptr ) continue;
		m_specialCollectables[ index ]->Render();
	}
	m_player->Render();
	m_aiOne->Render();
	m_aiTwo->Render();
	m_aiThree->Render();

	if ( !IsPlayerAlive() )
	{
		RenderRespawnScreen();
	}

	/*if ( g_theInput->IsLeftMouseButtonPressed() )
	{
		RenderRespawnScreen();
	}*/

	//DisplayDistanceFieldInfo();
}

void Map::RenderRespawnScreen()
{
	AABB2 box = AABB2( Vec2( 0.f , 0.f ) , Vec2( 53.3f , 30.f ) );
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->DrawAABB2D( box , Rgba8( 100 , 100 , 100 , 100 ) );

	Texture* ggTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/respawn.png" );
	AABB2 ggBox = AABB2( m_game->m_camera->GetOrthoBottomLeft().x+ 4.f , m_game->m_camera->GetOrthoBottomLeft().y+ 6.f , m_game->m_camera->GetOrthoBottomLeft().x+ 22.f , m_game->m_camera->GetOrthoBottomLeft().y+ 10.f );

	g_theRenderer->BindTexture( ggTex );
	g_theRenderer->DrawAABB2D( ggBox , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->BindTexture( nullptr );
}

void Map::PopulateTiles()
{
	int numTiles = m_tileDimensions.x * m_tileDimensions.y;
	for ( int index = 0; index < numTiles; index++ )
	{
		int tileX = index % m_tileDimensions.x;
		int tileY = index / m_tileDimensions.x;

		m_tiles.push_back( Tile( tileX , tileY , TILE_TYPE_WOOD ) );
	}

	for ( int tileX = 0; tileX < m_tileDimensions.x; tileX++ )
	{
		SetTileType( tileX , 0 , TILE_TYPE_STONE );
		SetTileType( tileX , m_tileDimensions.y - 1 , TILE_TYPE_STONE );
	}

	for ( int tileY = 0; tileY < m_tileDimensions.y; tileY++ )
	{
		SetTileType( 0 , tileY , TILE_TYPE_STONE );
		SetTileType( m_tileDimensions.x - 1 , tileY , TILE_TYPE_STONE );
	}

	SetTileType( 13 , 12 , TILE_TYPE_ROCK );
	SetTileType( 14 , 12 , TILE_TYPE_ROCK );
	SetTileType( 15 , 12 , TILE_TYPE_ROCK );
	SetTileType( 16 , 12 , TILE_TYPE_ROCK );
	SetTileType( 17 , 12 , TILE_TYPE_ROCK );
	SetTileType( 12 , 12 , TILE_TYPE_ROCK );
	SetTileType( 12 , 13 , TILE_TYPE_ROCK );
	SetTileType( 17 , 13 , TILE_TYPE_ROCK );
	
	SetTileType( 2 , 5 , TILE_TYPE_STONE );
	SetTileType( 2 , 4 , TILE_TYPE_STONE );
	SetTileType( 2 , 3 , TILE_TYPE_STONE );
	SetTileType( 2 , 2 , TILE_TYPE_STONE );
	SetTileType( 4 , 2 , TILE_TYPE_STONE );
	SetTileType( 5 , 2 , TILE_TYPE_STONE );
	SetTileType( 6 , 2 , TILE_TYPE_STONE );
	SetTileType( 7 , 2 , TILE_TYPE_STONE );
	SetTileType( 8 , 2 , TILE_TYPE_STONE );
	SetTileType( 9 , 2 , TILE_TYPE_STONE );
	SetTileType( 10 , 2 , TILE_TYPE_STONE );
	SetTileType( 9 , 6 , TILE_TYPE_STONE );
	SetTileType( 8 , 6 , TILE_TYPE_STONE );
	SetTileType( 7 , 6 , TILE_TYPE_STONE );
	SetTileType( 6 , 6 , TILE_TYPE_STONE );
	SetTileType( 5 , 6 , TILE_TYPE_STONE );
	SetTileType( 4 , 6 , TILE_TYPE_STONE );
	SetTileType( 3 , 6 , TILE_TYPE_STONE );
	SetTileType( 2 , 6 , TILE_TYPE_STONE );
	SetTileType( 5 , 4 , TILE_TYPE_STONE );
	SetTileType( 4 , 4 , TILE_TYPE_STONE );
	SetTileType( 7 , 4 , TILE_TYPE_STONE );
	SetTileType( 8 , 4 , TILE_TYPE_STONE );
	SetTileType( 9 , 4 , TILE_TYPE_STONE );
	SetTileType( 11 , 4 , TILE_TYPE_STONE );
	SetTileType( 11 , 5 , TILE_TYPE_STONE );
	SetTileType( 11 , 6 , TILE_TYPE_STONE );
	SetTileType( 10 , 6 , TILE_TYPE_STONE );
	SetTileType( 12 , 4 , TILE_TYPE_STONE );
	SetTileType( 13 , 4 , TILE_TYPE_STONE );
	SetTileType( 14 , 4 , TILE_TYPE_STONE );
	SetTileType( 15 , 4 , TILE_TYPE_STONE );
	SetTileType( 16 , 4 , TILE_TYPE_STONE );
	SetTileType( 18 , 4 , TILE_TYPE_STONE );
	SetTileType( 18 , 5 , TILE_TYPE_STONE );
	SetTileType( 18 , 6 , TILE_TYPE_STONE );
	SetTileType( 17 , 6 , TILE_TYPE_STONE );
	SetTileType( 16 , 6 , TILE_TYPE_STONE );
	SetTileType( 15 , 6 , TILE_TYPE_STONE );
	SetTileType( 14 , 6 , TILE_TYPE_STONE );
	SetTileType( 13 , 6 , TILE_TYPE_STONE );
	SetTileType( 20 , 6 , TILE_TYPE_STONE );
	SetTileType( 21 , 6 , TILE_TYPE_STONE );
	SetTileType( 22 , 6 , TILE_TYPE_STONE );
	SetTileType( 20 , 5 , TILE_TYPE_STONE );
	SetTileType( 21 , 5 , TILE_TYPE_STONE );
	SetTileType( 22 , 5 , TILE_TYPE_STONE );
	SetTileType( 20 , 4 , TILE_TYPE_STONE );

	//Paint Neeraj
	SetTileType( 2 , 22 , TILE_TYPE_STONE );
	SetTileType( 2 , 23 , TILE_TYPE_STONE );
	SetTileType( 2 , 24 , TILE_TYPE_STONE );
	SetTileType( 2 , 25 , TILE_TYPE_STONE );
	SetTileType( 2 , 26 , TILE_TYPE_STONE );
	SetTileType( 3 , 26 , TILE_TYPE_STONE );
	SetTileType( 4 , 26 , TILE_TYPE_STONE );
	SetTileType( 4 , 25 , TILE_TYPE_STONE );
	SetTileType( 4 , 24 , TILE_TYPE_STONE );
	SetTileType( 4 , 23 , TILE_TYPE_STONE );
	SetTileType( 4 , 22 , TILE_TYPE_STONE );
	SetTileType( 5 , 22 , TILE_TYPE_STONE );
	SetTileType( 6 , 22 , TILE_TYPE_STONE );
	SetTileType( 6 , 23 , TILE_TYPE_STONE );
	SetTileType( 6 , 24 , TILE_TYPE_STONE );
	SetTileType( 6 , 25 , TILE_TYPE_STONE );
	SetTileType( 6 , 26 , TILE_TYPE_STONE );
	SetTileType( 8 , 22 , TILE_TYPE_STONE );
	SetTileType( 8 , 23 , TILE_TYPE_STONE );
	SetTileType( 8 , 24 , TILE_TYPE_STONE );
	SetTileType( 8 , 25 , TILE_TYPE_STONE );
	SetTileType( 8 , 26 , TILE_TYPE_STONE );
	SetTileType( 9 , 22 , TILE_TYPE_STONE );
	SetTileType( 10 , 22 , TILE_TYPE_STONE );
	SetTileType( 9 , 26 , TILE_TYPE_STONE );
	SetTileType( 10 , 26 , TILE_TYPE_STONE );
	SetTileType( 9 , 24 , TILE_TYPE_STONE );
	
	SetTileType( 12 , 22 , TILE_TYPE_STONE );
	SetTileType( 12 , 23 , TILE_TYPE_STONE );
	SetTileType( 12 , 24 , TILE_TYPE_STONE );
	SetTileType( 12 , 25 , TILE_TYPE_STONE );
	SetTileType( 12 , 26 , TILE_TYPE_STONE );
	SetTileType( 13 , 26 , TILE_TYPE_STONE );
	SetTileType( 14 , 26 , TILE_TYPE_STONE );
	SetTileType( 13 , 22 , TILE_TYPE_STONE );
	SetTileType( 14 , 22 , TILE_TYPE_STONE );
	SetTileType( 13 , 24 , TILE_TYPE_STONE );

	SetTileType( 16 , 22 , TILE_TYPE_STONE );
	SetTileType( 16 , 23 , TILE_TYPE_STONE );
	SetTileType( 16 , 24 , TILE_TYPE_STONE );
	SetTileType( 16 , 25 , TILE_TYPE_STONE );
	SetTileType( 16 , 26 , TILE_TYPE_STONE );
	SetTileType( 17 , 26 , TILE_TYPE_STONE );
	SetTileType( 18 , 26 , TILE_TYPE_STONE );
	SetTileType( 17 , 24 , TILE_TYPE_STONE );
	SetTileType( 18 , 24 , TILE_TYPE_STONE );
	SetTileType( 17 , 23 , TILE_TYPE_STONE );
	SetTileType( 18 , 22 , TILE_TYPE_STONE );
	SetTileType( 18 , 25 , TILE_TYPE_STONE );

	SetTileType( 20 , 22 , TILE_TYPE_STONE );
	SetTileType( 20 , 23 , TILE_TYPE_STONE );
	SetTileType( 20 , 24 , TILE_TYPE_STONE );
	SetTileType( 20 , 25 , TILE_TYPE_STONE );
	SetTileType( 20 , 26 , TILE_TYPE_STONE );
	SetTileType( 22 , 22 , TILE_TYPE_STONE );
	SetTileType( 22 , 23 , TILE_TYPE_STONE );
	SetTileType( 22 , 24 , TILE_TYPE_STONE );
	SetTileType( 22 , 25 , TILE_TYPE_STONE );
	SetTileType( 22 , 26 , TILE_TYPE_STONE );
	SetTileType( 21 , 26 , TILE_TYPE_STONE );
	SetTileType( 21 , 24 , TILE_TYPE_STONE );

	SetTileType( 24 , 26 , TILE_TYPE_STONE );
	SetTileType( 25 , 26 , TILE_TYPE_STONE );
	SetTileType( 26 , 26 , TILE_TYPE_STONE );
	SetTileType( 25 , 25 , TILE_TYPE_STONE );
	SetTileType( 25 , 24 , TILE_TYPE_STONE );
	SetTileType( 25 , 23 , TILE_TYPE_STONE );
	SetTileType( 25 , 22 , TILE_TYPE_STONE );
	SetTileType( 24 , 22 , TILE_TYPE_STONE );

	SetTileType( 24 , 4 , TILE_TYPE_STONE );
	SetTileType( 25 , 4 , TILE_TYPE_STONE );
	SetTileType( 26 , 4 , TILE_TYPE_STONE );
	SetTileType( 27 , 4 , TILE_TYPE_STONE );
	SetTileType( 24 , 6 , TILE_TYPE_STONE );
	SetTileType( 27 , 6 , TILE_TYPE_STONE );
	SetTileType( 25 , 6 , TILE_TYPE_STONE );
	SetTileType( 26 , 6 , TILE_TYPE_STONE );

	SetTileType( 15 , 2 , TILE_TYPE_STONE );
	SetTileType( 15 , 1 , TILE_TYPE_STONE );
	SetTileType( 14 , 2 , TILE_TYPE_STONE );
	SetTileType( 13 , 2 , TILE_TYPE_STONE );
	SetTileType( 12 , 2 , TILE_TYPE_STONE );

	SetTileType( 17 , 2 , TILE_TYPE_STONE );
	SetTileType( 18 , 2 , TILE_TYPE_STONE );
	SetTileType( 19 , 2 , TILE_TYPE_STONE );
	SetTileType( 20 , 2 , TILE_TYPE_STONE );
	SetTileType( 21 , 2 , TILE_TYPE_STONE );
	SetTileType( 22 , 2 , TILE_TYPE_STONE );
	SetTileType( 23 , 2 , TILE_TYPE_STONE );
	SetTileType( 24 , 2 , TILE_TYPE_STONE );
	SetTileType( 26 , 2 , TILE_TYPE_STONE );
	SetTileType( 27 , 2 , TILE_TYPE_STONE );
	SetTileType( 28 , 2 , TILE_TYPE_STONE );

	SetTileType( 4 , 8 , TILE_TYPE_STONE );
	SetTileType( 5 , 8 , TILE_TYPE_STONE );
	SetTileType( 6 , 8 , TILE_TYPE_STONE );
	SetTileType( 7 , 8 , TILE_TYPE_STONE );
	SetTileType( 4 , 9 , TILE_TYPE_STONE );
	SetTileType( 5 , 9 , TILE_TYPE_STONE );
	SetTileType( 6 , 9 , TILE_TYPE_STONE );
	SetTileType( 7 , 9 , TILE_TYPE_STONE );
	SetTileType( 4 , 10 , TILE_TYPE_STONE );
	SetTileType( 5 , 10 , TILE_TYPE_STONE );
	SetTileType( 6 , 10 , TILE_TYPE_STONE );
	SetTileType( 7 , 10 , TILE_TYPE_STONE );

	SetTileType( 22 , 8 , TILE_TYPE_STONE );
	SetTileType( 23 , 8 , TILE_TYPE_STONE );
	SetTileType( 24 , 8 , TILE_TYPE_STONE );
	SetTileType( 25 , 8 , TILE_TYPE_STONE );
	SetTileType( 22 , 9 , TILE_TYPE_STONE );
	SetTileType( 23 , 9 , TILE_TYPE_STONE );
	SetTileType( 24 , 9 , TILE_TYPE_STONE );
	SetTileType( 25 , 9 , TILE_TYPE_STONE );
	SetTileType( 22 , 10 , TILE_TYPE_STONE );
	SetTileType( 23 , 10 , TILE_TYPE_STONE );
	SetTileType( 24 , 10 , TILE_TYPE_STONE );
	SetTileType( 25 , 10 , TILE_TYPE_STONE );

	SetTileType( 4 , 12 , TILE_TYPE_STONE );
	SetTileType( 5 , 12 , TILE_TYPE_STONE );
	SetTileType( 6 , 12 , TILE_TYPE_STONE );
	SetTileType( 7 , 12 , TILE_TYPE_STONE );
	SetTileType( 4 , 13 , TILE_TYPE_STONE );
	SetTileType( 5 , 13 , TILE_TYPE_STONE );
	SetTileType( 6 , 13 , TILE_TYPE_STONE );
	SetTileType( 7 , 13 , TILE_TYPE_STONE );
	SetTileType( 4 , 14 , TILE_TYPE_STONE );
	SetTileType( 5 , 14 , TILE_TYPE_STONE );
	SetTileType( 6 , 14 , TILE_TYPE_STONE );
	SetTileType( 7 , 14 , TILE_TYPE_STONE );

	SetTileType( 22 , 12 , TILE_TYPE_STONE );
	SetTileType( 23 , 12 , TILE_TYPE_STONE );
	SetTileType( 24 , 12 , TILE_TYPE_STONE );
	SetTileType( 25 , 12 , TILE_TYPE_STONE );
	SetTileType( 22 , 13 , TILE_TYPE_STONE );
	SetTileType( 23 , 13 , TILE_TYPE_STONE );
	SetTileType( 24 , 13 , TILE_TYPE_STONE );
	SetTileType( 25 , 13 , TILE_TYPE_STONE );
	SetTileType( 22 , 14 , TILE_TYPE_STONE );
	SetTileType( 23 , 14 , TILE_TYPE_STONE );
	SetTileType( 24 , 14 , TILE_TYPE_STONE );
	SetTileType( 25 , 14 , TILE_TYPE_STONE );

	SetTileType( 4 , 16 , TILE_TYPE_STONE );
	SetTileType( 5 , 16 , TILE_TYPE_STONE );
	SetTileType( 6 , 16 , TILE_TYPE_STONE );
	SetTileType( 7 , 16 , TILE_TYPE_STONE );
	SetTileType( 4 , 17 , TILE_TYPE_STONE );
	SetTileType( 5 , 17 , TILE_TYPE_STONE );
	SetTileType( 6 , 17 , TILE_TYPE_STONE );
	SetTileType( 7 , 17 , TILE_TYPE_STONE );
	SetTileType( 4 , 18 , TILE_TYPE_STONE );
	SetTileType( 5 , 18 , TILE_TYPE_STONE );
	SetTileType( 6 , 18 , TILE_TYPE_STONE );
	SetTileType( 7 , 18 , TILE_TYPE_STONE );

	SetTileType( 22 , 16 , TILE_TYPE_STONE );
	SetTileType( 23 , 16 , TILE_TYPE_STONE );
	SetTileType( 24 , 16 , TILE_TYPE_STONE );
	SetTileType( 25 , 16 , TILE_TYPE_STONE );
	SetTileType( 22 , 17 , TILE_TYPE_STONE );
	SetTileType( 23 , 17 , TILE_TYPE_STONE );
	SetTileType( 24 , 17 , TILE_TYPE_STONE );
	SetTileType( 25 , 17 , TILE_TYPE_STONE );
	SetTileType( 22 , 18 , TILE_TYPE_STONE );
	SetTileType( 23 , 18 , TILE_TYPE_STONE );
	SetTileType( 24 , 18 , TILE_TYPE_STONE );
	SetTileType( 25 , 18 , TILE_TYPE_STONE );

	SetTileType( 27 , 8 , TILE_TYPE_STONE );
	SetTileType( 27 , 10 , TILE_TYPE_STONE );
	SetTileType( 27 , 12 , TILE_TYPE_STONE );
	SetTileType( 27 , 14 , TILE_TYPE_STONE );
	SetTileType( 27 , 16 , TILE_TYPE_STONE );
	SetTileType( 27 , 18 , TILE_TYPE_STONE );

	SetTileType( 2 , 8 , TILE_TYPE_STONE );
	SetTileType( 2 , 10 , TILE_TYPE_STONE );
	SetTileType( 2 , 12 , TILE_TYPE_STONE );
	SetTileType( 2 , 14 , TILE_TYPE_STONE );
	SetTileType( 2 , 16 , TILE_TYPE_STONE );
	SetTileType( 2 , 18 , TILE_TYPE_STONE );

	SetTileType( 2 , 20 , TILE_TYPE_STONE );
	SetTileType( 4 , 20 , TILE_TYPE_STONE );
	SetTileType( 6 , 20 , TILE_TYPE_STONE );
	SetTileType( 8 , 20 , TILE_TYPE_STONE );
	SetTileType( 10 , 20 , TILE_TYPE_STONE );
	SetTileType( 12 , 20 , TILE_TYPE_STONE );
	SetTileType( 14, 20 , TILE_TYPE_STONE );
	SetTileType( 16 , 20 , TILE_TYPE_STONE );
	SetTileType( 18 , 20 , TILE_TYPE_STONE );
	SetTileType( 20 , 20 , TILE_TYPE_STONE );
	SetTileType( 22 , 20 , TILE_TYPE_STONE );
	SetTileType( 24 , 20 , TILE_TYPE_STONE );
	SetTileType( 26 , 20 , TILE_TYPE_STONE );


	SetTileType( 17 , 18 , TILE_TYPE_STONE );
	SetTileType( 19 , 18 , TILE_TYPE_STONE );
	SetTileType( 17 , 17 , TILE_TYPE_STONE );
	SetTileType( 19 , 17 , TILE_TYPE_STONE );

	SetTileType( 17 , 9 , TILE_TYPE_STONE );
	SetTileType( 19 , 9 , TILE_TYPE_STONE );
	SetTileType( 17 , 8 , TILE_TYPE_STONE );
	SetTileType( 19 , 8 , TILE_TYPE_STONE );
	SetTileType( 20 , 8 , TILE_TYPE_STONE );

	SetTileType( 14 , 8 , TILE_TYPE_STONE );
	SetTileType( 13 , 8 , TILE_TYPE_STONE );
	SetTileType( 12 , 8 , TILE_TYPE_STONE );
	SetTileType( 11 , 8 , TILE_TYPE_STONE );
	SetTileType( 10 , 8 , TILE_TYPE_STONE );
	SetTileType( 9 , 8 , TILE_TYPE_STONE );
	SetTileType( 9 , 9 , TILE_TYPE_STONE );
	SetTileType( 9 , 10 , TILE_TYPE_STONE );
	SetTileType( 9 , 11 , TILE_TYPE_STONE );
	SetTileType( 9 , 12 , TILE_TYPE_STONE );
	SetTileType( 10 , 12 , TILE_TYPE_STONE );
	SetTileType( 9 , 14 , TILE_TYPE_STONE );
	SetTileType( 9 , 15 , TILE_TYPE_STONE );
	SetTileType( 9 , 16 , TILE_TYPE_STONE );
	SetTileType( 9 , 17 , TILE_TYPE_STONE );
	SetTileType( 11 , 17 , TILE_TYPE_STONE );
	SetTileType( 12 , 17 , TILE_TYPE_STONE );
	SetTileType( 13 , 17 , TILE_TYPE_STONE );
	SetTileType( 14 , 17 , TILE_TYPE_STONE );
	SetTileType( 16 , 17 , TILE_TYPE_STONE );
	SetTileType( 14 , 16 , TILE_TYPE_STONE );
	SetTileType( 14 , 15 , TILE_TYPE_STONE );
	SetTileType( 14 , 10 , TILE_TYPE_STONE );
	SetTileType( 13 , 10 , TILE_TYPE_STONE );
	SetTileType( 12 , 10 , TILE_TYPE_STONE );
	SetTileType( 15 , 10 , TILE_TYPE_STONE );
	SetTileType( 19 , 11 , TILE_TYPE_STONE );
	SetTileType( 19 , 12 , TILE_TYPE_STONE );
	SetTileType( 19 , 13 , TILE_TYPE_STONE );
	SetTileType( 19 , 14 , TILE_TYPE_STONE );



	for ( int verticalIndex = 0; verticalIndex < m_tileDimensions.y; verticalIndex++ )
	{
		for ( int horizontalIndex = 0; horizontalIndex < m_tileDimensions.x; horizontalIndex++ )
		{
			int index = verticalIndex * m_tileDimensions.x + horizontalIndex;
			AABB2 aabb = AABB2( ( float ) horizontalIndex , ( float ) verticalIndex , ( float ) horizontalIndex + 1 , ( float ) verticalIndex + 1 );
			Vec2 minUVs;
			Vec2 maxUvs;

			TileType temp = m_tiles[ index ].m_type;
			TileDefinition::s_definitions[ temp ].m_sprite.GetUVs( minUVs , maxUvs );

			AppendAABB2( m_tileVertices , aabb , Rgba8( 255 , 255 , 255 , 255 ) , minUVs , maxUvs );
		}
	}
}

void Map::PoplulateCollectables()
{
	int numTiles = m_tileDimensions.x * m_tileDimensions.y;
	for ( int index = 0; index < numTiles; index++ )
	{
		int tileX = index % m_tileDimensions.x;
		int tileY = index / m_tileDimensions.x;

		int tileIndex = GetTileIndexForTileCoords( IntVec2( tileX , tileY ) );

		if ( Vec2(tileX,tileY)!= Vec2(RoundDownToInt(m_player->GetPosition().x), RoundDownToInt( m_player->GetPosition().y ) )
			&& m_tiles[ tileIndex ].m_hasBeenSeen )
		{
			Collectable* temp = new Collectable( m_game , Vec2( tileX + 0.5f , tileY + 0.5f ) );
			m_collectables.push_back( temp );
			m_collectableCounter++;
		}
	}

	for ( int index = 0; index < 5; index++ )
	{
		bool passWhileLoop = true;

		while ( passWhileLoop )
		{
			int randomX = rng.GetRandomIntInRange( 1 , 29 );
			int randomY = rng.GetRandomIntInRange( 1 , 29 );

			int randomIndex = GetTileIndexForTileCoords( IntVec2( randomX , randomY ) );

			if ( m_tiles[ randomIndex ].m_hasBeenSeen )
			{
				passWhileLoop = false;
				SpecialCollectable* temp = new SpecialCollectable( m_game , Vec2( randomX + 0.5f , randomY + 0.5f ) );
				m_specialCollectables[ index ] = temp;
			}

		}

		m_SplCollectableCounter++;
	}
}

void Map::SetTileType( int tileX , int tileY , TileType type )
{
	int tileIndex = GetTileIndexForTileCoords( IntVec2( tileX , tileY ) );
	m_tiles[ tileIndex ].m_type = type;
}

int Map::GetTileIndexForTileCoords( const IntVec2& tileCoords ) const
{
	return tileCoords.x + ( m_tileDimensions.x * tileCoords.y );
}

IntVec2 Map::GetTileCoordsForIndex( int index )
{
	return m_tiles[ index ].m_tileCoords;
}

void Map::HandleCollissions()
{
	{
		Vec2 player_position = m_player->GetPosition();

		IntVec2 current_tileCoords = IntVec2( RoundDownToInt( player_position.x ) , RoundDownToInt( player_position.y ) );

		IntVec2 leftCoords = IntVec2( current_tileCoords.x - 1 , current_tileCoords.y );
		IntVec2 rightCoords = IntVec2( current_tileCoords.x + 1 , current_tileCoords.y );
		IntVec2 upCoords = IntVec2( current_tileCoords.x , current_tileCoords.y + 1 );
		IntVec2 downCoords = IntVec2( current_tileCoords.x , current_tileCoords.y - 1 );

		IntVec2 topLeftCoords = IntVec2( current_tileCoords.x - 1 , current_tileCoords.y + 1 );
		IntVec2 bottomLeftCoords = IntVec2( current_tileCoords.x - 1 , current_tileCoords.y - 1 );
		IntVec2 topRightCoords = IntVec2( current_tileCoords.x + 1 , current_tileCoords.y + 1 );
		IntVec2 bottomRightCoords = IntVec2( current_tileCoords.x + 1 , current_tileCoords.y - 1 );

		AABB2 leftAABB = AABB2( ( float ) leftCoords.x , ( float ) leftCoords.y , ( float ) leftCoords.x + 1 , ( float ) leftCoords.y + 1 );
		AABB2 rightAABB = AABB2( ( float ) rightCoords.x , ( float ) rightCoords.y , ( float ) rightCoords.x + 1 , ( float ) rightCoords.y + 1 );
		AABB2 upAABB = AABB2( ( float ) upCoords.x , ( float ) upCoords.y , ( float ) upCoords.x + 1 , ( float ) upCoords.y + 1 );
		AABB2 downAABB = AABB2( ( float ) downCoords.x , ( float ) downCoords.y , ( float ) downCoords.x + 1 , ( float ) downCoords.y + 1 );

		AABB2 topLeftAABB = AABB2( ( float ) topLeftCoords.x , ( float ) topLeftCoords.y , ( float ) topLeftCoords.x + 1 , ( float ) topLeftCoords.y + 1 );
		AABB2 bottomRightAABB = AABB2( ( float ) bottomRightCoords.x , ( float ) bottomRightCoords.y , ( float ) bottomRightCoords.x + 1 , ( float ) bottomRightCoords.y + 1 );
		AABB2 topRightAABB = AABB2( ( float ) topRightCoords.x , ( float ) topRightCoords.y , ( float ) topRightCoords.x + 1 , ( float ) topRightCoords.y + 1 );
		AABB2 bottomLeftAABB = AABB2( ( float ) bottomLeftCoords.x , ( float ) bottomLeftCoords.y , ( float ) bottomLeftCoords.x + 1 , ( float ) bottomLeftCoords.y + 1 );


		if ( !CanEntityMoveOnTile( leftCoords ) )
		{
			PushDiscOutOFAABB2D( player_position , .4f , leftAABB );
		}
		if ( !CanEntityMoveOnTile( rightCoords ) )
		{
			PushDiscOutOFAABB2D( player_position , .4f , rightAABB );
		}
		if ( !CanEntityMoveOnTile( upCoords ) )
		{
			PushDiscOutOFAABB2D( player_position , .4f , upAABB );
		}
		if ( !CanEntityMoveOnTile( downCoords ) )
		{
			PushDiscOutOFAABB2D( player_position , .4f , downAABB );
		}
		if ( !CanEntityMoveOnTile( topRightCoords ) )
		{
			PushDiscOutOFAABB2D( player_position , .4f , topRightAABB );
		}
		if ( !CanEntityMoveOnTile( topLeftCoords ) )
		{
			PushDiscOutOFAABB2D( player_position , .4f , topLeftAABB );
		}
		if ( !CanEntityMoveOnTile( bottomRightCoords ) )
		{
			PushDiscOutOFAABB2D( player_position , .4f , bottomRightAABB );
		}
		if ( !CanEntityMoveOnTile( bottomLeftCoords ) )
		{
			PushDiscOutOFAABB2D( player_position , .4f , bottomLeftAABB );
		}

		m_player->SetPosition( player_position );
	}

	{
		Vec2 aiOne_position = m_aiOne->GetPosition();

		IntVec2 current_tileCoords = IntVec2( RoundDownToInt( aiOne_position.x ) , RoundDownToInt( aiOne_position.y ) );

		IntVec2 leftCoords = IntVec2( current_tileCoords.x - 1 , current_tileCoords.y );
		IntVec2 rightCoords = IntVec2( current_tileCoords.x + 1 , current_tileCoords.y );
		IntVec2 upCoords = IntVec2( current_tileCoords.x , current_tileCoords.y + 1 );
		IntVec2 downCoords = IntVec2( current_tileCoords.x , current_tileCoords.y - 1 );

		IntVec2 topLeftCoords = IntVec2( current_tileCoords.x - 1 , current_tileCoords.y + 1 );
		IntVec2 bottomLeftCoords = IntVec2( current_tileCoords.x - 1 , current_tileCoords.y - 1 );
		IntVec2 topRightCoords = IntVec2( current_tileCoords.x + 1 , current_tileCoords.y + 1 );
		IntVec2 bottomRightCoords = IntVec2( current_tileCoords.x + 1 , current_tileCoords.y - 1 );

		AABB2 leftAABB = AABB2( ( float ) leftCoords.x , ( float ) leftCoords.y , ( float ) leftCoords.x + 1 , ( float ) leftCoords.y + 1 );
		AABB2 rightAABB = AABB2( ( float ) rightCoords.x , ( float ) rightCoords.y , ( float ) rightCoords.x + 1 , ( float ) rightCoords.y + 1 );
		AABB2 upAABB = AABB2( ( float ) upCoords.x , ( float ) upCoords.y , ( float ) upCoords.x + 1 , ( float ) upCoords.y + 1 );
		AABB2 downAABB = AABB2( ( float ) downCoords.x , ( float ) downCoords.y , ( float ) downCoords.x + 1 , ( float ) downCoords.y + 1 );

		AABB2 topLeftAABB = AABB2( ( float ) topLeftCoords.x , ( float ) topLeftCoords.y , ( float ) topLeftCoords.x + 1 , ( float ) topLeftCoords.y + 1 );
		AABB2 bottomRightAABB = AABB2( ( float ) bottomRightCoords.x , ( float ) bottomRightCoords.y , ( float ) bottomRightCoords.x + 1 , ( float ) bottomRightCoords.y + 1 );
		AABB2 topRightAABB = AABB2( ( float ) topRightCoords.x , ( float ) topRightCoords.y , ( float ) topRightCoords.x + 1 , ( float ) topRightCoords.y + 1 );
		AABB2 bottomLeftAABB = AABB2( ( float ) bottomLeftCoords.x , ( float ) bottomLeftCoords.y , ( float ) bottomLeftCoords.x + 1 , ( float ) bottomLeftCoords.y + 1 );


		if ( !CanEntityMoveOnTile( leftCoords ) )
		{
			PushDiscOutOFAABB2D( aiOne_position , .5f , leftAABB );
		}
		if ( !CanEntityMoveOnTile( rightCoords ) )
		{
			PushDiscOutOFAABB2D( aiOne_position , .5f , rightAABB );
		}
		if ( !CanEntityMoveOnTile( upCoords ) )
		{
			PushDiscOutOFAABB2D( aiOne_position , .5f , upAABB );
		}
		if ( !CanEntityMoveOnTile( downCoords ) )
		{
			PushDiscOutOFAABB2D( aiOne_position , .5f , downAABB );
		}
		if ( !CanEntityMoveOnTile( topRightCoords ) )
		{
			PushDiscOutOFAABB2D( aiOne_position , .5f , topRightAABB );
		}
		if ( !CanEntityMoveOnTile( topLeftCoords ) )
		{
			PushDiscOutOFAABB2D( aiOne_position , .5f , topLeftAABB );
		}
		if ( !CanEntityMoveOnTile( bottomRightCoords ) )
		{
			PushDiscOutOFAABB2D( aiOne_position , .5f , bottomRightAABB );
		}
		if ( !CanEntityMoveOnTile( bottomLeftCoords ) )
		{
			PushDiscOutOFAABB2D( aiOne_position , .5f , bottomLeftAABB );
		}

		m_aiOne->SetPosition( aiOne_position );
	}

	{
		Vec2 aiTwo_position = m_aiTwo->GetPosition();

		IntVec2 current_tileCoords = IntVec2( RoundDownToInt( aiTwo_position.x ) , RoundDownToInt( aiTwo_position.y ) );

		IntVec2 leftCoords = IntVec2( current_tileCoords.x - 1 , current_tileCoords.y );
		IntVec2 rightCoords = IntVec2( current_tileCoords.x + 1 , current_tileCoords.y );
		IntVec2 upCoords = IntVec2( current_tileCoords.x , current_tileCoords.y + 1 );
		IntVec2 downCoords = IntVec2( current_tileCoords.x , current_tileCoords.y - 1 );

		IntVec2 topLeftCoords = IntVec2( current_tileCoords.x - 1 , current_tileCoords.y + 1 );
		IntVec2 bottomLeftCoords = IntVec2( current_tileCoords.x - 1 , current_tileCoords.y - 1 );
		IntVec2 topRightCoords = IntVec2( current_tileCoords.x + 1 , current_tileCoords.y + 1 );
		IntVec2 bottomRightCoords = IntVec2( current_tileCoords.x + 1 , current_tileCoords.y - 1 );

		AABB2 leftAABB = AABB2( ( float ) leftCoords.x , ( float ) leftCoords.y , ( float ) leftCoords.x + 1 , ( float ) leftCoords.y + 1 );
		AABB2 rightAABB = AABB2( ( float ) rightCoords.x , ( float ) rightCoords.y , ( float ) rightCoords.x + 1 , ( float ) rightCoords.y + 1 );
		AABB2 upAABB = AABB2( ( float ) upCoords.x , ( float ) upCoords.y , ( float ) upCoords.x + 1 , ( float ) upCoords.y + 1 );
		AABB2 downAABB = AABB2( ( float ) downCoords.x , ( float ) downCoords.y , ( float ) downCoords.x + 1 , ( float ) downCoords.y + 1 );

		AABB2 topLeftAABB = AABB2( ( float ) topLeftCoords.x , ( float ) topLeftCoords.y , ( float ) topLeftCoords.x + 1 , ( float ) topLeftCoords.y + 1 );
		AABB2 bottomRightAABB = AABB2( ( float ) bottomRightCoords.x , ( float ) bottomRightCoords.y , ( float ) bottomRightCoords.x + 1 , ( float ) bottomRightCoords.y + 1 );
		AABB2 topRightAABB = AABB2( ( float ) topRightCoords.x , ( float ) topRightCoords.y , ( float ) topRightCoords.x + 1 , ( float ) topRightCoords.y + 1 );
		AABB2 bottomLeftAABB = AABB2( ( float ) bottomLeftCoords.x , ( float ) bottomLeftCoords.y , ( float ) bottomLeftCoords.x + 1 , ( float ) bottomLeftCoords.y + 1 );


		if ( !CanEntityMoveOnTile( leftCoords ) )
		{
			PushDiscOutOFAABB2D( aiTwo_position , .5f , leftAABB );
		}
		if ( !CanEntityMoveOnTile( rightCoords ) )
		{
			PushDiscOutOFAABB2D( aiTwo_position , .5f , rightAABB );
		}
		if ( !CanEntityMoveOnTile( upCoords ) )
		{
			PushDiscOutOFAABB2D( aiTwo_position , .5f , upAABB );
		}
		if ( !CanEntityMoveOnTile( downCoords ) )
		{
			PushDiscOutOFAABB2D( aiTwo_position , .5f , downAABB );
		}
		if ( !CanEntityMoveOnTile( topRightCoords ) )
		{
			PushDiscOutOFAABB2D( aiTwo_position , .5f , topRightAABB );
		}
		if ( !CanEntityMoveOnTile( topLeftCoords ) )
		{
			PushDiscOutOFAABB2D( aiTwo_position , .5f , topLeftAABB );
		}
		if ( !CanEntityMoveOnTile( bottomRightCoords ) )
		{
			PushDiscOutOFAABB2D( aiTwo_position , .5f , bottomRightAABB );
		}
		if ( !CanEntityMoveOnTile( bottomLeftCoords ) )
		{
			PushDiscOutOFAABB2D( aiTwo_position , .5f , bottomLeftAABB );
		}

		m_aiTwo->SetPosition( aiTwo_position );
	}

	{
		Vec2 aiThree_position = m_aiThree->GetPosition();

		IntVec2 current_tileCoords = IntVec2( RoundDownToInt( aiThree_position.x ) , RoundDownToInt( aiThree_position.y ) );

		IntVec2 leftCoords = IntVec2( current_tileCoords.x - 1 , current_tileCoords.y );
		IntVec2 rightCoords = IntVec2( current_tileCoords.x + 1 , current_tileCoords.y );
		IntVec2 upCoords = IntVec2( current_tileCoords.x , current_tileCoords.y + 1 );
		IntVec2 downCoords = IntVec2( current_tileCoords.x , current_tileCoords.y - 1 );

		IntVec2 topLeftCoords = IntVec2( current_tileCoords.x - 1 , current_tileCoords.y + 1 );
		IntVec2 bottomLeftCoords = IntVec2( current_tileCoords.x - 1 , current_tileCoords.y - 1 );
		IntVec2 topRightCoords = IntVec2( current_tileCoords.x + 1 , current_tileCoords.y + 1 );
		IntVec2 bottomRightCoords = IntVec2( current_tileCoords.x + 1 , current_tileCoords.y - 1 );

		AABB2 leftAABB = AABB2( ( float ) leftCoords.x , ( float ) leftCoords.y , ( float ) leftCoords.x + 1 , ( float ) leftCoords.y + 1 );
		AABB2 rightAABB = AABB2( ( float ) rightCoords.x , ( float ) rightCoords.y , ( float ) rightCoords.x + 1 , ( float ) rightCoords.y + 1 );
		AABB2 upAABB = AABB2( ( float ) upCoords.x , ( float ) upCoords.y , ( float ) upCoords.x + 1 , ( float ) upCoords.y + 1 );
		AABB2 downAABB = AABB2( ( float ) downCoords.x , ( float ) downCoords.y , ( float ) downCoords.x + 1 , ( float ) downCoords.y + 1 );

		AABB2 topLeftAABB = AABB2( ( float ) topLeftCoords.x , ( float ) topLeftCoords.y , ( float ) topLeftCoords.x + 1 , ( float ) topLeftCoords.y + 1 );
		AABB2 bottomRightAABB = AABB2( ( float ) bottomRightCoords.x , ( float ) bottomRightCoords.y , ( float ) bottomRightCoords.x + 1 , ( float ) bottomRightCoords.y + 1 );
		AABB2 topRightAABB = AABB2( ( float ) topRightCoords.x , ( float ) topRightCoords.y , ( float ) topRightCoords.x + 1 , ( float ) topRightCoords.y + 1 );
		AABB2 bottomLeftAABB = AABB2( ( float ) bottomLeftCoords.x , ( float ) bottomLeftCoords.y , ( float ) bottomLeftCoords.x + 1 , ( float ) bottomLeftCoords.y + 1 );


		if ( !CanEntityMoveOnTile( leftCoords ) )
		{
			PushDiscOutOFAABB2D( aiThree_position , .5f , leftAABB );
		}
		if ( !CanEntityMoveOnTile( rightCoords ) )
		{
			PushDiscOutOFAABB2D( aiThree_position , .5f , rightAABB );
		}
		if ( !CanEntityMoveOnTile( upCoords ) )
		{
			PushDiscOutOFAABB2D( aiThree_position , .5f , upAABB );
		}
		if ( !CanEntityMoveOnTile( downCoords ) )
		{
			PushDiscOutOFAABB2D( aiThree_position , .5f , downAABB );
		}
		if ( !CanEntityMoveOnTile( topRightCoords ) )
		{
			PushDiscOutOFAABB2D( aiThree_position , .5f , topRightAABB );
		}
		if ( !CanEntityMoveOnTile( topLeftCoords ) )
		{
			PushDiscOutOFAABB2D( aiThree_position , .5f , topLeftAABB );
		}
		if ( !CanEntityMoveOnTile( bottomRightCoords ) )
		{
			PushDiscOutOFAABB2D( aiThree_position , .5f , bottomRightAABB );
		}
		if ( !CanEntityMoveOnTile( bottomLeftCoords ) )
		{
			PushDiscOutOFAABB2D( aiThree_position , .5f , bottomLeftAABB );
		}

		m_aiThree->SetPosition( aiThree_position );
	}
}

void Map::HandleCollectableCollissions()
{
	for ( int index = 0; index < m_collectables.size(); index++ )
	{
		if ( m_collectables[ index ] != nullptr )
		{
			if ( DoDiscsOverlap( m_player->GetPosition() , 0.4f , m_collectables[ index ]->GetPosition() , 0.2f ) )
			{
				m_collectables[ index ]->Die();
				m_game->points += 2;
				m_collectableCounter--;
			}
		}
	}

	for ( int index = 0; index < 5; index++ )
	{
		if ( m_specialCollectables[ index ] != nullptr )
		{
			if ( DoDiscsOverlap( m_player->GetPosition() , 0.4f , m_specialCollectables[ index ]->GetPosition() , 0.35f ) )
			{
				m_specialCollectables[ index ]->Die();
				m_game->points += 5;
				m_SplCollectableCounter--;
				m_aiOne->ChangeState( STATE_FLEE );
				m_aiTwo->ChangeState( STATE_FLEE );
				m_aiThree->ChangeState( STATE_FLEE );
			}
		}
	}
}

void Map::HandleEntityCollissions()
{
	if ( m_aiOne->m_state == STATE_CHASE )
	{
		if ( DoDiscsOverlap( m_player->GetPosition() , 0.4f , m_aiOne->GetPosition() , 0.4f ) )
		{
			m_player->Die();
		}
	}

	if ( m_aiTwo->m_state == STATE_CHASE )
	{
		if ( DoDiscsOverlap( m_player->GetPosition() , 0.4f , m_aiTwo->GetPosition() , 0.4f ) )
		{
			m_player->Die();
		}
	}

	if ( m_aiThree->m_state == STATE_CHASE )
	{
		if ( DoDiscsOverlap( m_player->GetPosition() , 0.4f , m_aiThree->GetPosition() , 0.4f ) )
		{
			m_player->Die();
		}
	}
}

bool Map::CanEntityMoveOnTile( IntVec2 tileCoords )
{
	int tileIndex = GetTileIndexForTileCoords( tileCoords );

	if ( m_tiles[ tileIndex ].m_type == TILE_TYPE_WOOD )
	{
		return true;
	}

	return false;
}

void Map::SetMapMetaData()
{
	bool hasAnythingChanged = true;
	IntVec2 start_tile = IntVec2( 1 , 1 );
	IntVec2 end_tile = IntVec2( m_tileDimensions.x - 2 , m_tileDimensions.y - 2 );

	int startTileIndex = GetTileIndexForTileCoords( start_tile );
	m_tiles[ startTileIndex ].m_hasBeenSeen = true;
	int tilesTraversed = 0;

	while ( hasAnythingChanged )
	{
		hasAnythingChanged = false;
		for ( int currentTileIndex = startTileIndex + 1; currentTileIndex < m_tiles.size(); currentTileIndex++ )
		{
			IntVec2 currentTile = GetTileCoordsForIndex( currentTileIndex );
			IntVec2 northTile = IntVec2( currentTile.x , currentTile.y + 1 );
			IntVec2 southTile = IntVec2( currentTile.x , currentTile.y - 1 );
			IntVec2 eastTile = IntVec2( currentTile.x + 1 , currentTile.y );
			IntVec2 westTile = IntVec2( currentTile.x - 1 , currentTile.y );
			int northTileIndex = GetTileIndexForTileCoords( northTile );
			int southTileIndex = GetTileIndexForTileCoords( southTile );
			int eastTileIndex = GetTileIndexForTileCoords( eastTile );
			int westTileIndex = GetTileIndexForTileCoords( westTile );
			if ( !IsTileSolid( currentTile ) &&
				( m_tiles[ northTileIndex ].m_hasBeenSeen || m_tiles[ southTileIndex ].m_hasBeenSeen ||
					m_tiles[ eastTileIndex ].m_hasBeenSeen || m_tiles[ westTileIndex ].m_hasBeenSeen )
				)
			{
				if ( !m_tiles[ currentTileIndex ].m_hasBeenSeen )
				{
					m_tiles[ currentTileIndex ].m_hasBeenSeen = true;
					tilesTraversed++;
					hasAnythingChanged = true;
				}
			}
		}
	}
}

void Map::SetDistanceField( int goalTileIndex )
{
	for ( int index = 0; index < m_tileDimensions.x * m_tileDimensions.y - 1; index++ )
	{
		m_distanceFields[ goalTileIndex ][ index ] = 9999;
	}

	m_distanceFields[ goalTileIndex ][ goalTileIndex ] = 0;
	bool hasAnythingChanged = true;
	IntVec2 start_tile = IntVec2( 1 , 1 );
	IntVec2 end_tile = IntVec2( m_tileDimensions.x - 2 , m_tileDimensions.y - 2 );
	int startTileIndex = GetTileIndexForTileCoords( start_tile );

	while ( hasAnythingChanged )
	{
		hasAnythingChanged = false;
		for ( int currentTileIndex = startTileIndex; currentTileIndex < m_tiles.size(); currentTileIndex++ )
		{
			if ( m_distanceFields[ goalTileIndex ][ currentTileIndex ] == 9999 )
			{
				continue;
			}

			IntVec2 currentTile = GetTileCoordsForIndex( currentTileIndex );
			IntVec2 northTile = IntVec2( currentTile.x , currentTile.y + 1 );
			IntVec2 southTile = IntVec2( currentTile.x , currentTile.y - 1 );
			IntVec2 eastTile = IntVec2( currentTile.x + 1 , currentTile.y );
			IntVec2 westTile = IntVec2( currentTile.x - 1 , currentTile.y );
			int northTileIndex = GetTileIndexForTileCoords( northTile );
			int southTileIndex = GetTileIndexForTileCoords( southTile );
			int eastTileIndex = GetTileIndexForTileCoords( eastTile );
			int westTileIndex = GetTileIndexForTileCoords( westTile );

			if (northTileIndex>=0 && northTileIndex<900 && !IsTileSolid( northTile ) && m_distanceFields[ goalTileIndex ][ currentTileIndex ] + 1 < m_distanceFields[ goalTileIndex ][ northTileIndex ] )
			{
				m_distanceFields[ goalTileIndex ][ northTileIndex ] = m_distanceFields[ goalTileIndex ][ currentTileIndex ] + 1;
				hasAnythingChanged = true;
			}

			if ( southTileIndex >= 0 && southTileIndex < 900 && !IsTileSolid( southTile ) && m_distanceFields[ goalTileIndex ][ currentTileIndex ] + 1 < m_distanceFields[ goalTileIndex ][ southTileIndex ] )
			{
				m_distanceFields[ goalTileIndex ][ southTileIndex ] = m_distanceFields[ goalTileIndex ][ currentTileIndex ] + 1;
				hasAnythingChanged = true;
			}

			if ( eastTileIndex >= 0 && eastTileIndex < 900 && !IsTileSolid( eastTile ) && m_distanceFields[ goalTileIndex ][ currentTileIndex ] + 1 < m_distanceFields[ goalTileIndex ][ eastTileIndex ] )
			{
				m_distanceFields[ goalTileIndex ][ eastTileIndex ] = m_distanceFields[ goalTileIndex ][ currentTileIndex ] + 1;
				hasAnythingChanged = true;
			}

			if ( westTileIndex >= 0 && westTileIndex < 900 && !IsTileSolid( westTile ) && m_distanceFields[ goalTileIndex ][ currentTileIndex ] + 1 < m_distanceFields[ goalTileIndex ][ westTileIndex ] )
			{
				m_distanceFields[ goalTileIndex ][ westTileIndex ] = m_distanceFields[ goalTileIndex ][ currentTileIndex ] + 1;
				hasAnythingChanged = true;
			}
		}
	}
}

bool Map::IsTileSolid( IntVec2 tileCoords )
{
	int tileIndex = GetTileIndexForTileCoords( tileCoords );
	if ( m_tiles[ tileIndex ].m_type != TILE_TYPE_WOOD )
	{
		return true;
	}

	return false;
}

void Map::DisplayDistanceFieldInfo() 
{
	AABB2 box = AABB2( Vec2( 0.f , 0.f ) , Vec2( 30.f , 30.f ) );
	g_theRenderer->DrawAABB2D( box , Rgba8( 100 , 100 , 100 , 100 ) );

	int startTileIndex = GetTileIndexForTileCoords( IntVec2( 1 , 1 ) );

	int numTiles = m_tileDimensions.x * m_tileDimensions.y;
	for ( int index = 0; index < numTiles; index++ )
	{
		int tileX = index % m_tileDimensions.x;
		int tileY = index / m_tileDimensions.x;
		
		int tileIndex = GetTileIndexForTileCoords( IntVec2( tileX , tileY ) );
		int value = m_distanceFields[ startTileIndex ][ tileIndex ];

		DrawTextTriangles2D( *g_theRenderer , std::to_string( value ) , Vec2( tileX , tileY + 0.2f ) , 0.35f , Rgba8( 0 , 0 , 100 , 255 ) );
	}

}

Vec2 Map::GetPlayerPosition()
{
	return m_player->GetPosition();
}

void Map::StartUp()
{
	m_aiOne = new AIType1( m_game , Vec2( 14.4f , 13.4f ) );
	m_aiTwo = new AIType2( m_game , Vec2( 15.4f , 13.4f ) );
	m_aiThree = new AIType3( m_game , Vec2( 16.4f , 13.4f ) );
}

void Map::DetectWinOrLoss()
{
	if ( m_collectableCounter <= 0 )
	{
		m_game->ChangeState( STATE_WIN );
	}

	if ( m_game->currentLives <= 0 )
	{
		m_game->ChangeState( STATE_GAMEOVER );
	}
}

void Map::DeleteGarbageEntites()
{
	for ( int index = 0; index < m_collectables.size(); index++ )
	{
		if(m_collectables[index]==nullptr ) continue;

		if ( m_collectables[ index ]->m_isGarbage )
		{
			delete m_collectables[ index ];
			m_collectables[ index ] = nullptr;
		}
	}

	for ( int index = 0; index < 5; index++ )
	{
		if ( m_specialCollectables[ index ] == nullptr ) continue;

		if ( m_specialCollectables[ index ]->m_isGarbage )
		{
			delete m_specialCollectables[ index ];
			m_specialCollectables[ index ] = nullptr;
		}
	}
}

bool Map::IsPlayerAlive()
{
	if ( m_player->m_isGarbage )
	{
		return false;
	}

	return true;
}

void Map::RespawnPlayer()
{
	if ( m_game->currentLives > 0 && m_player->m_isGarbage )
	{
		m_player->m_position = Vec2( 1.4f , 1.4f );
		m_player->m_isGarbage = false;
		m_game->currentLives--;
		m_aiOne->ChangeState( STATE_FLEE );
		m_aiTwo->ChangeState( STATE_FLEE );
		m_aiThree->ChangeState( STATE_FLEE );
	}
}
